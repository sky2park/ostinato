#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <Preferences.h>
#include <string>
#include <string.h>

#define DEVICE_NAME "Ostinato"
#define MANUFACTURER_NAME "sky2park"

#define UART_RX_PIN 20
#define UART_TX_PIN 21
#define UART_BAUD 115200

#define FRAME_HEADER 0xAA
#define EVENT_PRESS 0x01
#define EVENT_RELEASE 0x00
#define CMD_SELECT_BT_SLOT 0x10
#define CMD_CLEAR_BT_SLOT 0x11

#define BT_SLOT_NONE 0x00
#define BT_SLOT_1 0x01
#define BT_SLOT_2 0x02
#define BT_SLOT_3 0x03
#define BT_SLOT_COUNT 3

#define STATUS_BT 0x80

#define BT_STATE_OFF 0x00
#define BT_STATE_ADVERTISING 0x01
#define BT_STATE_CONNECTED 0x02
#define BT_STATE_PAIRING 0x03
#define BT_STATE_ERROR 0x04

#define DEBUG_UART_RX 0

#define REPORT_ID_KEYBOARD 0x01
#define MAX_KEYS 6

struct KeyboardReport {
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[MAX_KEYS];
} __attribute__((packed));


static uint8_t hidReportMap[] = {
  0x05, 0x01,
  0x09, 0x06,
  0xA1, 0x01,
  0x85, REPORT_ID_KEYBOARD,

  0x05, 0x07,
  0x19, 0xE0,
  0x29, 0xE7,
  0x15, 0x00,
  0x25, 0x01,
  0x75, 0x01,
  0x95, 0x08,
  0x81, 0x02,

  0x95, 0x01,
  0x75, 0x08,
  0x81, 0x01,

  0x95, 0x05,
  0x75, 0x01,
  0x05, 0x08,
  0x19, 0x01,
  0x29, 0x05,
  0x91, 0x02,

  0x95, 0x01,
  0x75, 0x03,
  0x91, 0x01,

  0x95, 0x06,
  0x75, 0x08,
  0x15, 0x00,
  0x26, 0xFF, 0x00,
  0x05, 0x07,
  0x19, 0x00,
  0x29, 0xFF,
  0x81, 0x00,

  0xC0
};

HardwareSerial KeyboardUart(1);

NimBLEServer *server = nullptr;
NimBLEHIDDevice *hid = nullptr;
NimBLECharacteristic *keyboardInput = nullptr;

struct SlotPeer {
  bool valid;
  NimBLEAddress address;
};

KeyboardReport report = {0, 0, {0, 0, 0, 0, 0, 0}};
SlotPeer slotPeers[BT_SLOT_COUNT + 1];
bool connected = false;
uint16_t activeConnHandle = 0;
uint8_t activeSlot = BT_SLOT_NONE;
uint8_t pairingSlot = BT_SLOT_NONE;
uint8_t lastStatusSlot = 0xFF;
uint8_t lastStatusState = 0xFF;

bool isModifier(uint8_t keycode) {
  return keycode >= 0xE0 && keycode <= 0xE7;
}

void sendReport() {
  if (!connected || keyboardInput == nullptr) {
    return;
  }

  const uint8_t *data = reinterpret_cast<const uint8_t *>(&report);
  keyboardInput->setValue(data, sizeof(report));
  keyboardInput->notify(data, sizeof(report));
}

void sendBtStatus(uint8_t slot, uint8_t state) {
  if (lastStatusSlot == slot && lastStatusState == state) {
    return;
  }

  lastStatusSlot = slot;
  lastStatusState = state;

  KeyboardUart.write(FRAME_HEADER);
  KeyboardUart.write(STATUS_BT);
  KeyboardUart.write((slot << 4) | (state & 0x0F));

  Serial.print("TX status: slot=");
  Serial.print(slot);
  Serial.print(" state=");
  Serial.println(state);
}

void clearKeys() {
  memset(&report, 0, sizeof(report));
  sendReport();
}

bool isBtSlot(uint8_t slot) {
  return slot >= BT_SLOT_1 && slot <= BT_SLOT_3;
}

void makeSlotKeys(uint8_t slot, char *addressKey, size_t addressKeySize, char *typeKey, size_t typeKeySize) {
  snprintf(addressKey, addressKeySize, "slot%u", slot);
  snprintf(typeKey, typeKeySize, "type%u", slot);
}

