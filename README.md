# Ostinato

[🇺🇸 English](docs/README.en.md)

[![Firmware: GPL-3.0](https://img.shields.io/badge/Firmware-GPL--3.0-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.html)
[![Hardware: CC BY-NC 4.0](https://img.shields.io/badge/Hardware-CC%20BY--NC%204.0-blue.svg)](https://creativecommons.org/licenses/by-nc/4.0/)

![Ostinato Keyboard](docs/9DMEK.jpg)

처음부터 직접 설계한 40% 오소리니어 기계식 키보드입니다.

Ostinato는 커스텀 PCB, 알루미늄 케이스, 가스켓 마운트 플레이트, 로터리 엔코더, RGB 상태 표시등, USB/Bluetooth 하이브리드 연결을 갖춘 45키 오소리니어 키보드입니다.

이 프로젝트에는 PCB와 케이스부터 QMK/Vial 펌웨어와 ESP32-C3 Bluetooth 컨트롤러까지 전체 하드웨어 및 펌웨어 설계가 포함되어 있습니다.

---

## 주요 기능

- 40% 오소리니어 레이아웃
- 45키
- MX 호환 스위치
- QMK 펌웨어
- Vial 지원
- USB 및 Bluetooth 연결
- ESP32-C3 기반 Bluetooth 컨트롤러
- RP2040 기반 메인 컨트롤러
- 3개의 Bluetooth 연결 슬롯
- 로터리 엔코더
- SK6812MINI RGB LED
- 전용 Bluetooth 상태 표시등
- 레이어 상태 표시
- 가스켓 마운트 플레이트
- 커스텀 알루미늄 상판 케이스
- MJF PA12S 나일론 하판 케이스
- SUS304 스테인리스 스틸 또는 H59 구리 합금 황동 내부 웨이트

---

## 사양

| 항목 | 내용 |
|---|---|
| 레이아웃 | 40% 오소리니어 |
| 키 수 | 45 |
| 메인 MCU | RP2040 |
| Bluetooth MCU | ESP32-C3 |
| 펌웨어 | QMK |
| 키맵 설정 | Vial |
| 연결 방식 | USB / Bluetooth |
| Bluetooth 슬롯 | 3 |
| 스위치 종류 | MX 호환 |
| RGB LED | SK6812MINI (4개) |
| 로터리 엔코더 | 1개 |
| 마운팅 | 가스켓 마운트 (Poron) |
| 플레이트 | PC |
| 상판 케이스 | 6061 알루미늄 |
| 하판 케이스 | MJF PA12S 나일론 |
| 내부 웨이트 | SUS304 스테인리스 스틸 또는 H59 구리 합금 황동 |

---

## 레이아웃

Ostinato는 45키의 컴팩트한 40% 오소리니어 레이아웃을 사용합니다.

작은 크기를 유지하면서 풀사이즈 키보드에 준하는 기능을 제공할 수 있도록 여러 레이어를 중심으로 설계되었습니다.

### 레이어

펌웨어에는 다음과 같은 주요 레이어가 포함되어 있습니다:

- **기본**
- **내비게이션**
- **숫자 / 기호**
- **기능**
- **게이밍**
- **게이밍+**
- **게이밍 내비게이션**

추가 레이어와 키 할당은 Vial을 통해 설정할 수 있습니다.

---

## 연결

Ostinato는 유선 USB와 Bluetooth를 모두 지원합니다.

RP2040은 키보드의 주요 기능을 담당하며, ESP32-C3 모듈이 Bluetooth 연결을 제공합니다.

### USB

USB 모드에서는 일반적인 USB HID 키보드로 동작합니다.

### Bluetooth

키보드는 3개의 Bluetooth 연결 슬롯을 지원합니다:

- **Bluetooth 1**
- **Bluetooth 2**
- **Bluetooth 3**

활성 Bluetooth 슬롯은 키보드에서 직접 선택할 수 있습니다. 펌웨어는 UART를 통해 ESP32-C3와 통신하며 Bluetooth 연결 상태와 슬롯 선택을 제어합니다.

---

## Bluetooth 상태

RGB LED는 Bluetooth 상태를 시각적으로 표시하는 데 사용됩니다. 각 Bluetooth 슬롯에는 전용 상태 표시등이 있습니다.

표시등은 다음과 같은 상태를 구분합니다:

- 광고(Advertising)
- 연결됨
- 페어링
- 오류 / 사용 불가

이를 통해 호스트 기기를 사용하지 않고도 현재 Bluetooth 연결 상태를 확인할 수 있습니다.

---

## RGB 표시등

Ostinato는 4개의 SK6812MINI RGB LED를 사용합니다. 이 LED는 장식용 백라이트가 아니라 기능적인 상태 표시등으로 사용됩니다.

### Bluetooth 표시등

3개의 LED가 각각 3개의 Bluetooth 슬롯 상태를 표시합니다.

### 레이어 표시등

나머지 1개의 LED는 레이어/상태 표시용으로 사용됩니다. 펌웨어는 이 표시등을 특수 키보드 기능의 상태를 표시하는 데에도 사용할 수 있습니다.

---

## 로터리 엔코더

Ostinato에는 추가 입력을 위한 로터리 엔코더가 포함되어 있습니다.

엔코더는 QMK와 Vial을 통해 설정할 수 있으며, 활성 레이어에 따라 서로 다른 기능을 할당할 수 있습니다.

---

## 아키텍처 및 설계 철학

Ostinato의 목표는 컴팩트한 오소리니어 레이아웃에 일반적으로 대형 키보드에서 제공되는 기능을 결합하는 것입니다.

하나의 무선 컨트롤러에 모든 기능을 의존하는 대신, 메인 키보드 컨트롤러와 Bluetooth 컨트롤러를 분리하는 구조로 설계했습니다:

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

이 아키텍처를 통해 RP2040은 핵심 키보드 동작(매트릭스 스캔, 조명, Vial 설정)을 담당하고, ESP32-C3는 Bluetooth 연결 스택을 담당합니다.

---

## 펌웨어

Ostinato 펌웨어는 QMK 및 Vial-QMK를 기반으로 하며 **GNU General Public License v2.0 (GPL-2.0)**에 따라 라이선스가 부여됩니다.

```text
firmware/
├── esp32-c3/
├── qmk/
│   └── ostinato/
└── vial/
    └── ostinato/
```

- **QMK 펌웨어**: 매트릭스 스캔, 키맵 및 레이어, 커스텀 키코드, 로터리 엔코더, RGB 상태 표시등, USB 출력, ESP32-C3와의 UART 통신을 담당합니다.
- **ESP32-C3 펌웨어**: Bluetooth 프로파일 관리, 연결 슬롯, 무선 HID 출력을 담당합니다.

### 커스텀 키코드

Ostinato는 출력 및 연결 제어를 위해 다음과 같은 커스텀 키코드를 정의합니다:

- `OUT_USB`
- `OUT_BT1`
- `OUT_BT2`
- `OUT_BT3`
- `BT_CLR`

이 키코드를 사용하면 USB와 페어링된 3개의 Bluetooth 기기 사이를 즉시 전환할 수 있습니다.

---

## 하드웨어

PCB, 케이스, 플레이트 및 관련 CAD 파일을 포함한 Ostinato 하드웨어 설계 파일은 **Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)** 라이선스에 따라 제공됩니다.

개인적 및 비상업적 사용은 허용됩니다.

상업적 사용에는 저작자의 사전 허가가 필요합니다.

하드웨어는 다음과 같은 모듈형 커스텀 보드로 구성됩니다:

```text
pcb/
├── main-module/
├── led_module/
└── knob-module/
```

- **메인 모듈**: RP2040 컨트롤러, ESP32-C3, 다이오드 매트릭스 및 전원 회로를 탑재합니다.
- **LED 모듈**: 슬롯 및 레이어 상태 표시를 위한 4개의 SK6812MINI 어드레서블 RGB LED를 탑재합니다.
- **노브 모듈**: 로터리 엔코더와 브레이크아웃 인터페이스를 탑재합니다.

### 케이스 및 플레이트

- **상판 케이스**: CNC 가공 6061 알루미늄
- **하판 케이스**: MJF 3D 프린팅 PA12S 나일론
- **내부 웨이트**: 레이저 절단 SUS304 스테인리스 스틸 또는 H59 구리 합금 황동
- **마운팅**: PC 플레이트를 분리하는 커스텀 Poron 가스켓 스트립을 사용하여 부드럽고 공명감 있는 바텀아웃을 구현합니다.

> **제조 및 공차 관련 참고사항**  
> 모든 케이스 부품(상판 케이스, 하판 케이스, 내부 웨이트)은 **JLCCNC** 및 **JLC3DP**를 통해 프로토타입을 제작하고 검증했습니다. 제조업체와 제작 방식에 따라 공차와 표면 마감이 달라질 수 있으므로, 다른 업체에서 제작한 부품은 테스트되지 않았으며 조립 또는 치수 호환성 문제가 발생할 수 있습니다.

플레이트 절단 프로파일은 `plate/` 아래에 DXF 형식으로 제공됩니다:
- `design.dxf`
- `plate.dxf`

---

## 제조 및 주문 가이드

모든 부품은 특정 제작 업체를 기준으로 설계하고 검증했습니다. Ostinato를 제작할 계획이라면 아래 주문 가이드를 참고하세요.

### 개요

| 부품 | 소스 파일 | 권장 서비스 | 공정 / 재료 사양 |
|---|---|---|---|
| **Top Case** | `case/top.step`<br>`case/top_theads.pdf` (Thread/tapping drawing) | JLCCNC | CNC Machining / 6061 Aluminum (Sandblasted / Anodized) |
| **Bottom Case** | `case/bottom.step` | JLC3DP | 3D Printing / MJF Nylon (PA12S) |
| **Internal Weight** | `case/weight.step` | JLCCNC | CNC or Sheet Metal / SUS304 Stainless steel or H59 Brass |
| **Plate** | `plate/plate.dxf` | Laser cutting service | Laser Cut / 1.5mm Polycarbonate (PC) |
| **PCBs** | `pcb/*/gerber/` | JLCPCB | FR-4 / 1.6mm (or 1.2mm depending on build) |

---

### 부품 상세 및 주문 방법

#### 1. 케이스 및 웨이트 (JLCCNC / JLC3DP)
- **Top Case**:
  - **File**: `case/top.step`
  - **나사산 도면**: `case/top_theads.pdf` (탭 가공을 주문할 때 이 2D 도면을 함께 첨부하세요)
  - **서비스**: [JLCCNC](https://jlccnc.com/) (CNC 가공)
  - **재료**: 6061 알루미늄
  - **표면 처리**: 비드 블라스트(샌드블라스트 #150 또는 #180) + 아노다이징(원하는 색상)
- **Bottom Case**:
  - **File**: `case/bottom.step`
  - **Service**: [JLC3DP](https://jlc3dp.com/) (3D Printing)
  - **기술**: MJF (Multi Jet Fusion)
  - **재료**: PA12S 나일론 (검정/염색)
- **Internal Weight**:
  - **File**: `case/weight.step`
  - **Service**: [JLCCNC](https://jlccnc.com/)
  - **재료**: SUS304 스테인리스 스틸 (헤어라인 / 샌드블라스트) 또는 H59 황동

> ⚠️ **중요 공차 경고**:  
> CAD 모델, 공차 및 조립 여유 치수는 **JLCCNC** 및 **JLC3DP**를 기준으로 조정하고 검증했습니다. 다른 업체나 다른 제작 방식은 테스트되지 않았으며, 부적절한 조립, 수축 편차 또는 조립 문제가 발생할 수 있습니다.

#### 2. 스위치 플레이트
- **File**: `plate/plate.dxf`
- **재료**: 1.5mm 폴리카보네이트 (PC)
- **서비스**: 정밀 레이저 절단이 가능한 업체 (예: JLCCNC 판재 절단 또는 현지 레이저 절단 업체).

#### 3. PCB
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
  
#### 4. 추가 기성 부품 (하드웨어 및 구매)

이러한 표준 하드웨어 부품과 구성품은 별도 제작품이 아니며, 일반 하드웨어 마켓플레이스(예: AliExpress)나 커스텀 키보드 판매처에서 구매할 수 있습니다.

> **참고**: 아래 링크는 제가 프로토타입 제작에 실제로 사용한 동일한 제품 목록과 구매처입니다. 참고용으로만 제공하며, 사양에 맞는 동등한 부품이라면 어떤 제품을 사용해도 됩니다.

| 품목 | 사양 | 수량 | 참고 링크 | 비고 |
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

## 빌드 가이드

🚧 상세 빌드 가이드 영상은 현재 준비 중입니다.

---

## 저장소 구조

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

## 상태

Ostinato는 현재 개발이 진행 중인 커스텀 하드웨어 프로젝트입니다. 실제 리비전을 테스트하는 과정에서 회로도, Gerber 파일 및 펌웨어 구현이 호환성을 깨뜨리는 형태로 변경될 수 있습니다.

---

## 크레딧

**sky2park**가 설계 및 개발했습니다.

- [QMK Firmware](https://qmk.fm/)
- [Vial](https://get.vial.today/)
- Raspberry Pi RP2040 & Espressif ESP32-C3

![Ostinato Keyboard](docs/RHO8S.jpg)
