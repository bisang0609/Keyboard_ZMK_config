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
- `pro_micro_serial` 비활성화 유지
- Git 관리 대상은 `Keyboard_ZMK_config`만 유지

## 현재 유지 실드

- `tomak79_left`
- `tomak79_right`
- `tomak79_dongle`
- `tomak79_dongle_ssd1306_block`
- `tomak79_dongle_st7789_test`
- `tomak79_dongle_yads`

## 현재 상태

- `tomak79_dongle_ssd1306_block`는 SSD1306 안정화 기준 실드로 유지한다.
- `tomak79_dongle_st7789_test`는 ST7789 단독 표시 확인용 실드로 유지한다.
- `tomak79_dongle_yads`는 `zmk-dongle-screen` 기반 ST7789 동글 실드로 유지한다.
- `left/right`는 `MAX17048`를 `P0.06/P0.08` I2C로 읽도록 정리했다.
- `left/right`는 배터리 갱신 루프를 추가해 split 배터리 값이 idle 이후에도 다시 전달되도록 보강했다.
- `left/right`는 `RGB underglow`와 `EXT_POWER`를 분리해 `P0.13` 전원 제어 영향을 줄이도록 조정했다.

## 최근 완료 작업

- SSD1306 block OLED 레이아웃과 고양이 애니메이션을 마무리했다.
- ST7789 test/YADS 실드를 현재 저장소 구조에 맞게 추가했다.
- `MAX17048` 실기 로그를 바탕으로 `left/right` 배터리 읽기와 주기 갱신 경로를 정리했다.
- 임시 USB 로그 빌드 타깃과 중복 로그 코드를 정리했다.

## 마지막 빌드 결과

- `tomak79_left`: 빌드 성공
- `tomak79_right`: 직전 정상 구성 유지
- `tomak79_dongle_ssd1306_block`: 빌드 성공, 실기 표시 정상
- `tomak79_dongle_yads + dongle_screen`: 빌드 성공

## 다음 작업 후보

- `right` 실기 배터리 값 확인
- `tomak79_dongle_st7789_test` 화면 크기/배치 미세 조정
- `tomak79_dongle_yads` 실기 UI 조정