void loadSlotPeers() {
  Preferences prefs;

  if (!prefs.begin("bt_slots", false)) {
    Serial.println("Preferences open failed for load");
    return;
  }

  for (uint8_t slot = BT_SLOT_1; slot <= BT_SLOT_3; slot++) {
    char addressKey[8];
    char typeKey[8];
    makeSlotKeys(slot, addressKey, sizeof(addressKey), typeKey, sizeof(typeKey));

    String address = prefs.getString(addressKey, "");
    uint8_t type = prefs.getUChar(typeKey, 0);

    slotPeers[slot].valid = address.length() > 0;
    if (slotPeers[slot].valid) {
      slotPeers[slot].address = NimBLEAddress(std::string(address.c_str()), type);

      Serial.print("Loaded BT slot ");
      Serial.print(slot);
      Serial.print(": ");
      Serial.println(address);
    } else {
      Serial.print("Loaded BT slot ");
      Serial.print(slot);
      Serial.println(": empty");
    }
  }
}

void saveSlotPeer(uint8_t slot, const NimBLEAddress &address) {
  if (!isBtSlot(slot) || address.isNull()) {
    return;
  }

  Preferences prefs;

  if(!prefs.begin("bt_slots", false)) {
    Serial.println("Preferences open failed for save");
    return;
  }

  char addressKey[8];
  char typeKey[8];
  makeSlotKeys(slot, addressKey, sizeof(addressKey), typeKey, sizeof(typeKey));

  String addressText = String(address.toString().c_str());
  uint8_t type = address.getType();

  size_t addressBytes = prefs.putString(addressKey, addressText);
  size_t typeBytes = prefs.putUChar(typeKey, type);

  String readBackAddress = prefs.getString(addressKey, "");
  uint8_t readBackType = prefs.getUChar(typeKey, 0);

  prefs.end();

  slotPeers[slot].valid = readBackAddress.length() > 0;
  if(slotPeers[slot].valid) {
    slotPeers[slot].address = NimBLEAddress(std::string(readBackAddress.c_str()), readBackType);
  }

  Serial.print("BT slot ");
  Serial.print(slot);
  Serial.print(": ");
  Serial.println(addressText);
  
  Serial.print("Saved bytes address/type: ");
  Serial.print(addressBytes);
  Serial.print("/");
  Serial.println(typeBytes);

  Serial.print("Read back slot ");
  Serial.print(slot);
  Serial.print(": ");
  Serial.print(readBackAddress);
  Serial.print(" type=");
  Serial.println(readBackType);
}

uint8_t findSlotByPeer(NimBLEConnInfo &connInfo) {
  NimBLEAddress idAddress = connInfo.getIdAddress();
  NimBLEAddress otaAddress = connInfo.getAddress();

  for (uint8_t slot = BT_SLOT_1; slot <= BT_SLOT_3; slot++) {
    if (!slotPeers[slot].valid) {
      continue;
    }
    
    if (slotPeers[slot].address == idAddress || slotPeers[slot].address == otaAddress) {
      return slot;
    }
  }

  return BT_SLOT_NONE;
}

void clearWhiteList() {
  while (NimBLEDevice::getWhiteListCount() > 0) {
    NimBLEDevice::whiteListRemove(NimBLEDevice::getWhiteListAddress(0));
  }
}

void startAdvertisingForActiveSlot() {
  NimBLEAdvertising *advertising = NimBLEDevice::getAdvertising();

  NimBLEDevice::stopAdvertising();
  clearWhiteList();

  if (activeSlot == BT_SLOT_NONE || !isBtSlot(activeSlot)) {
    return;
  }

  if (slotPeers[activeSlot].valid && pairingSlot != activeSlot) {
    NimBLEDevice::whiteListAdd(slotPeers[activeSlot].address);
    advertising->setScanFilter(false, true);
    Serial.print("BLE advertising whitelist slot ");
    Serial.println(activeSlot);
    sendBtStatus(activeSlot, BT_STATE_ADVERTISING);
  } else {
    advertising->setScanFilter(false, false);
    Serial.println("BLE advertising open");
    sendBtStatus(activeSlot, BT_STATE_PAIRING);
  }

  advertising->start();
  
}

bool peerMatchesSlot(uint8_t slot, NimBLEConnInfo &connInfo) {
  if (!isBtSlot(slot) || !slotPeers[slot].valid) {
    return false;
  }

  NimBLEAddress idAddress = connInfo.getIdAddress();
  NimBLEAddress otaAddress = connInfo.getAddress();

  return slotPeers[slot].address == idAddress || slotPeers[slot].address == otaAddress;
}

NimBLEAddress preferredPeerAddress(NimBLEConnInfo &connInfo) {
  NimBLEAddress idAddress = connInfo.getIdAddress();

  if (!idAddress.isNull()) {
    return idAddress;
  }

  return connInfo.getAddress();
}

