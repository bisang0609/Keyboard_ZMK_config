# codex.md

## 목적

- 이 저장소는 ZMK `v0.3` 기반 `tomak79` keyboard/dongle config만 관리한다.
- 목표는 빌드 가능 상태와 재현 가능한 환경을 우선 유지하는 것이다.

## 수정 금지

- `../zmk/`
- `../zephyr/`
- `../modules/`
- `../bootloader/`

## 반드시 유지할 사항

- `left/right/keypad`용 `MAX17048` I2C 핀
  - `SDA = P0.06`
  - `SCL = P0.08`
- Git 관리 대상은 `Keyboard_ZMK_config`만 유지
- 사용자 실기 검증용 성공 산출물은 임의 삭제하지 않는다.
  - `build/tomak79_left/`
  - `build/tomak79_right/`
  - `build/tomak79_dongle_ssd1306_block/`
  - `build/tomak79_dongle_ssh1106/`
  - `build/tomak79_dongle_ssh1106_encoder/`
  - `build/tomak79_dongle_yads_encoder/`

## 현재 유지 실드

- `tomak79_left`
- `tomak79_right`
- `tomak79_right2`
- `tomak79_right3`
- `tomak79_dongle`
- `tomak79_dongle_ssd1306_block`
- `tomak79_dongle_ssh1106`
- `tomak79_dongle_ssh1106_encoder`
- `tomak79_dongle_yads`
- `tomak79_dongle_yads_encoder`

## 현재 상태

