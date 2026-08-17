# codex.md

## 목적

- 이 저장소는 ZMK `v0.3` 기반 `tomak79` keyboard/dongle config만 관리한다.
- 목표는 기능 실험보다 빌드 가능 상태와 재현 가능한 환경을 우선 유지하는 것이다.
- 집/회사 PC 동기화를 위해 주요 성공 빌드 산출물도 저장소 내부 `build/`에 함께 보관한다.

## 수정 금지

- `../zmk/`
- `../zephyr/`
- `../modules/`
- `../bootloader/`

## 반드시 유지할 사항

- `left/right/keypad`용 `MAX17048` I2C 핀
  - `SDA = P0.06`
  - `SCL = P0.08`
- Git 관리 대상은 `Keyboard_ZMK_config`만 유지한다.
- ZMK 본체나 Zephyr 쪽 수정으로 문제를 해결하지 않는다.

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

## 현재 상태 요약

- `left/right`는 `MAX17048`를 `P0.06/P0.08` I2C로 읽도록 유지한다.
- `left/right`는 split 배터리 갱신 루프를 포함한다.
- `left/right` underglow는 `BREATHE` 시작 효과와 최대 밝기 `60%` 기준으로 유지한다.
- `tomak79_right`는 underglow가 꺼진 뒤 다시 켜지지 않던 문제를 피하기 위해 강제 재시동 로직을 포함한 안정판이다.
- `tomak79_right2`는 `right` 복제 기반의 기능 실험용 분기다.
- `tomak79_right3`는 `right` 안정판 기반의 추가 실험용 분기다.
- `tomak79_right`와 `tomak79_right3`의 `P0.09` 보조 LED 3개는 `nRF52 PWM + EasyDMA` 방식으로 분리 구동한다.
- `tomak79_right`와 `tomak79_right3`의 `P0.09` LED 3개는 밝기 `30%` 고정의 느린 레인보우 흐름 효과를 사용한다.
- `tomak79_dongle_ssd1306_block`는 SSD1306 안정화 기준 실드다.
- `tomak79_dongle_ssh1106_encoder`는 SH1106 OLED와 엔코더를 포함한 현재 동글 주력 빌드다.
- `tomak79_dongle_ssh1106_encoder`는 OLED에 `UG ON/OFF`를 표시하고, `Fn + ESC/F1..F6` underglow 제어를 포함한다.
- `tomak79_dongle_yads_encoder`는 ST7789 기반 대체 동글 실드로 유지한다.

## 빌드 운용 기준

- 사용자가 평소 쓰는 기본 빌드 루트는 `~/ZMK_Keyboard/build/` 이다.
- 저장소 내부 `build/`는 집/회사 동기화용 복사본이다.
- `build/origin/`은 보존본이며 직접 수정하지 않는다.
- `build/test/`는 작업용 빌드 폴더이며, 저장소 내부 테스트 빌드는 여기만 사용한다.
- 저장소 내부 테스트 빌드 예시는 `west build -d build/test/<name> ...` 형식으로 진행한다.
- 사용자가 바로 플래시할 최종 산출물은 가능하면 `~/ZMK_Keyboard/build/<artifact-name>/zephyr/` 기준으로 다시 빌드해 제공한다.

## 저장소 내부 build 구성

- `build/origin/`
  - 기존 성공 빌드 보존본
  - 읽기 전용 기준 폴더
- `build/test/`
  - 현재 작업용 복사본
  - 유지 대상:
    - `tomak79_dongle_ssh1106_encoder`
    - `tomak79_left`
    - `tomak79_right`
    - `tomak79_right2`
    - `tomak79_right3`

## 보존 규칙

- 저장소 내부 `build/`는 현재 Git 포함 대상이다.
- `build/origin/`은 사용자 확인 없이 수정하거나 비우지 않는다.
- `build/test/`는 작업용이므로 필요 시 재빌드하거나 교체할 수 있다.
- 아래 성공 빌드 폴더는 사용자 확인 없이 삭제하지 않는다.
  - `tomak79_left`
  - `tomak79_right`
  - `tomak79_dongle_ssd1306_block`
  - `tomak79_dongle_ssh1106`
  - `tomak79_dongle_ssh1106_encoder`
  - `tomak79_dongle_yads_encoder`

## 다음 작업 메모

- 저장소 내부 테스트 빌드는 `build/test/`만 사용한다.
- 최종 배포용 산출물은 필요 시 상위 `~/ZMK_Keyboard/build/`에서도 다시 빌드해 맞춘다.
- underglow는 `left/right` 개별 상태 유지 방식에서 `dongle central` 기준의 중앙 통합형으로 전환하는 방향을 다음 작업 기준으로 둔다.
