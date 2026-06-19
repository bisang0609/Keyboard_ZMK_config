# codex.md

## 목적

ZMK 0.3.0 기반 tomak79 + dongle 구성 재작업.

## 현재 기준

- 작업 폴더: `~/ZMK_Keyboard`
- Git 저장소: `Keyboard_ZMK_config`
- 최종 원격 저장소: `https://github.com/bisang0609/Keyboard_ZMK_config`
- ZMK 버전: `v0.3`
- ZMK 본체 직접 수정 금지
- Zephyr 본체 직접 수정 금지
- modules 직접 수정 금지
- config 저장소만 Git 관리

## 유지해야 할 기능

- tomak79 left/right matrix
- tomak79 keymap
- tomak79 dongle keymap
- common physical layout / matrix transform
- dongle I2C SDA=P0.06, SCL=P0.08
- pro_micro_serial 비활성화

## 제외 항목

- oled/i2c 실험 파일
- artifacts
- 임시 스크립트
- ZMK 4.1 전용 실험 파일
- 기존 codex.md
- 기존 agent.md

## 작업 로그

### 2026-06-19

- 초기 환경 구성 시작
- `~/ZMK_Keyboard` 상위 폴더 생성
- `Keyboard_ZMK_config` 새 Git 저장소 생성
- 원격 저장소 연결: `https://github.com/bisang0609/Keyboard_ZMK_config.git`
- 기존 저장소 `zmk-config`는 `_old_zmk_config`로 참고용 clone
- 필요한 tomak79 설정 파일만 선별 복사
- `tomak79_dongle_i2c_test.*` 실험 파일 제거
- 기존 `codex.md`, `agent.md`는 복사하지 않음
- 새 `codex.md`, 새 `agent.md` 생성

### 2026-06-19 빌드 검증 결과

#### 수정한 파일

- `west.yml`
  - ZMK revision을 `v0.3`으로 고정
  - `import: app/west.yml` 추가
- `boards/shields/tomak79/Kconfig.shield`
  - `tomak79_dongle_i2c_test` 실험 shield 제거
- `boards/shields/tomak79/Kconfig.defconfig`
  - `tomak79_dongle_i2c_test` 조건 제거
- `boards/shields/tomak79/tomak79_dongle.overlay`
  - OLED compatible을 SSD1306으로 변경
  - I2C SDA=P0.06 유지
  - I2C SCL=P0.08 유지
  - `pro_micro_serial` disabled 유지
  - I2C `clock-frequency`를 `<100000>`으로 수정
- `boards/shields/tomak79/tomak79_left.conf`
  - WS2812 SPI RGB underglow용 설정 추가
- `boards/shields/tomak79/tomak79_right.conf`
  - WS2812 SPI RGB underglow용 설정 추가
- `config/tomak79_dongle.keymap`
  - dongle 빌드용 최소 `zmk,keymap` 노드 작성
- `build.yaml`
  - `tomak79_dongle_i2c_test` 실험 빌드 대상 제거
  - Studio snippet/cmake-args 제거
- `.gitignore`
- `.vscode/settings.json`
- `agent.md`

#### 실행한 주요 명령

```bash
west init -l Keyboard_ZMK_config
west update