void clearBtSlot(uint8_t slot) {
  if (!isBtSlot(slot)) {
    return;
  }

  if (slotPeers[slot].valid) {
    NimBLEDevice::deleteBond(slotPeers[slot].address);
  }

  char addressKey[8];
  char typeKey[8];
  makeSlotKeys(slot, addressKey, sizeof(addressKey), typeKey, sizeof(typeKey));

  Preferences prefs;
  if (prefs.begin("bt_slots", false)) {
    prefs.remove(addressKey);
    prefs.remove(typeKey);
    prefs.end();
  } else {
    Serial.println("Preferences open failed for clear");
  }
  slotPeers[slot].valid = false;
  slotPeers[slot].address = NimBLEAddress();

  if(activeSlot == slot) {
    clearKeys();
    activeSlot = BT_SLOT_NONE;
    pairingSlot = BT_SLOT_NONE;
    NimBLEDevice::stopAdvertising();

    if (connected && server != nullptr) {
      server->disconnect(activeConnHandle);
    }
  }

  Serial.print("BT slot cleared: ");
  Serial.println(slot);
}

void selectBtSlot(uint8_t slot) {
  if (slot != BT_SLOT_NONE && !isBtSlot(slot)) {
    return;
  }

  if(activeSlot == slot && slot != BT_SLOT_NONE) {
    if (!connected) {
      startAdvertisingForActiveSlot();
    }
    return;
  }

  clearKeys();
  activeSlot = slot;

  if (activeSlot == BT_SLOT_NONE) {
    pairingSlot = BT_SLOT_NONE;
    NimBLEDevice::stopAdvertising();
    clearWhiteList();

    if (connected && server != nullptr) {
      Serial.println("Disconnecting current BLE connection");
      server->disconnect(activeConnHandle);
    }

    Serial.println("BT slot none");
    sendBtStatus(BT_SLOT_NONE, BT_STATE_OFF);
    return;
  }

  if (!slotPeers[activeSlot].valid) {
    pairingSlot = activeSlot;
    Serial.print("BT slot ");
    Serial.print(activeSlot);
    Serial.println(" is empty; pairing mode");
    sendBtStatus(activeSlot, BT_STATE_PAIRING);
  } else {
    pairingSlot = BT_SLOT_NONE;
    Serial.print("BT slot selected: ");
    Serial.println(activeSlot);
    sendBtStatus(activeSlot, BT_STATE_ADVERTISING);
  }

  if (connected && server != nullptr) {
    Serial.println("Disconnecting current BLE connection");
    server->disconnect(activeConnHandle);
    return;
  }

  startAdvertisingForActiveSlot();
}

void pressKey(uint8_t keycode) {
  if (keycode == 0x00) {
    return;
  }
  
  if (isModifier(keycode)) {
    report.modifiers |= 1 << (keycode - 0xE0);
    sendReport();
    return;
  }

  for(uint8_t i = 0; i < MAX_KEYS; i++) {
    if (report.keys[i] == keycode) {
      sendReport();
      return;
    }
  }

  for(uint8_t i = 0; i < MAX_KEYS; i++) {
    if(report.keys[i] == 0x00) {
      report.keys[i] = keycode;
      sendReport();
      return;
    }
  }

  //more than 6 normal keys are ignored by this simple keyboard report.
}

void releaseKey(uint8_t keycode) {
  if (keycode == 0x00) {
    return;
  }
  
  if (isModifier(keycode)) {
    report.modifiers &= ~(1 << (keycode - 0xE0));
    sendReport();
    return;
  }

  for(uint8_t i = 0; i < MAX_KEYS; i++) {
    if (report.keys[i] == keycode) {
      report.keys[i] = 0x00;
    }
  }

  sendReport();
}

bool readFrame(uint8_t &eventType, uint8_t &keycode) {
  static uint8_t state = 0;
  static uint8_t event = 0;

  while (KeyboardUart.available() > 0) {
    uint8_t b = static_cast<uint8_t>(KeyboardUart.read());

#if DEBUG_UART_RX
    Serial.print("RX byte: 0x");
    if(b < 0x10) {
      Serial.print("0");
    }
    Serial.println(b, HEX);
#endif

    if (state == 0) {
      if (b == FRAME_HEADER) {
        state = 1;
      }
    } else if (state == 1) {
      if (b == EVENT_PRESS || b == EVENT_RELEASE || b == CMD_SELECT_BT_SLOT || b == CMD_CLEAR_BT_SLOT) {
        event = b;
        state = 2;
      } else if(b != FRAME_HEADER) {
        state = 0;
      }
    } else {
      eventType = event;
      keycode = b;
      state = 0;
      return true;
    }
  }

  return false;
}

class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer *server, NimBLEConnInfo &connInfo) override {
    connected = true;
    activeConnHandle = connInfo.getConnHandle();
    memset(&report, 0, sizeof(report));
    server->updateConnParams(connInfo.getConnHandle(), 12, 24, 0, 200);

