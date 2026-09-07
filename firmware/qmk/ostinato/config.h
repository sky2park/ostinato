#pragma once

// UART 설정 (ESP32C3와의 통신을 위한 핀과 보레이트 정의)
#define UART_DRIVER SIOD1
#define UART_TX_PIN GP20
#define UART_RX_PIN GP21
#define UART_BAUD_RATE 115200

#define ENCODER_A_PINS { GP22 }
#define ENCODER_B_PINS { GP23 }
#define ENCODER_RESOLUTION 2

#define WS2812_DI_PIN GP25
#define WS2812_LED_COUNT 4

#define BOOTMAGIC_ROW 0
#define BOOTMAGIC_COLUMN 0