- `tomak79_dongle_ssd1306_block`는 SSD1306 안정화 기준 실드로 유지한다.
- `tomak79_dongle_ssh1106`는 1.3인치 SH1106 OLED용 분리 실드로 유지한다.
- `tomak79_dongle_ssh1106_encoder`는 SH1106 OLED에 YADS와 동일한 엔코더 핀 구성을 붙인 분리 실드로 유지한다.
- `tomak79_dongle_ssh1106_encoder`는 엔코더 반틱에서 볼륨이 먼저 두 번 움직이던 체감을 줄이기 위해 `triggers-per-rotation = 20`으로 조정한 상태다.
- `tomak79_dongle_yads`는 `zmk-dongle-screen` 기반 ST7789 동글 실드로 유지한다.
- `tomak79_dongle_yads_encoder`는 ST7789 YADS에 단일 로터리 엔코더를 추가한 동글 실드로 유지한다.
- `left/right`는 `MAX17048`를 `P0.06/P0.08` I2C로 읽도록 정리했다.
- `left/right`는 배터리 갱신 루프를 추가해 split 배터리 값이 idle 이후에도 다시 전달되도록 보강했다.
- `left/right`는 `RGB underglow`와 `EXT_POWER`를 분리해 `P0.13` 전원 제어 영향을 줄이도록 조정했다.
- `left/right` underglow는 `BREATHE` 시작 효과와 `60%` 최대 밝기 제한으로 유지한다.
- `tomak79_right2`는 `right`를 복제한 실험용 split peripheral 실드로 추가했고, 이후 부가기능 실험은 `right2` 기준으로 진행한다.
- `tomak79_right3`는 현재 안정 상태의 `right`를 그대로 복제한 새 실험용 split peripheral 실드로 추가했고, 다음 기능 실험은 `right3` 기준으로 진행한다.
- `tomak79_right3`의 `P0.09` 추가 WS2812 실험은 underglow 복귀 이상 원인 분리를 위해 현재 잠시 제외했고, 다시 `right`와 동일한 동작 기준으로 되돌린 상태다.
- `tomak79_right3`는 현재 `right`와 동일하게 underglow 복귀 강제 재시동 로직을 함께 포함하도록 맞췄다.
- `tomak79_right3`에는 `P0.09` 별도 `WS2812 3개` 체인을 다시 추가했고, `Caps/Num` indicator 이벤트로 점등하도록 분리했다.
- `tomak79_right3`의 `P0.09` 3LED는 `spi2/ws2812-spi`와 수동 bitbang 경로가 실기에서 안정적이지 않아, 현재는 `nRF52 PWM + EasyDMA` 방식으로 underglow와 완전히 분리해 재구성했다.
- `tomak79_right3`의 `P0.09` 3LED는 현재 실기에서 `LED1=Caps`, `LED2=Caps/Num 요약`, `LED3=Num` 표시로 정상 동작한다.
- `tomak79_right` underglow 복귀 이상은 동글 표시 문제가 아니라 `right` 쪽 WS2812 재점등 경로 문제로 확인했다.
- `tomak79_right`에는 키 입력 후 underglow 상태가 `ON`으로 복귀해도 실제 LED 출력이 다시 살아나지 않는 경우를 대비해, `off -> 20ms 지연 -> on` 강제 재시동 로직을 추가했다.
- `tomak79_dongle_ssh1106_encoder` 화면에는 `USB/BLx` 위 빈 줄에 `UG ON/OFF` 상태를 표시하도록 보강했다.
- `tomak79_dongle_ssh1106_encoder`의 `UG OFF` 표시가 `O`만 보이던 원인은 커스텀 OLED 글리프 테이블에 `F`, `G`가 빠져 있었기 때문이며, 해당 글리프를 추가해 수정했다.
- 현재 확인된 동작 기준은 `right` 실제 underglow 복귀 정상, 동글 OLED `UG ON/OFF` 상태 표시 정상이다.
- `tomak79_right2`는 현재 `right` 복제 기준의 정상 입력 확인용 실드 상태를 우선 유지하고, OLED/추가 LED는 별도 안정화 후 다시 얹는 쪽으로 보류했다.
- `tomak79_right2` 실드 메타(`.zmk.yml`)도 `right`와 같은 형태로 정리해, 현재는 이름만 다른 `right` 기능 추가용 분기 상태로 맞췄다.
- `tomak79_right2`에는 `0.91"` `SSD1306` OLED를 keyboard-side `MAX17048`와 같은 `P0.06/P0.08` I2C 버스에 추가했다.
- `tomak79_right2`에는 기존 underglow(`P0.10`, `48` LED)는 그대로 유지하고, 별도 `P0.09`에 `WS2812 4개` 체인을 추가했다.
- `tomak79_right2`의 추가 `WS2812 4개`는 `Caps Lock` HID indicator와 연동되도록 별도 전용 소스로 붙였다.
- `tomak79_right2` OLED는 현재 커스텀 status screen으로 `LT`, `RT`, `CAP`, `NUM` 4개 항목을 128x32 텍스트로 직접 그리도록 구성했다.
- `tomak79_right2`는 split peripheral 구조라 `RT`는 자기 배터리로 표시하고, `LT`는 현재 공개 배터리 fetch 경로가 없어 우선 `NC`로 유지한다.
- `tomak79_right2`의 `CAP/NUM`은 ZMK 본체 수정 없이, 실드 내부 최소 `hid_indicators` 구현을 추가해 split indicator 이벤트를 OLED 표시용으로만 받도록 정리했다.
- `tomak79_right2`의 OLED는 현재 실드 내부 고정 overlay보다 `oled_adapter_pro_micro_128x32` 추가 실드 조합을 우선 실험 중이다.
- `tomak79_right2` 자체 overlay에서는 중복 `0x3c` OLED 노드를 제거해, 어댑터 실드 빌드 시 OLED 장치가 하나만 잡히도록 정리했다.
- `tomak79_right2` OLED 노이즈 증상 확인을 위해 `ready-time-ms = <100>` 테스트 빌드를 추가했다.
- `tomak79_dongle_ssh1106` 실드를 추가해 `ssd1306_block`와 분리된 1.3인치 SH1106 OLED 구성을 유지하도록 정리했다.
- `tomak79_dongle_ssh1106_encoder` 실드를 추가해 `P0.29/P0.31` 회전과 `P0.02` 버튼을 사용하는 엔코더 구성을 SH1106 쪽에도 분리 유지하도록 정리했다.
- fresh build에서 빠지던 `CONFIG_APPLICATION_DEFINED_SYSCALL`를 공통 설정으로 보강해 app-defined syscall 헤더 생성 경로를 복구하도록 정리했다.
- `tomak79_dongle_ssh1106_encoder`는 split peripheral battery fetch를 켜서 동글 화면에서 `LT/RT` 배터리 표시로 연결 상태를 바로 확인할 수 있게 정리했다.
- `tomak79_dongle_ssh1106_encoder`는 `studio-rpc-usb-uart` 사용 시 PC에서 CDC-ACM serial 장치가 뜨도록 `pro_micro_serial` 비활성화를 제거했다.
- `tomak79_dongle_ssh1106_encoder`는 중앙 동글 빌드에도 `RGB underglow`/`LED_STRIP`/`WS2812_STRIP`를 포함하고 `zmk,underglow = &led_strip`를 잡아 `Fn + ESC/F1..F6` underglow 제어를 split peripheral로 전파할 수 있게 정리했다.
- `tomak79_dongle_ssh1106_encoder`의 실제 `Fn` 물리 위치는 `Ctrl`과 `Alt` 사이 키로 다시 연결했고, ZMK Studio 레이어 이름에도 `Fn`이 보이도록 정리했다.
- `Fn` 레이어 underglow 제어는 `Esc=toggle`, `F1=hue+`, `F2=hue-`, `F3=bri+`, `F4=bri-`, `F5=effect`, `F6=effect reverse`로 맞췄다.
- `left/right/dongle ssh1106 encoder` underglow 최대 밝기는 모두 `60%`로 제한한 상태다.
- 로컬 기본 빌드 산출물 위치는 `~/ZMK_Keyboard/build/<artifact-name>/zephyr/` 기준으로 관리한다.

