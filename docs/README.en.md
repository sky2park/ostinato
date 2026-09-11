# Ostinato

[🇰🇷 한국어](../README.md)

[![Firmware: GPL-3.0](https://img.shields.io/badge/Firmware-GPL--3.0-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.html)
[![Hardware: CC BY-NC 4.0](https://img.shields.io/badge/Hardware-CC%20BY--NC%204.0-blue.svg)](https://creativecommons.org/licenses/by-nc/4.0/)

![Ostinato Keyboard](9DMEK.jpg)

A custom 40% ortholinear mechanical keyboard designed from the ground up.

Ostinato is a compact 45-key ortholinear keyboard featuring a custom PCB, aluminum case, gasket-mounted plate, rotary encoder, RGB status indicators, and hybrid USB/Bluetooth connectivity.

The project includes the complete hardware and firmware design, from the PCB and case to the QMK/Vial firmware and ESP32-C3 Bluetooth controller.

---

## Features

- 40% ortholinear layout
- 45 keys
- MX-compatible switches
- QMK firmware
- Vial support
- USB and Bluetooth connectivity
- ESP32-C3-based Bluetooth controller
- RP2040-based main controller
- Three Bluetooth connection slots
- Rotary encoder
- SK6812MINI RGB LEDs
- Dedicated Bluetooth status indicators
- Layer status indication
- Gasket-mounted plate
- Custom aluminum top case
- MJF PA12S nylon bottom case
- Stainless steel or H59 Copper alloy Brass internal weight

---

## Specifications

| Specification | Details |
|---|---|
| Layout | 40% Ortholinear |
| Keys | 45 |
| Main MCU | RP2040 |
| Bluetooth MCU | ESP32-C3 |
| Firmware | QMK |
| Keymap Configuration | Vial |
| Connectivity | USB / Bluetooth |
| Bluetooth Slots | 3 |
| Switch Type | MX-compatible |
| RGB LEDs | SK6812MINI (4x) |
| Rotary Encoder | 1 |
| Mounting | Gasket mount (Poron) |
| Plate | PC |
| Top Case | 6061 Aluminum |
| Bottom Case | MJF PA12S Nylon |
| Internal Weight | SUS304 Stainless steel or H59 Copper alloy Brass |

---

## Layout

Ostinato uses a compact 40% ortholinear layout with 45 keys.

The layout is designed around multiple layers to provide the functionality of a full-size keyboard while maintaining a small footprint.

### Layers

The firmware includes the following primary layers:

- **Base**
- **Navigation**
- **Number / Symbol**
- **Function**
- **Gaming**
- **Gaming+**
- **Gaming Navigation**

Additional layers and key assignments can be configured through Vial.

---

## Connectivity

Ostinato supports both wired USB and Bluetooth operation.

The RP2040 handles the primary keyboard functionality, while an ESP32-C3 module provides Bluetooth connectivity.

### USB

In USB mode, the keyboard operates as a standard USB HID keyboard.

### Bluetooth

The keyboard supports three Bluetooth connection slots:

- **Bluetooth 1**
- **Bluetooth 2**
- **Bluetooth 3**

The active Bluetooth slot can be selected directly from the keyboard. The firmware communicates with the ESP32-C3 over UART to control Bluetooth connection state and slot selection.

---

## Bluetooth Status

The RGB LEDs are used to provide visual feedback for Bluetooth status. Each Bluetooth slot has a dedicated status indicator.

The indicators distinguish between states such as:

- Advertising
- Connected
- Pairing
- Error / unavailable

This allows the current Bluetooth connection state to be checked without using a host device.

---

## RGB Indicators

Ostinato uses four SK6812MINI RGB LEDs. The LEDs are used as functional status indicators rather than decorative backlighting.

### Bluetooth Indicators

Three LEDs indicate the status of the three Bluetooth slots.

### Layer Indicator

The remaining LED is used for layer/status indication. The firmware can also use the indicator to display the state of special keyboard functions.

---

## Rotary Encoder

Ostinato includes a rotary encoder for additional input.

The encoder can be configured through QMK and Vial and can be assigned different functions depending on the active layer.

---

## Architecture & Design Philosophy

The goal of Ostinato is to combine a compact ortholinear layout with functionality normally associated with larger keyboards.

Rather than relying on a single wireless controller, the design separates the main keyboard controller and Bluetooth controller:

```text
       ┌───────────────┐
       │    RP2040     │
       │ Main Keyboard │
       │  Controller   │
       └───────┬───────┘
               │
              UART
               │
       ┌───────▼───────┐
       │   ESP32-C3    │
       │   Bluetooth   │
       │  Controller   │
       └───────────────┘
```

This architecture allows the RP2040 to remain responsible for the core keyboard operations (matrix scanning, lighting, Vial configuration) while the ESP32-C3 handles the Bluetooth connectivity stack.

---

## Firmware

The Ostinato firmware is based on QMK and Vial-QMK and is licensed under the **GNU General Public License v2.0 (GPL-2.0)**.

```text
firmware/
├── esp32-c3/
├── qmk/
│   └── ostinato/
└── vial/
    └── ostinato/
```

- **QMK Firmware**: Handles matrix scanning, keymaps and layers, custom keycodes, rotary encoder, RGB status indicators, USB output, and UART communication with the ESP32-C3.
- **ESP32-C3 Firmware**: Handles Bluetooth profile management, connection slots, and wireless HID output.

### Custom Keycodes

Ostinato defines custom keycodes for output and connection control:

- `OUT_USB`
- `OUT_BT1`
- `OUT_BT2`
- `OUT_BT3`
- `BT_CLR`

These keycodes allow switching between USB and the three paired Bluetooth devices on the fly.

---

## Hardware

The Ostinato hardware design files, including PCB, case, plate, and related CAD files, are licensed under the **Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)** license.

Personal and non-commercial use is permitted.

Commercial use requires prior permission from the author.

The hardware is divided into modular custom boards:

```text
pcb/
├── main-module/
├── led_module/
└── knob-module/
```

- **Main Module**: Hosts the RP2040 controller, ESP32-C3, diode matrix, and power regulation.
- **LED Module**: Contains four SK6812MINI addressable RGB LEDs for slot and layer visualization.
- **Knob Module**: Houses the rotary encoder and its breakout interface.

### Case & Plate

- **Top Case**: CNC-machined 6061 aluminum
- **Bottom Case**: MJF 3D-printed PA12S nylon
- **Internal Weight**: Laser-cut SUS304 Stainless steel or H59 Copper alloy Brass
- **Mounting**: Custom Poron gasket strips isolating the PC plate for a soft, resonant bottom-out.

> **Note on Manufacturing & Tolerances**  
> All case components (top case, bottom case, and internal weight) were prototyped and verified using **JLCCNC** and **JLC3DP**. Because tolerances and finishing can vary across different manufacturers and fabrication methods, parts produced through other vendors have not been tested and may have fitment/assembly issues.

Plate cutting profiles are provided in DXF format under `plate/`:
- `design.dxf`
- `plate.dxf`

---

## Manufacturing & Ordering Guide

All components have been designed and verified with specific fabrication vendors. If you plan to build Ostinato, follow the guidelines below for ordering parts.

### Overview

| Part | Source Files | Recommended Service | Process / Material Specs |
|---|---|---|---|
| **Top Case** | `case/top.step`<br>`case/top_theads.pdf` (Thread/tapping drawing) | JLCCNC | CNC Machining / 6061 Aluminum (Sandblasted / Anodized) |
| **Bottom Case** | `case/bottom.step` | JLC3DP | 3D Printing / MJF Nylon (PA12S) |
| **Internal Weight** | `case/weight.step` | JLCCNC | CNC or Sheet Metal / SUS304 Stainless steel or H59 Brass |
| **Plate** | `plate/plate.dxf` | Laser cutting service | Laser Cut / 1.5mm Polycarbonate (PC) |
| **PCBs** | `pcb/*/gerber/` | JLCPCB | FR-4 / 1.6mm (or 1.2mm depending on build) |

---

### Component Details & Ordering Instructions

#### 1. Case & Weight (JLCCNC / JLC3DP)
- **Top Case**:
  - **File**: `case/top.step`
  - **Thread Drawing**: `case/top_theads.pdf` (Attach this 2D drawing when ordering for threaded hole tapping)
  - **Service**: [JLCCNC](https://jlccnc.com/) (CNC Machining)
  - **Material**: 6061 Aluminum
  - **Finish**: Bead blasted (Sandblasting #150 or #180) + Anodized (Color of your choice)
- **Bottom Case**:
  - **File**: `case/bottom.step`
  - **Service**: [JLC3DP](https://jlc3dp.com/) (3D Printing)
  - **Technology**: MJF (Multi Jet Fusion)
  - **Material**: PA12S Nylon (Black/Dyed)
- **Internal Weight**:
  - **File**: `case/weight.step`
  - **Service**: [JLCCNC](https://jlccnc.com/)
  - **Material**: SUS304 Stainless steel (Brushed / Sandblasted) or H59 Brass

> ⚠️ **Important Tolerance Warning**:  
> The CAD models, tolerances, and fitment clearances are tuned and verified specifically for **JLCCNC** and **JLC3DP**. Fabrication via other vendors or processes has not been tested and may result in improper fitment, shrinkage variance, or assembly issues.

#### 2. Switch Plate
- **File**: `plate/plate.dxf`
- **Material**: 1.5mm Polycarbonate (PC)
- **Service**: Any precision laser-cutting vendor (e.g., JLCCNC sheet cutting or local laser cutting services).

#### 3. PCBs
- **Paths**:
  - `pcb/main-module/`
  - `pcb/led_module/`
  - `pcb/knob-module/`
- **Service**: [JLCPCB](https://jlcpcb.com/)
- **Ordering Recommendations**:
  - Upload the Gerber zip files along with the **BOM and CPL files** from each folder to order the **PCB Assembly (SMT)** service.
  - **PCBA Settings**:
    - **Assembly Side**: Select **`Bottom Side`** for all three modules.
    - **PCBA Type**:
      - `main-module`: Must be set to **`Standard`** due to assembly constraints of the ESP32-C3 module (bottom pad soldering).
      - `led_module` & `knob-module`: Select **`Economy`**.
  - ⚠️ **Note**: The **EC11 rotary encoder** on the `knob-module` is not covered by the SMT service; source it separately and solder it by hand (through-hole soldering).
  
#### 4. Additional Off-the-Shelf Parts (Hardware & Sourcing)

These standard hardware parts and components are not custom-fabricated and can be sourced from general hardware marketplaces (e.g., AliExpress) or custom keyboard vendors.

> **Note**: The links below reflect the exact listings and sources I used for my prototype build. They are provided solely for reference—any equivalent parts matching the specifications will work.

| Item | Specification | Qty | Reference Link | Notes |
|---|---|---|---|---|
| **Poron Gasket Strips** | 20mm (L) × 3mm (W) × 2mm (T) | 8 | [AliExpress](https://ko.aliexpress.com/item/1005005713921230.html) | Mounts to plate tabs / case edges |
| **FPC Ribbon Cable** | 0.5mm pitch, 6-pin, **Reverse (Type B)**, 10–15cm | 2 | [AliExpress](https://aliexpress.com/item/1005007862185591.html) | Connects main module to LED / knob modules |
| **Rotary Encoder** | EC11 rotary encoder, **15mm shaft height** | 1 | [AliExpress](https://aliexpress.com/item/1005009120433387.html) | Hand-soldered onto `knob-module` |
| **Encoder Knob** | Compatible with EC11 shaft **20 x 13 x 6mm** | 1 | [AliExpress](https://aliexpress.com/item/1005007339301144.html) | Fits selected rotary encoder |
| **Unified Daughterboard S1 & Cable** | Unified Daughterboard (UDB) S1 with Molex cable | 1 | [Vendor Link](https://keebd.com/products/unified-daughterboards?variant=45606739411096) | Available via various keyboard vendor stores (e.g., CannonKeys, Swagkeys, etc.)
| **Light Guides** | Acrylic light pipe / guide **1.5-2.2 or 2.9-3.0** | 4 | [AliExpress](https://aliexpress.com/item/1005005231503928.html) | Diffuses status LEDs |
| **Heat-set Insert** | M2 × L3mm × OD 3.2mm | 4 | [AliExpress](https://aliexpress.com/item/1005006472641726.html) | Case / module mounting |
| **Heat-set Insert** | M3 × L4mm × OD 4.2mm | 6 | [AliExpress](https://aliexpress.com/item/1005006472641726.html) | Weight / case and Weight / UDB mounting |
| **Screw** | M2 × 4mm | 4 | [AliExpress](https://aliexpress.com/item/1005005270702287.html) | Internal module mounting |
| **Screw** | M2.5 × 8mm | 4 | [AliExpress](https://aliexpress.com/item/1005005270702287.html) | Case assembly |
| **Screw** | M3 × 5mm | 6 | [AliExpress](https://aliexpress.com/item/1005005270702287.html) | Internal weight / UDB fastening |

---

## Repository Structure

```text
ostinato/
├── case/             # CAD and step/stl files for case parts
├── firmware/
│   ├── esp32-c3/     # ESP-IDF / Arduino BT firmware
│   ├── qmk/          # QMK source and keyboard rules
│   │   └── ostinato/
│   └── vial/         # Vial configuration & keymap files
│       └── ostinato/
├── pcb/
│   ├── main-module/  # Schematic, KiCad layouts, and Gerber files
│   ├── led_module/
│   └── knob-module/
├── plate/
│   ├── design.dxf
│   └── plate.dxf
└── README.md
```

---

## Status

Ostinato is an active custom hardware project. Schematics, gerber files, and firmware implementations may receive breaking updates as physical revisions are tested.

---

## Credits

Designed and developed by **sky2park**.

- [QMK Firmware](https://qmk.fm/)
- [Vial](https://get.vial.today/)
- Raspberry Pi RP2040 & Espressif ESP32-C3

![Ostinato Keyboard](RHO8S.jpg)
