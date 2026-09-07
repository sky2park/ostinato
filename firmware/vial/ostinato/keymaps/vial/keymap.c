// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include <uart.h>
#include "ws2812.h"

#define UART_FRAME_HEADER 0xAA
#define UART_EVENT_PRESS 0x01
#define UART_EVENT_RELEASE 0x00

#define UART_CMD_SELECT_BT_SLOT 0x10
#define UART_CMD_CLEAR_BT_SLOT 0x11

#define BT_SLOT_NONE 0x00
#define BT_SLOT_1    0x01
#define BT_SLOT_2    0x02
#define BT_SLOT_3    0x03

#define STATUS_BT 0x80

#define BT_STATE_OFF          0x00
#define BT_STATE_ADVERTISING  0x01
#define BT_STATE_CONNECTED    0x02
#define BT_STATE_PAIRING      0x03
#define BT_STATE_ERROR        0x04

#define LED_BRIGHTNESS 2
#define LED_LAYER  3
#define LED_IDLE_TIMEOUT 300000

enum layers {
    _BASE,
    _LOWER,
    _RAISE,
    _FN,
    _GAMING,
    _GAMING_PLUS,
    _GAMING_NAVI,
};

enum output_mode {
    OUTPUT_USB,
    OUTPUT_BT1,
    OUTPUT_BT2,
    OUTPUT_BT3,
};

static uint8_t bt_state[3] = {
    BT_STATE_OFF,
    BT_STATE_OFF,
    BT_STATE_OFF
};

enum custom_keycodes {
    OUT_USB = QK_KB_0,
    OUT_BT1,
    OUT_BT2,
    OUT_BT3,
    BT_CLR,
};

#define KC_LCUB S(KC_LBRC)
#define KC_RCUB S(KC_RBRC)
#define KC_PLS S(KC_EQL)
#define KC_UNDR S(KC_MINS)

static bool uart_ready = false;
static bool ws2812_ready = false;

static bool gaming_enabled = false;
static enum output_mode current_output = OUTPUT_USB;
static uint32_t last_status_poll = 0;

static uint32_t last_activity = 0;
static bool leds_idle = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // 기본 레이어 (Layer 0)
    [_BASE] = LAYOUT(
        KC_GRV,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MUTE,
        KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
        
        MO(3),   KC_LGUI, KC_LALT, KC_TAB,  MO(1),       KC_SPC,       MO(2),   KC_BSPC, KC_DEL,           KC_ENT
    ),

    // [1] Lower 레이어 (Layer 1)
    [_LOWER] = LAYOUT(
        KC_ESC,  C(KC_F4),KC_END,  KC_UP,   KC_HOME, KC_PGUP, KC_PGUP, KC_HOME, KC_UP,   KC_END,  KC_TRNS, KC_TRNS,
        KC_TRNS, A(KC_F4),KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN, KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_F12,  KC_F11,  KC_F5,   KC_TRNS, KC_INS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS,      KC_TRNS, KC_RALT, KC_RCTL,          KC_TRNS
    ),

    // [2] Raise 레이어 (Layer 2)
    [_RAISE] = LAYOUT(
        KC_ESC,  S(KC_1), S(KC_2), S(KC_3), S(KC_4), S(KC_5), S(KC_6), S(KC_7), S(KC_8), S(KC_9), S(KC_0), KC_TRNS,
        KC_TRNS, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    CW_TOGG,
        KC_TRNS, KC_LBRC, KC_RBRC, KC_LCUB, KC_RCUB, KC_TRNS, KC_BSLS, KC_MINS, KC_PLS,  KC_EQL,  KC_UNDR, KC_TRNS,

        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS,      KC_TRNS, KC_RALT, KC_RCTL,          KC_TRNS
    ),

    // [3] Fn 레이어 (Layer 3)
    [_FN] = LAYOUT(
        KC_TRNS, KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, BT_CLR,  KC_TRNS,
        KC_TRNS, KC_F5,   KC_F6,   KC_F7,   KC_F8,   DF(4),   KC_TRNS, OUT_USB, OUT_BT1, OUT_BT2, OUT_BT3, KC_TRNS,
        KC_TRNS, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,

        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS
    ),

    // [4] Gaming 레이어 (Layer 4)
    [_GAMING] = LAYOUT(
        KC_GRV,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MUTE,
        KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
        
        KC_TRNS, KC_LGUI, KC_LALT, KC_TAB,  MO(5),       KC_SPC,       MO(6),   KC_BSPC, KC_DEL,           KC_ENT
    ),

    // [5] Gaming Plus 레이어 (Layer 5)
    [_GAMING_PLUS] = LAYOUT(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_PGUP, KC_HOME, KC_UP,   KC_END,  KC_TRNS, KC_TRNS,
        KC_TRNS, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_MINS, KC_EQL,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,

        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS
    ),

    // [6] Gaming Navi 레이어 (Layer 6)
    [_GAMING_NAVI] = LAYOUT(
        KC_TRNS, S(KC_1), S(KC_2), S(KC_3), S(KC_4), S(KC_5), S(KC_6), S(KC_7), S(KC_8), S(KC_9), S(KC_0), KC_TRNS,
        KC_TRNS, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    CW_TOGG,
        KC_TRNS, KC_LBRC, KC_RBRC, KC_LCUB, KC_RCUB, KC_TRNS, KC_BSLS, KC_MINS, KC_PLS,  KC_EQL,  KC_UNDR, KC_TRNS,

        DF(0),   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS
    ),
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = {
        ENCODER_CCW_CW(C(KC_Z), C(KC_Y))
    },
    [1] = {
        ENCODER_CCW_CW(C(KC_LEFT), C(KC_RGHT))
    },
    [2] = {
        ENCODER_CCW_CW(KC_TRNS, KC_TRNS)
    },
    [3] = {
        ENCODER_CCW_CW(C(S(KC_TAB)), C(KC_TAB))
    },
    [4] = {
        ENCODER_CCW_CW(KC_TRNS, KC_TRNS)
    },
    [5] = {
        ENCODER_CCW_CW(KC_TRNS, KC_TRNS)
    },
    [6] = {
        ENCODER_CCW_CW(KC_TRNS, KC_TRNS)
    }
};
#endif