## 최근 완료 작업

- `tomak79_dongle_ssh1106`를 `sinowealth,sh1106` 기반 128x64 OLED 실드로 추가했다.
- `tomak79_dongle_ssh1106_encoder`를 추가하고 엔코더 회전은 볼륨 업/다운, 버튼은 짧게 `studio_unlock`, 길게 `BT_CLR`로 맞췄다.
- `tomak79_dongle_ssh1106_encoder`는 `triggers-per-rotation = 20`으로 조정해 반틱 반응을 줄이고 1틱 기준 볼륨 변화 체감으로 맞췄다.
- `tomak79_dongle_yads_encoder`의 엔코더 핀 기준을 SH1106 쪽에도 동일하게 적용했다.
- `MAX17048` 실기 로그를 바탕으로 `left/right` 배터리 읽기와 주기 갱신 경로를 정리했다.
- `tomak79_dongle_ssh1106_encoder`에 `CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING=y`를 추가해 `LT/RT` 패널에 split 배터리 표시가 나오도록 보강했다.
- `left/right` underglow 기본 효과를 `BREATHE`로 맞추고 최대 밝기를 `60%`로 제한했다.
- `tomak79_dongle_ssh1106_encoder` keymap에 `FN + ESC/F1..F6` underglow 제어를 추가했다.
- `tomak79_dongle_ssh1106_encoder` keymap에서 `Fn`이 보이지 않던 원인은 물리 `Fn` 슬롯에 `&enc_btn_btclr 0 0`가 들어가 있었기 때문이며, 이를 `&mo 4`로 되돌려 `Ctrl-Alt` 사이 키가 `Fn`으로 동작하도록 수정했다.
- `tomak79_dongle_ssh1106_encoder`의 USB serial 복구를 위해 `pro_micro_serial` 비활성화를 제거하고, 최종 빌드에는 `studio-rpc-usb-uart`를 명시적으로 포함해 `uart_rpc_transport`와 `cdc_acm`이 살아 있는 상태로 맞췄다.
- `settings_reset_nice_nano_v2`를 다시 빌드해 reset UF2 경로를 `~/ZMK_Keyboard/build/settings_reset_nice_nano_v2/zephyr/zmk.uf2`로 확인했다.
- `left` 없이 `right`만 연결된 상태에서는 동글의 첫 peripheral 슬롯 표시가 `LT`처럼 보일 수 있음을 확인했다.
- 회사/집 환경 공통 진행을 위해 `tomak79_right2` 실드와 `build.yaml` artifact를 추가했다.
- `tomak79_right2`에 얹었던 `SSD1306`/추가 WS2812 실험 구성은 입력 이상 가능성이 있어 일단 제거하고, `right`와 동일한 동작 기준으로 되돌렸다.
- `tomak79_right2.zmk.yml`에서 `right`와 달랐던 `url/features/siblings` 차이를 제거해 실드 정의도 `right`에 최대한 맞췄다.
- `right`는 실제 빌드 결과에 keymap node가 포함되지 않는 split peripheral 기준 상태였고, `right2`에만 `KEYMAP_FILE`를 강제로 넣으면 오히려 `right`와 달라져 키맵이 어긋날 수 있어 해당 강제 지정은 다시 제거했다.
- `right`의 실입력 정상 빌드(`~/ZMK_Keyboard/build/tomak79_right (2)/zephyr`)와 현재 `right`/`right2`를 비교해, 핵심 차이가 keymap이 아니라 `tomak79_right.dtsi`의 `row-gpios`/`col-gpios` 순서였음을 확인했고 known-good 순서로 복구했다.
- 그 복구 후 `tomak79_right`와 `tomak79_right2` UF2는 known-good `right (2)` UF2와 동일 SHA256로 맞춰 정상 입력 기준을 회복했다.
- 이후 `right2`에만 `SSD1306 128x32`와 `P0.09`의 별도 `WS2812 4개`/`Caps Lock` 연동을 다시 얹어 재빌드했다.
- `west.yml`에 `zmk-oled-adapter` 모듈을 추가하고, `build.yaml`에 `tomak79_right2 oled_adapter_pro_micro_128x32` 조합 artifact를 추가했다.
- `tomak79_right2_oled_adapter` 빌드에서는 merged DTS 기준 `zephyr,display = &oled_128x32`만 남고, `right2` 쪽 중복 OLED 노드는 제거된 상태를 확인했다.