    Serial.println("BLE Connected");

    if(activeSlot == BT_SLOT_NONE || !isBtSlot(activeSlot)) {
      Serial.println("No active BT slot; disconnecting");
      server->disconnect(connInfo);
      return;
    }

    if (pairingSlot == activeSlot) {
      Serial.println("Pairing mode connection accepted");
      sendBtStatus(activeSlot, BT_STATE_CONNECTED);
      return;
    }

    if(pairingSlot != activeSlot && slotPeers[activeSlot].valid && !peerMatchesSlot(activeSlot, connInfo)) {
      Serial.println("Connected peer does not match selected slot; disconnecting");
      sendBtStatus(activeSlot, BT_STATE_ERROR);
      server->disconnect(connInfo);
      return;
    }

    sendBtStatus(activeSlot, BT_STATE_CONNECTED);
  }
  
  void onDisconnect(NimBLEServer *server, NimBLEConnInfo &connInfo, int reason) override {
    connected = false;
    activeConnHandle = 0;
    memset(&report, 0, sizeof(report));
    Serial.println("BLE disconnected, advertising again");

    if(activeSlot != BT_SLOT_NONE) {
      startAdvertisingForActiveSlot();
    }
  }

  void onAuthenticationComplete(NimBLEConnInfo &connInfo) override {
    if (!connInfo.isBonded()) {
      Serial.println("BLE authentication failed of not bonded; disconnecting");
      
      if (server != nullptr) {
        server->disconnect(connInfo.getConnHandle());
      }

      return;
    }

    if (isBtSlot(pairingSlot)) {
      uint8_t existingSlot = findSlotByPeer(connInfo);

      if (existingSlot != BT_SLOT_NONE && existingSlot != pairingSlot) {
        Serial.print("Peer already saved in BT slot ");
        Serial.print(existingSlot);
        Serial.println("; disconnecting");

        if (server != nullptr) {
          server->disconnect(connInfo);
        }

        return;
      }

      saveSlotPeer(pairingSlot, preferredPeerAddress(connInfo));
      pairingSlot = BT_SLOT_NONE;
    }
  }

  void onIdentity(NimBLEConnInfo &connInfo) override {
    if (isBtSlot(pairingSlot) && connInfo.isBonded()) {
      uint8_t existingSlot = findSlotByPeer(connInfo);

      if (existingSlot != BT_SLOT_NONE && existingSlot != pairingSlot) {
        Serial.print("Peer already saved in BT slot ");
        Serial.print(existingSlot);
        Serial.println("; disconnecting");

        if (server != nullptr) {
          server->disconnect(connInfo);
        }

        return;
      }

      saveSlotPeer(pairingSlot, preferredPeerAddress(connInfo));
      pairingSlot = BT_SLOT_NONE;
    }
  }
};

ServerCallbacks callbacks;

void setupBle() {
  NimBLEDevice::init(DEVICE_NAME);
  NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
  NimBLEDevice::setSecurityAuth(true, false, true);

  server = NimBLEDevice::createServer();
  server->setCallbacks(&callbacks, false);

  hid = new NimBLEHIDDevice(server);
  keyboardInput = hid->getInputReport(REPORT_ID_KEYBOARD);

  hid->setManufacturer(MANUFACTURER_NAME);
  hid->setPnp(0x02, 0xCafe, 0x4001, 0x0100);
  hid->setHidInfo(0x00, 0x01);
  hid->setReportMap(hidReportMap, sizeof(hidReportMap));
  hid->setBatteryLevel(100);

  server->start();

  NimBLEAdvertising *advertising = NimBLEDevice::getAdvertising();
  advertising->setName(DEVICE_NAME);
  advertising->setAppearance(HID_KEYBOARD);
  advertising->addServiceUUID(hid->getHidService()->getUUID());
  advertising->enableScanResponse(true);
  if(activeSlot != BT_SLOT_NONE) {
    startAdvertisingForActiveSlot();
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  KeyboardUart.begin(UART_BAUD, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
  loadSlotPeers();
  setupBle();
  
  Serial.println("UART to BLE HID keyboard ready");
}

void loop() {
  uint8_t eventType;
  uint8_t keycode;

  while (readFrame(eventType, keycode)) {
    if (eventType == EVENT_PRESS) {
      if(activeSlot != BT_SLOT_NONE) {
        pressKey(keycode);
      }
    } else if(eventType == EVENT_RELEASE) {
      if(activeSlot != BT_SLOT_NONE) {
        releaseKey(keycode);
      }
    } else if(eventType == CMD_SELECT_BT_SLOT) {
      selectBtSlot(keycode);
    } else if(eventType == CMD_CLEAR_BT_SLOT) {
      clearBtSlot(keycode);
    }
  }

  delay(1);
}