static void set_led(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    ws2812_set_color(index, r, g, b);
}

static void all_leds_off(void)
{
    if (!ws2812_ready) {
        return;
    }

    for (uint8_t i = 0; i < 4; i++) {
        set_led(i, 0, 0, 0);
    }

    ws2812_flush();
}

static void update_bt_led(uint8_t led, uint8_t state) {
    switch (state) {

        case BT_STATE_ADVERTISING:
        case BT_STATE_PAIRING:
            // Blue
            set_led(led, 0, 0, LED_BRIGHTNESS);
            break;

        case BT_STATE_CONNECTED:
            // Green
            set_led(led, 0, LED_BRIGHTNESS, 0);
            break;

        case BT_STATE_OFF:
        case BT_STATE_ERROR:
            // Red
            set_led(led, LED_BRIGHTNESS, 0, 0);
            break;

        default:
            set_led(led, 0, 0, 0);
            break;
    }
}

static void update_leds(void) {
    if (!ws2812_ready || leds_idle) {
        return;
    }

    // BT1 ~ BT3
    if (current_output == OUTPUT_USB) {

        // USB 연결 상태에서는 세 개 모두 흰색
        for (uint8_t i = 0; i < 3; i++) {
            set_led(i, LED_BRIGHTNESS, LED_BRIGHTNESS, LED_BRIGHTNESS);
        }

    } else {

        // BT 모드에서는 일단 전부 끄기
        for (uint8_t i = 0; i < 3; i++) {
            set_led(i, 0, 0, 0);
        }

        // 현재 선택된 BT 채널
        uint8_t bt = current_output - OUTPUT_BT1;

        if (bt < 3) {
            update_bt_led(bt, bt_state[bt]);
        }
    }


    // LED4: Gaming / Gaming+
    if (gaming_enabled) {
        set_led(LED_LAYER, 0, LED_BRIGHTNESS, 0);
    } else {
        set_led(LED_LAYER, 0, 0, 0);
    }

    ws2812_flush();
}

layer_state_t layer_state_set_user(layer_state_t state) {
    gaming_enabled = (get_highest_layer(default_layer_state) >= _GAMING) || (get_highest_layer(state) >= _GAMING);

    if(ws2812_ready) {
        update_leds();
    }

    return state;
}

static bool is_bt_output(enum output_mode output) {
    return output == OUTPUT_BT1 || output == OUTPUT_BT2 || output == OUTPUT_BT3;
}

static void uart_send_frame(uint8_t type, uint8_t value) {
    if(!uart_ready) {
        return;
    }

    uart_write(UART_FRAME_HEADER);
    uart_write(type);
    uart_write(value);
}

static uint8_t output_to_bt_slot(enum output_mode output) {
    switch(output) {
        case OUTPUT_BT1: 
            return BT_SLOT_1;
        case OUTPUT_BT2: 
            return BT_SLOT_2;
        case OUTPUT_BT3: 
            return BT_SLOT_3;
        case OUTPUT_USB:
        default: 
            return BT_SLOT_NONE;
    }
}