## 마지막 빌드 결과

- `tomak79_left`: 빌드 성공
- `tomak79_right`: 빌드 성공
- `tomak79_right + underglow resume restart`: 빌드 성공, 산출물 `~/ZMK_Keyboard/build/tomak79_right/zephyr/zmk.uf2`
- `tomak79_right2`: 빌드 성공, 산출물 `build/tomak79_right2/`
- `tomak79_right2 + SSD1306 + P0.09 caps WS2812`: 빌드 성공, 산출물 `~/ZMK_Keyboard/build/tomak79_right2/zephyr/zmk.uf2`
- `tomak79_right2 + SSD1306 ready-time 100ms`: 빌드 성공, 산출물 `~/ZMK_Keyboard/build/tomak79_right2/zephyr/zmk.uf2`
- `tomak79_right2 + custom OLED status (LT/RT/CAP/NUM)`: 빌드 성공, 산출물 `~/ZMK_Keyboard/build/tomak79_right2/zephyr/zmk.uf2`
- `tomak79_right3`: 빌드 성공, 산출물 `~/ZMK_Keyboard/build/tomak79_right3/zephyr/zmk.uf2`
- `tomak79_right3 (right 기준 복구판)`: 빌드 성공, 산출물 `~/ZMK_Keyboard/build/tomak79_right3/zephyr/zmk.uf2`
- `tomak79_right3 (right 기준 + underglow resume 공유판)`: 빌드 성공, 산출물 `~/ZMK_Keyboard/build/tomak79_right3/zephyr/zmk.uf2`
- `tomak79_right3 + P0.09 WS2812 x3 Caps/Num`: 빌드 성공, 산출물 `~/ZMK_Keyboard/build/tomak79_right3/zephyr/zmk.uf2`
- `tomak79_right3 + P0.09 PWM EasyDMA Caps/Num`: 빌드 성공, 산출물 `~/ZMK_Keyboard/build/tomak79_right3/zephyr/zmk.uf2`
- `tomak79_right2 + oled_adapter_pro_micro_128x32`: 빌드 성공, 산출물 `~/ZMK_Keyboard/build/tomak79_right2_oled_adapter/zephyr/zmk.uf2`
- `tomak79_dongle_ssd1306_block`: 빌드 성공, 실기 표시 정상
- `tomak79_dongle_ssh1106`: 빌드 성공, 산출물 `build/tomak79_dongle_ssh1106/`
- `tomak79_dongle_ssh1106_encoder`: 빌드 성공, 산출물 `build/tomak79_dongle_ssh1106_encoder/`
- `tomak79_dongle_ssh1106_encoder + UG ON/OFF 표시`: 빌드 성공, 산출물 `~/ZMK_Keyboard/build/tomak79_dongle_ssh1106_encoder/zephyr/zmk.uf2`
- `tomak79_dongle_ssh1106_encoder + split battery fetch`: 빌드 성공, 산출물 `build/tomak79_dongle_ssh1106_encoder_battfetch_20260706/`, `LT/RT` 배터리 표시로 split 연결 상태 확인 가능
- `tomak79_dongle_yads + dongle_screen`: 빌드 성공
- `tomak79_dongle_yads_encoder + dongle_screen`: 빌드 성공, `studio_unlock`/길게 `BT_CLR(400ms)` 및 풀 Studio keymap 표시 구조 확인, `Mod Widget on / WPM off / left-right-keypad 3배터리 표시` 반영 확인
- `tomak79_dongle_yads_encoder`: 세로 화면 실험은 되돌리고, 현재는 `dongle_screen` 기반 안정 상태를 우선 유지한다.
- `settings_reset_nice_nano_v2`: 빌드 성공, 산출물 `build/settings_reset_nice_nano_v2/`

