# codex.md

## 목적

- 이 저장소는 ZMK `v0.3` 기반 `tomak79` keyboard + dongle config만 관리한다.
- 목표는 기능 추가보다 빌드 가능 상태와 재현 가능한 환경을 우선 유지하는 것이다.

## 작업 기준

- 작업 폴더: `~/ZMK_Keyboard`
- Git 저장소: `~/ZMK_Keyboard/Keyboard_ZMK_config`
- 원격 저장소: `https://github.com/bisang0609/Keyboard_ZMK_config`
- ZMK revision: `v0.3`
- Git 관리 대상: `Keyboard_ZMK_config`만

## 수정 금지

- `../zmk/`
- `../zephyr/`
- `../modules/`
- `../bootloader/`

## 반드시 유지할 사항

- `tomak79` left/right matrix
- `tomak79` keymap
- `tomak79_dongle` keymap
- 공용 physical layout / matrix transform
- 기본 dongle I2C 핀
  - `SDA = P0.06`
  - `SCL = P0.08`
- `pro_micro_serial` 비활성화 유지

## 현재 남겨둔 실드

- `tomak79_left`
- `tomak79_right`
- `tomak79_dongle`
- `tomak79_dongle_ssd1306_block`

## 현재 상태

- 기본 keyboard/dongle 구성은 유지 중이다.
- 실험용 OLED 비교 실드들은 정리했다.
- 현재 OLED 안정화 기준은 `tomak79_dongle_ssd1306_block`이다.
- 이 안정화 실드는 ZMK/LVGL 전체 화면 대신 작은 block write 방식으로 SSD1306에 직접 표시한다.
- `dongle_display` 계열 UI는 실기에서 화면 깨짐이 있어 현재 기준선에서는 제외했다.

## OLED 안정화 실드 메모

- 실드 이름: `tomak79_dongle_ssd1306_block`
- 목적: USB와 OLED를 함께 안정 동작시키는 기준선 확보
- 표시 방식: 작은 `8x8` block 단위 직접 쓰기
- 현재 표시 내용
  - `LT / RT / KP` battery
  - `USB`
  - active layer
  - `CAP / NUM`
  - animated bongo cat

## 핀 관련 주의

- 기본 dongle overlay의 고정 I2C 핀은 반드시 `P0.06/P0.08`을 유지한다.
- 별도 OLED 안정화 실드 `tomak79_dongle_ssd1306_block`는 실기 검증 결과에 맞춰 별도 핀 구성을 사용한다.
- 따라서 기본 dongle과 OLED 실드의 핀 설정을 혼동해서 합치지 않는다.

## 최근 완료 작업

- SSD1306 실기 테스트 결과를 반영해 block 기반 OLED 실드를 추가했다.
- `tomak79_dongle_ssd1306_block`에 최종 OLED 레이아웃과 고양이 애니메이션을 반영했다.
- 비교/실험용 실드와 테스트 빌드 폴더를 정리했다.
- `build.yaml`, `Kconfig.shield`, `Kconfig.defconfig`를 현재 유지 실드 기준으로 정리했다.
- 실기 기준에서 불안정했던 `ddtry` 실드는 제외했다.

## 마지막 검증 결과

- 대상: `tomak79_dongle_ssd1306_block`
- 결과: 빌드 성공
- 메모리 사용량
  - FLASH `232108 B`
  - RAM `54250 B`
- 생성물
  - `build/tomak79_dongle_ssd1306_block/zephyr/zmk.uf2`
- 실기 확인
  - USB 정상
  - 최종 OLED 레이아웃 정상
  - `LT / RT / KP`, `USB / L0`, `CAP / NUM` 정상
  - 고양이 애니메이션 정상

## 제외 대상

- OLED/I2C 실험용 비교 실드
- 임시 빌드 산출물
- `artifacts/`
- 임시 스크립트
- ZMK 4.x 전용 실험 파일

## 다음 작업 후보

- 현재 OLED 안정화 버전 유지
- 필요 시 표시 위치/문구만 소폭 조정