static void read_esp_status(void) {
    static uint8_t state = 0;

    while (uart_available()) {
        uint8_t b = uart_read();
        switch (state) {
            case 0:
                if (b == UART_FRAME_HEADER) {
                    state = 1;
                }
                break;

            case 1:
                if (b == STATUS_BT) {
                    state = 2;
                } else if (b == UART_FRAME_HEADER) {
                    state = 1;
                } else {
                    state = 0;
                }
                break;

            case 2: {
                uint8_t slot = b >> 4;
                uint8_t new_state = b & 0x0F;
            
                if (slot >= 1 && slot <= 3) {
                    uint8_t bt = slot - 1;
                    if (bt_state[bt] != new_state) {
                        bt_state[bt] = new_state;
                        if (current_output == OUTPUT_BT1 + bt) {
                            update_leds();
                        }
                    }
                }

                state = 0;
                break;
            }
        }
    }
}

static void uart_send_mods(uint8_t mods, bool pressed) {
    static const uint8_t mod_keycodes[] = {
        KC_LCTL, KC_LSFT, KC_LALT, KC_LGUI,
        KC_RCTL, KC_RSFT, KC_RALT, KC_RGUI
    };

    for (uint8_t i = 0; i < 8; i++) {
        if (mods & (1 << i)) {
            uart_send_frame(
                pressed ? UART_EVENT_PRESS : UART_EVENT_RELEASE,
                mod_keycodes[i]
            );
        }
    }
}

void housekeeping_task_user(void) {
    static uint32_t boot_time = 0;

    if (!uart_ready) {
        if (boot_time == 0) {
            boot_time = timer_read32();
        }

        if (timer_elapsed32(boot_time) > 1500) {
            uart_init(UART_BAUD_RATE);
            uart_ready = true;
        }

        return;
    }

    if (timer_elapsed32(last_status_poll) >= 100) {
        last_status_poll = timer_read32();
        read_esp_status();
    }

    if (!leds_idle && timer_elapsed32(last_activity) >= LED_IDLE_TIMEOUT) {
        leds_idle = true;
        all_leds_off();
    }
    
}

void keyboard_post_init_user(void) {     
    ws2812_init();
    ws2812_ready = true;

    last_activity = timer_read32();
    leds_idle = false;

    update_leds();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        last_activity = timer_read32();

        if (leds_idle) {
            leds_idle = false;
            update_leds();
        }
    }

    switch (keycode) {
        case OUT_USB:
            if (record->event.pressed) {
                current_output = OUTPUT_USB;
                update_leds();
                uart_send_frame(UART_CMD_SELECT_BT_SLOT, BT_SLOT_NONE);
            }
            return false;

        case OUT_BT1:
            if (record->event.pressed) {
                current_output = OUTPUT_BT1;
                update_leds();
                uart_send_frame(UART_CMD_SELECT_BT_SLOT, BT_SLOT_1);
            }
            return false;

        case OUT_BT2:
            if (record->event.pressed) {
                current_output = OUTPUT_BT2;
                update_leds();
                uart_send_frame(UART_CMD_SELECT_BT_SLOT, BT_SLOT_2);
            }
            return false;

        case OUT_BT3:
            if (record->event.pressed) {
                current_output = OUTPUT_BT3;
                update_leds();
                uart_send_frame(UART_CMD_SELECT_BT_SLOT, BT_SLOT_3);
            }
            return false;

        case BT_CLR:
            if (record->event.pressed) {
                uint8_t slot = output_to_bt_slot(current_output);

                if(slot != BT_SLOT_NONE) {
                    uart_send_frame(UART_CMD_CLEAR_BT_SLOT, slot);
                }
            }
            return false;
    }


    if (is_bt_output(current_output)) {

        // QMK 레이어/기능키는 QMK가 처리
        if (keycode == MO(1) ||
            keycode == MO(2) ||
            keycode == MO(3) ||
            keycode == MO(5) ||
            keycode == MO(6) ||
            keycode == DF(0) ||
            keycode == DF(4)) {
            return true;
        }

        // Ctrl+C, Shift+1 등
        if (IS_QK_MODS(keycode)) {
            uint8_t mods = QK_MODS_GET_MODS(keycode);
            uint8_t basic = QK_MODS_GET_BASIC_KEYCODE(keycode);

            if (record->event.pressed) {
                uart_send_mods(mods, true);
                uart_send_frame(UART_EVENT_PRESS, basic);
            } else {
                uart_send_frame(UART_EVENT_RELEASE, basic);
                uart_send_mods(mods, false);
            }

            return false;
        }

        // 일반 키
        if (IS_QK_BASIC(keycode)) {
            uart_send_frame(
                record->event.pressed ? UART_EVENT_PRESS : UART_EVENT_RELEASE,
                (uint8_t)keycode
            );
            return false;
        }

        return false;
    }

    return true;
}