## 빌드 위치

- 사용자가 평소 쓰는 기본 빌드 루트는 `~/ZMK_Keyboard/build/` 이다.
- 현재 저장소 내부에서 `west build -d build/...`를 실행하면 테스트 산출물은 `~/ZMK_Keyboard/Keyboard_ZMK_config/build/<name>/zephyr/` 아래에 생성된다.
- 사용자가 바로 플래시할 최종 산출물은 가능하면 `~/ZMK_Keyboard/build/<artifact-name>/zephyr/` 기준으로 다시 빌드해 제공하는 쪽을 우선한다.
- 이번 최종 반영 기준 `tomak79_dongle_ssh1106_encoder` UF2 위치는 `~/ZMK_Keyboard/build/tomak79_dongle_ssh1106_encoder/zephyr/zmk.uf2` 이다.
- 이번 `tomak79_right2` UF2 위치는 `~/ZMK_Keyboard/build/tomak79_right2/zephyr/zmk.uf2` 이다.
- 이번 `tomak79_right3` UF2 위치는 `~/ZMK_Keyboard/build/tomak79_right3/zephyr/zmk.uf2` 이다.
- 이번 reset 확인용 `settings_reset_nice_nano_v2` UF2 위치는 `~/ZMK_Keyboard/build/settings_reset_nice_nano_v2/zephyr/zmk.uf2` 이다.

## 산출물 보존 규칙

- `build/`는 기본적으로 Git 포함 대상은 아니지만, 사용자 실기 확인용 성공 산출물은 작업 중 임의 삭제하지 않는다.
- 특히 `left`, `right`, `ssd1306_block`, `ssh1106`, `ssh1106_encoder`, `yads_encoder`의 성공 빌드 폴더는 사용자 확인 없이 비우지 않는다.
- `tomak79_dongle_ssh1106_encoder_battfetch_20260706`는 이번 연결 확인용 최종 산출물로 보존한다.
- 저장소 내부 테스트 빌드(`Keyboard_ZMK_config/build/...`)는 작업 흔적으로만 보고, 최종 반영 후에는 정리해 두는 쪽을 우선한다.
- 현재 상위 `~/ZMK_Keyboard/build/`에는 `tomak79_dongle_ssh1106_encoder`, `tomak79_dongle_yads_encoder`, `tomak79_left`, `tomak79_right`만 남기고 나머지 구버전/실험 산출물은 정리했다.
- 상위 `~/ZMK_Keyboard/snake-module`는 이번 정리에서 삭제했다.
- 산출물 정리가 필요하면 먼저 어떤 빌드 결과를 남길지 확인한 뒤 일부만 정리한다.

### 2026-08-05 18:09:32 KST 자동 작업 기록

#### 변경 파일

-  M boards/shields/tomak79/tomak79_dongle_ssh1106_encoder.overlay
-  M boards/shields/tomak79/tomak79_right.dtsi
-  M codex.md
-  M config/tomak79_dongle_ssh1106_encoder.keymap

#### Diff stat

 .../tomak79/tomak79_dongle_ssh1106_encoder.overlay |  2 +-
 boards/shields/tomak79/tomak79_right.dtsi          | 24 +++++++++++-----------
 codex.md                                           | 12 +++++++++++
 config/tomak79_dongle_ssh1106_encoder.keymap       |  8 +-------
 4 files changed, 26 insertions(+), 20 deletions(-)

#### 다음 작업 메모

- 필요 시 이 항목을 수동으로 보완한다.
