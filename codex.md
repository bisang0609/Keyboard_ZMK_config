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
```

### 2026-06-19 dongle SSD1306 분리 작업

#### 수정한 파일

- `boards/shields/tomak79/Kconfig.shield`
  - `tomak79_dongle_ss1306` shield 추가
- `boards/shields/tomak79/Kconfig.defconfig`
  - `tomak79_dongle_ss1306`에도 display/i2c 기본 설정이 적용되도록 조건 확장
- `boards/shields/tomak79/tomak79_dongle_ss1306.conf`
  - split central + USB + display용 기본 Kconfig 추가
- `boards/shields/tomak79/tomak79_dongle_ss1306.overlay`
  - 새 SSD1306 dongle overlay 추가
  - I2C SDA=`P1.15`, SCL=`P1.13` 적용
  - `pro_micro_serial` disabled 유지
  - `zmk,kscan`은 mock kscan 유지
- `boards/shields/tomak79/tomak79_dongle_ss1306.zmk.yml`
  - 새 shield 메타데이터 추가
- `build.yaml`
  - `tomak79_dongle_ss1306` 빌드 타깃 추가

#### 빌드 결과

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_ss1306 -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_ss1306 -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config
```

- 결과:
  - `tomak79_dongle_ss1306` 빌드 성공
  - 생성물: `build/tomak79_dongle_ss1306/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `314424 B` / RAM `64038 B`
  - keymap은 별도 파일을 추가하지 않고 기존 `config/tomak79_dongle.keymap`을 공용 사용
  - 동일 조건으로 `build/tomak79_dongle_ss1306_verify` 재빌드 검증도 성공

#### 커스텀 표시 모듈

- `englmaxi/zmk-dongle-display` 모듈을 사용하기로 결정
- `west.yml`에 `zmk-dongle-display`를 추가하고 revision은 `v0.3`으로 고정
- `build.yaml`에 `tomak79_dongle_ss1306 dongle_display` 조합 빌드 타깃 추가

#### 커스텀 표시 모듈 빌드 결과

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_ss1306_display -b nice_nano_v2 ../zmk/app -- -DSHIELD="tomak79_dongle_ss1306 dongle_display" -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config
```

- 결과:
  - `tomak79_dongle_ss1306 dongle_display` 조합 빌드 성공
  - 생성물: `build/tomak79_dongle_ss1306_display/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `314364 B` / RAM `68430 B`
  - base shield는 `pro_micro_i2c` + SSD1306 `128x64` 구성을 유지하고, 화면 구성만 `dongle_display`로 대체

#### 다음 작업 후보

- `tomak79_dongle_st7789` shield 추가
- ST7789 SPI 핀맵 `MOSI=P1.15`, `SCK=P1.13`, `CS=P1.06`, `DC=P1.04`, `RST=P0.11`, `BL=P0.10` 반영
- ST7789용 display driver/Kconfig 최소 설정 정리

### 2026-06-19 dongle ST7789 분리 작업

#### 수정한 파일

- `boards/shields/tomak79/Kconfig.shield`
  - `tomak79_dongle_st7789` shield 추가
- `boards/shields/tomak79/Kconfig.defconfig`
  - `tomak79_dongle_st7789` 전용 SPI/display/LED 기본 설정 추가
- `boards/shields/tomak79/tomak79_dongle_st7789.conf`
  - split central + USB + display + ST7789/LED용 기본 Kconfig 추가
- `boards/shields/tomak79/tomak79_dongle_st7789.overlay`
  - 새 ST7789 dongle overlay 추가
  - SPI `MOSI=P1.15`, `SCK=P1.13`, `CS=P1.06` 적용
  - 제어 핀 `DC=P1.04`, `RST=P0.11`, `BL=P0.10` 적용
  - `zmk,display-led`를 backlight GPIO LED로 연결
  - `pro_micro_serial` disabled 유지
  - `pro_micro_i2c` disabled 처리
  - `zmk,kscan`은 mock kscan 유지
- `boards/shields/tomak79/tomak79_dongle_st7789.zmk.yml`
  - 새 shield 메타데이터 추가
- `build.yaml`
  - `tomak79_dongle_st7789` 빌드 타깃 추가

#### 빌드 결과

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_st7789 -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_st7789 -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config
```

- 결과:
  - `tomak79_dongle_st7789` 빌드 성공
  - 생성물: `build/tomak79_dongle_st7789/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `316436 B` / RAM `84022 B`
  - keymap은 별도 파일을 추가하지 않고 기존 `config/tomak79_dongle.keymap`을 공용 사용

### 2026-06-20 SSD1306 표시 깨짐 / USB 인식 불안정 대응

#### 확인된 증상

- `tomak79_dongle_ss1306 dongle_display` 펌웨어를 올린 뒤
  - SSD1306 화면이 깨져 보임
  - USB 인식이 정상적으로 되지 않는 현상 보고

#### 이번 대응 목적

- `englmaxi/zmk-dongle-display` 사용은 유지
- SSD1306 `128x64` 조건은 유지
- overlay와 display buffer 설정을 보수적으로 낮춰
  - 표시 안정성을 먼저 확인
  - USB enumeration에 영향을 줄 수 있는 display 부하를 줄여 재검증

#### 수정한 파일

- `boards/shields/tomak79/tomak79_dongle_ss1306.conf`
  - `CONFIG_LV_Z_VDB_SIZE=32` 추가
  - `CONFIG_ZMK_DONGLE_DISPLAY_BONGO_CAT=n` 추가
- `boards/shields/tomak79/tomak79_dongle_ss1306.overlay`
  - SSD1306 `segment-offset`를 `2`에서 `0`으로 변경
  - `inversion-on` 제거
  - I2C 속도는 `100000` 유지
  - 핀은 계속 `SDA=P1.15`, `SCL=P1.13` 유지

#### 빌드 결과

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_ss1306_display -b nice_nano_v2 ../zmk/app -- -DSHIELD="tomak79_dongle_ss1306 dongle_display" -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config
```

- 결과:
  - `tomak79_dongle_ss1306 dongle_display` 재빌드 성공
  - 생성물: `build/tomak79_dongle_ss1306_display/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `311912 B` / RAM `67966 B`
  - 이전 커스텀 display 빌드 대비 RAM 사용량이 소폭 감소

#### 현재 판단

- 빌드는 정상이며 USB stack 자체는 config 상 활성화 상태
- 이번 빌드는 화면 파라미터와 display workload를 줄인 안정화용 테스트 이미지
- 실제 보드에서
  - USB 인식 복구 여부
  - 화면 깨짐 감소 여부
  - 화면 방향/좌우 반전 필요 여부
  를 우선 확인해야 함

#### 다음 작업 후보

- 안정화 빌드로 실기 테스트
- 여전히 USB 인식 문제가 있으면 `CONFIG_ZMK_STUDIO` 비활성화 버전 추가 검증
- 여전히 화면이 깨지면 `segment-remap` / `com-invdir` / I2C 속도 조합을 단계적으로 재조정

### 2026-06-20 SSD1306 아두이노 기준 코드 대조 후 추가 축소

#### 기준 확인

- `boards/shields/tomak79/ss1306.cpp` 확인
- 동일 보드/동일 핀에서 Arduino 테스트는
  - `Wire`
  - 주소 `0x3C`
  - `128x64`
  - 별도 `segment-offset`, `segment-remap`, `com-invdir` 없이 동작

#### 이번 추가 조정

- `boards/shields/tomak79/tomak79_dongle_ss1306.conf`
  - `CONFIG_ZMK_STUDIO=n`
  - `CONFIG_ZMK_DONGLE_DISPLAY_MODIFIERS=n`
  - `CONFIG_ZMK_DONGLE_DISPLAY_LAYER=n`
  - `CONFIG_ZMK_DONGLE_DISPLAY_WPM=n`
- `boards/shields/tomak79/tomak79_dongle_ss1306.overlay`
  - `segment-remap` 제거
  - `com-invdir` 제거
  - SSD1306 초기화를 Arduino 예제에 더 가깝게 단순화

#### 빌드 결과 1: dongle_display 유지 버전

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_ss1306_display -b nice_nano_v2 ../zmk/app -- -DSHIELD="tomak79_dongle_ss1306 dongle_display" -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_ss1306_display/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `294484 B` / RAM `59304 B`
  - `ZMK Studio`와 일부 커스텀 위젯을 제거한 축소 버전

#### 빌드 결과 2: base status screen 비교용 버전

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_ss1306_base -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_ss1306 -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_ss1306_base/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `298452 B` / RAM `62808 B`
  - `dongle_display` 없이 기본 ZMK status screen만 사용하는 비교용 이미지

#### 현재 판단

- 핀 자체보다는
  - `dongle_display` 런타임 부하
  - `ZMK Studio`
  - SSD1306 orientation/segment 옵션
  중 하나가 실기 문제를 만들 가능성이 높음
- 이제 실기에서는 아래 순서로 원인 분리가 가능함
  - 먼저 `tomak79_dongle_ss1306_base`
  - 그 다음 `tomak79_dongle_ss1306_display`

#### 다음 작업 후보

- base 이미지에서 USB/화면이 정상인지 확인
- base는 정상이고 display만 실패하면 `dongle_display` 설정만 추가 축소
- base도 실패하면 SSD1306 overlay 또는 동글 하드웨어 연결 조건을 다시 점검

### 2026-06-20 SH1106 호환성 및 핀 간섭 가능성 반영

#### 실기 결과

- `tomak79_dongle_ss1306_base`
- `tomak79_dongle_ss1306_display`

두 이미지 모두 실기에서
- 화면 깨짐
- USB 인식 불가
가 동일하게 발생

#### 추가 판단

- `dongle_display` 유무와 무관하게 동일 증상이라
  - 커스텀 status screen 자체보다는
  - display 드라이버 초기화 단계
  - SSD1306/SH1106 컨트롤러 차이
  - overlay 상 추가 핀 제어
  중 하나가 원인일 가능성이 커짐

#### 이번 추가 수정

- `boards/shields/tomak79/tomak79_dongle_ss1306.overlay`
  - `gpio-hog`로 잡아 두었던 `P0.15` test LED 제거
  - display `compatible`을 `solomon,ssd1306fb`에서 `sinowealth,sh1106`로 변경
  - `segment-offset = <2>` 적용
  - `segment-remap`, `com-invdir` 재적용

#### 빌드 결과 1: SH1106 base 버전

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_ss1306_base -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_ss1306 -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_ss1306_base/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `298388 B` / RAM `62808 B`

#### 빌드 결과 2: SH1106 + dongle_display 버전

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_ss1306_display -b nice_nano_v2 ../zmk/app -- -DSHIELD="tomak79_dongle_ss1306 dongle_display" -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_ss1306_display/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `294436 B` / RAM `59304 B`

#### 현재 다음 판단

- 이번 이미지는
  - SH1106 계열 128x64 모듈 호환성
  - 기존 `P0.15` hog 제거
를 함께 반영한 첫 테스트 이미지
- 이 버전도 동일 증상이면, 다음 단계는 display를 아예 init하지 않는 진단용 펌웨어를 만들어
  - USB만 먼저 살리는지
  - display init 자체가 부팅을 막는지
  를 분리 확인해야 함

### 2026-06-20 no-display USB 진단 펌웨어 추가

#### 목적

- LCD 초기화가 실제 원인인지 분리하기 위해
  - `zephyr,display` 제거
  - `pro_micro_i2c` 비활성화
  - `CONFIG_ZMK_DISPLAY=n`
인 진단용 dongle 이미지를 별도 추가

#### 추가한 파일

- `boards/shields/tomak79/tomak79_dongle_ss1306_nodisplay.conf`
- `boards/shields/tomak79/tomak79_dongle_ss1306_nodisplay.overlay`
- `boards/shields/tomak79/tomak79_dongle_ss1306_nodisplay.zmk.yml`
- `boards/shields/tomak79/Kconfig.shield`
- `boards/shields/tomak79/Kconfig.defconfig`
- `build.yaml`

#### 빌드 결과

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_ss1306_nodisplay -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_ss1306_nodisplay -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_ss1306_nodisplay/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `207764 B` / RAM `46664 B`

#### 해석

- 이 이미지는 display 드라이버와 I2C display 초기화를 완전히 제거한 기준 이미지
- 이 이미지에서 USB가 정상 인식되면 원인은 LCD 경로
- 이 이미지에서도 USB가 인식되지 않으면 원인은 LCD가 아니라 dongle 기본 구성 또는 보드 상태 쪽

### 2026-06-20 USB 전용 최소 진단 펌웨어

#### 목적

- `nice_nano_v2` 자체의 USB HID 동작만 확인하기 위해
  - `split` 비활성화
  - `BLE` 비활성화
  - `display` 비활성화
  - `sleep` 비활성화
  - `battery reporting` 비활성화
한 최소 dongle 진단 이미지 추가

#### 추가한 파일

- `boards/shields/tomak79/tomak79_dongle_usbdiag.conf`
- `boards/shields/tomak79/tomak79_dongle_usbdiag.overlay`
- `boards/shields/tomak79/tomak79_dongle_usbdiag.zmk.yml`
- `boards/shields/tomak79/Kconfig.shield`
- `boards/shields/tomak79/Kconfig.defconfig`
- `build.yaml`

#### 빌드 결과

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_usbdiag -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_usbdiag -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_usbdiag/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `43284 B` / RAM `11992 B`

#### 해석

- 이 이미지는 사실상 `nice_nano_v2 + USB + mock kscan`만 남긴 최소 ZMK 이미지
- 이 이미지에서도 USB가 인식되지 않으면
  - tomak79 LCD 작업과 무관
  - split/BLE와도 무관
  - 보드 상태, 플래시 상태, 또는 더 낮은 레벨의 펌웨어/부트 후 실행 문제 가능성이 큼

### 2026-06-20 부트로더/핀맵/예전 설정 재검증

#### 사용자 제공 정보

- 부트로더 정보
  - `UF2 Bootloader 0.11.0`
  - `Model: nice!nano`
  - `Board-ID: nRF52840-nicenano`
  - `Date: May 21 2026`
  - `SoftDevice: S140 6.1.1`
- 핀맵 이미지 기준
  - `D1=P0.06`, `D0=P0.08`
  - `D18=P1.15`, `D15=P1.13`
  - `D9=P1.06`, `D8=P1.04`
  - `D7=P0.11`, `D16=P0.10`

#### 판단

- 부트로더 정보상 보드 타깃은 계속 `nice_nano_v2`가 맞음
- 사용자 보고처럼 예전에는 USB가 동작했다면
  - 현재 SSD1306/ST7789 설정만의 문제인지
  - 예전 dongle 설정 자체도 지금 환경에서 재현되는지
  를 분리 확인할 필요가 생김

#### 원격 예전 저장소 확인

- 확인 대상: `https://github.com/bisang0609/zmk-config`
- `main` 브랜치 최신 커밋과 과거 이력을 조회해 dongle 관련 시점을 확인
- 확인된 주요 커밋
  - `4d23f81` `Fix ZMK 4.1 local build warnings`
  - `8a0c7ba` `Use TWIM for dongle OLED I2C bus`
  - `26268ac` `Use TWIM for dongle I2C test bus`
  - `ab60c69` `Add dongle D0/D1 I2C test target`
  - `59572d5` `Disable UART0 on dongle D0/D1 I2C pins`
  - `11cccd1` `Move dongle OLED I2C to D1/D0 and disable encoder`
  - `b8cb89a` `Match reference SH1106 128x64 config`

#### 확인된 중요한 사실

- 이름은 `Fix ZMK 4.1 local build warnings`지만
  해당 시점의 `config/west.yml`은 실제로 `v0.3.0`을 가리킴
- 즉 사용자가 기억한 "이전 4.1" 상태는
  실제 저장소 내용 기준으로는 `ZMK v0.3.0` 기반일 가능성이 높음

#### 예전 커밋 재현 빌드

- 임시 clone 경로: `/tmp/zmk-config-4d23f81`
- 대상 커밋: `4d23f81`
- 첫 재현 빌드에서는 예전 overlay의
  `clock-frequency = <10000>;`
  때문에 현재 Zephyr TWIM 드라이버에서 정적 assert로 실패
- 임시 clone 안에서만 해당 값을 `100000`으로 바꾼 뒤 재빌드
- 이 수정은 현재 저장소가 아니라 `/tmp` 임시 복제본에서만 수행

#### 재현 빌드 결과

- 실행 명령:

```bash
west build -p -d /tmp/zmk-config-4d23f81/build/tomak79_dongle -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle -DZMK_CONFIG=/tmp/zmk-config-4d23f81/config -DZMK_EXTRA_MODULES=/tmp/zmk-config-4d23f81
```

- 결과:
  - 예전 `tomak79_dongle` 구성이 현재 환경에서도 빌드 성공
  - 생성물: `/tmp/zmk-config-4d23f81/build/tomak79_dongle/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `314424 B` / RAM `64038 B`

#### 현재 결론

- 현재 저장소의 `ss1306`, `nodisplay`, `usbdiag`, `usbdiag isolated`가 모두 USB 불가였다면
  LCD 설정만의 문제로 보긴 어려움
- 예전 저장소 커밋도 현재 환경에서 UF2까지 재생성 가능하므로
  다음 실기 비교 대상은
  `/tmp/zmk-config-4d23f81/build/tomak79_dongle/zephyr/zmk.uf2`
  가 됨
- 이 예전 재현 UF2까지 USB가 안 잡히면
  원인은 현재 config 변경분보다
  보드 상태, 플래시 상태, 또는 런타임 환경 쪽일 가능성이 더 커짐

#### 다음 작업 후보

- 예전 커밋 재현 UF2를 실기 플래시해서 USB 인식 여부 확인
- 동일 실패 시
  - LCD 분기 디버깅은 중단
  - 부트로더/플래시 상태/전원/케이블/리셋 동작 쪽으로 진단 전환

### 2026-06-20 초최소 USB 전용 쉴드 추가

#### 목적

- 기존 `usbdiag`도 `tomak79.dtsi`와 공용 `config/tomak79.conf` 영향을 일부 받기 때문에
  그것마저 완전히 제거한 비교 기준을 만들기 위함
- 즉
  - 공용 matrix/layout 정의 제거
  - 공용 dongle keymap 제거
  - 공용 split/BLE 기본 config 제거
  한 상태에서 `nice_nano_v2 + USB + mock kscan + 1키 레이아웃`만 남긴 이미지 추가

#### 추가한 파일

- `boards/shields/tomak79/tomak79_dongle_usbbare.conf`
- `boards/shields/tomak79/tomak79_dongle_usbbare.overlay`
- `boards/shields/tomak79/tomak79_dongle_usbbare.zmk.yml`
- `config/tomak79_dongle_usbbare.keymap`
- `config/usbbare/tomak79.conf`
- `config/usbbare/tomak79_dongle_usbbare.keymap`
- `boards/shields/tomak79/Kconfig.shield`
- `boards/shields/tomak79/Kconfig.defconfig`
- `build.yaml`

#### 빌드 결과 1: 일반 config 경로

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_usbbare -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_usbbare -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_usbbare/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `40708 B` / RAM `11320 B`
  - 다만 이 경로에서는 공용 `config/tomak79_dongle.keymap`이 자동 선택되어
    진단 기준으로는 불완전함

#### 빌드 결과 2: 완전 분리 isolated config 경로

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_usbbare_isolated -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_usbbare -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config/usbbare
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_usbbare_isolated/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `40600 B` / RAM `11312 B`
  - 이번 빌드는 실제로
    - 공용 `tomak79.conf` 없음
    - 공용 `tomak79_dongle.keymap` 없음
    - 공용 `tomak79.dtsi` 없음
    상태에서 생성된 첫 초최소 USB 전용 이미지

#### 현재 판단

- 이 isolated 이미지까지 새 보드에서 USB가 안 잡히면
  현재 저장소의
  - OLED 설정
  - ST7789 설정
  - split 설정
  - tomak79 공용 matrix/layout 정의
  가 USB 문제의 원인일 가능성은 매우 낮음
- 그 경우 원인 축은
  - 플래시 절차
  - 부트로더/런타임 handoff
  - 보드 하드웨어/USB 경로
  쪽으로 좁혀짐

### 2026-06-20 known-good dongle UF2 비교로 USB 원인 재축소

#### 기준 확인

- 사용자가 정상 동작 사례로
  `redmasters/zmk-config-dongle`
  의 `firmware/nice_corne_dongle_oled.uf2`를 제공
- 이 UF2를 새 보드에 플래시했을 때 USB가 정상 인식됨

#### 확인된 의미

- 보드 자체, 부트로더 자체, USB 케이블 자체가 항상 실패하는 상태는 아님
- 즉 문제는 `tomak79` 쪽 현재 펌웨어 구성에서 부팅 초기에 죽는 경로로 다시 좁혀짐

#### 비교한 핵심 차이

- `redmasters/zmk-config-dongle`도 `ZMK v0.3.0` 기반
- `nice_corne_dongle_oled.uf2`는 문자열상
  - `Nice_Dongle_O`
  - `snippet_studio_rpc_usb_uart`
  를 포함
- 즉 `dongle_display`, `OLED`, `Studio`가 같이 있어도 USB는 정상 동작 가능
- 가장 수상한 차이로 확인된 부분은 `kscan-mock` 정의
  - 동작하는 기준 overlay:
    - `columns = <0>;`
    - `rows = <0>;`
    - `events = <0>;`
  - 기존 tomak79 dongle overlay들:
    - `columns = <16>;` 또는 `<1>;`
    - `rows = <6>;` 또는 `<1>;`
    - `events = <>;`

#### 이번 수정

- 아래 overlay들의 `kscan-mock` 정의를 기준 저장소와 같은 형태로 변경
  - `boards/shields/tomak79/tomak79_dongle.overlay`
  - `boards/shields/tomak79/tomak79_dongle_ss1306.overlay`
  - `boards/shields/tomak79/tomak79_dongle_ss1306_nodisplay.overlay`
  - `boards/shields/tomak79/tomak79_dongle_st7789.overlay`
  - `boards/shields/tomak79/tomak79_dongle_usbdiag.overlay`
  - `boards/shields/tomak79/tomak79_dongle_usbbare.overlay`

#### 재빌드 결과

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_usbbare_isolated -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_usbbare -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config/usbbare
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_usbbare_isolated/zephyr/zmk.uf2`
  - export 복사본:
    `/home/bisang0609/ZMK_Keyboard/firmware_exports/tomak79_dongle_usbbare_isolated.uf2`
  - 메모리 사용량: FLASH `40876 B` / RAM `11312 B`

#### 현재 다음 판단

- 이 수정본에서 USB가 살아나면
  원인은 `mock_kscan`/dongle input 정의 차이였을 가능성이 큼
- 이 수정본도 USB가 안 되면
  그다음 비교 대상은
  `redmasters` 쪽 `corne_dongle_pro_micro` 구조를 tomak79용으로 더 직접 옮기는 단계

### 2026-06-20 redmasters corne 구조 추가 대조 반영

#### 추가 확인

- `redmasters/zmk-config-dongle`의 `boards/shields/corne`를 직접 대조
- 확인된 중요점
  - USB가 되는 `nice_corne_dongle_oled.uf2`는 오히려
    `split + BLE + Studio + dongle_display + OLED`
    가 모두 포함된 비교적 큰 이미지
  - 따라서 단순히 기능을 줄인다고 USB가 살아나는 문제는 아님

#### 추가 반영한 설정 차이

- `redmasters`의 `corne_dongle_pro_micro.conf` 기준으로 아래 옵션을 tomak79 dongle 계열에 반영
  - `CONFIG_ZMK_SPLIT_BLE_CENTRAL_PERIPHERALS=2`
  - `CONFIG_BT_MAX_CONN=5`
  - `CONFIG_BT_MAX_PAIRED=5`
  - `CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_BASIC=y`

#### 재빌드 대상

- display를 끄되, dongle central 구조는 유지한 비교본으로
  `tomak79_dongle_ss1306_nodisplay`를 재빌드

#### 빌드 결과

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_ss1306_nodisplay -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_ss1306_nodisplay -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_ss1306_nodisplay/zephyr/zmk.uf2`
  - export 복사본:
    `/home/bisang0609/ZMK_Keyboard/firmware_exports/tomak79_dongle_ss1306_nodisplay_redaligned.uf2`
  - 메모리 사용량: FLASH `207912 B` / RAM `44508 B`

#### 현재 다음 판단

- 이 비교본에서 USB가 살아나면
  원인은
  - `mock_kscan`
  - dongle central 옵션
  - HID/basic endpoint 옵션
  중 하나였을 가능성이 큼
- 이것도 USB가 안 되면
  다음 단계는 `corne_dongle_pro_micro.overlay` 구조를 tomak79 전용 임시 shield로 거의 그대로 이식하는 방향

### 2026-06-20 redmasters 재현 조건으로 USB 복구 경로 확인

#### 핵심 확인

- `redmasters/zmk-config-dongle`의 저장소 제공 UF2는 USB가 정상 인식됨
- 같은 저장소를 로컬에서 단순 수동 빌드한 초기 결과물은 USB가 되지 않았음
- 이후 아래 조건을 포함해 다시 재현 빌드했을 때 USB가 정상 동작하는 방향을 확인
  - `studio-rpc-usb-uart` snippet 포함
  - 짧은 `CONFIG_ZMK_KEYBOARD_NAME`

#### tomak79용 추가 실드

- `boards/shields/tomak79/tomak79_dongle_usbfix.conf`
- `boards/shields/tomak79/tomak79_dongle_usbfix.overlay`
- `boards/shields/tomak79/tomak79_dongle_usbfix.zmk.yml`
- `boards/shields/tomak79/Kconfig.shield`
- `boards/shields/tomak79/Kconfig.defconfig`
- `build.yaml`

#### usbfix 실드 빌드 조정

- `build.yaml`의 `tomak79_dongle_usbfix` 항목에 아래 조건 반영
  - `snippet: studio-rpc-usb-uart`
  - `cmake-args: -DCONFIG_ZMK_KEYBOARD_NAME=\"Tomak79USB\"`

#### 마지막 빌드 결과

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_usbfix -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_usbfix -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config -DSNIPPET="studio-rpc-usb-uart" -DCONFIG_ZMK_KEYBOARD_NAME=\"Tomak79USB\"
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_usbfix/zephyr/zmk.uf2`
  - 메모리 사용량: FLASH `233660 B` / RAM `60250 B`
  - UF2 크기: `467456 B`
  - 직전 실패 원인이었던 BLE 이름 길이 초과 문제는 해소됨

#### 현재 다음 작업 후보

- `tomak79_dongle_usbfix` UF2를 실보드에 올려 USB 인식 여부 최종 확인
- USB가 안정적이면 그 기준에서 SSD1306 `128x64`를 다시 얹기
- SSD1306 재적용 시에도 dongle 기본 I2C 핀 고정 조건(`P0.06`/`P0.08`)과 별도 SSD1306 실드의 실제 배선 차이를 분리해서 관리

### 2026-06-20 실패한 실험 파일 정리

#### 정리 원칙

- USB가 복구된 기준은 `tomak79_dongle_usbfix`만 유지
- 실패 확인용으로만 만들었던 실험 실드와 보조 파일은 저장소에서 제거

#### 제거한 항목

- `tomak79_dongle_ss1306*`
- `tomak79_dongle_ss1306_nodisplay*`
- `tomak79_dongle_st7789*`
- `tomak79_dongle_usbbare*`
- `tomak79_dongle_usbdiag*`
- SSD1306 Arduino 참고 파일과 보드 이미지 참고 파일
- `config/usbbare/`
- `config/usbdiag/`

#### 현재 유지 대상

- `tomak79_left`
- `tomak79_right`
- `tomak79_dongle`
- `tomak79_dongle_usbfix`

#### 현재 다음 작업 후보

- `tomak79_dongle_usbfix` 기준으로 USB 안정성 재확인
- 그다음 SSD1306 재도입은 새 실드로 다시 최소 범위에서 진행

### 2026-06-20 SSD1306 최소 재도입 시작

#### 목적

- USB가 복구된 `tomak79_dongle_usbfix`를 기준선으로 유지
- SSD1306은 별도 최소 실드로만 다시 추가
- 첫 단계는 `dongle_display` 없이 순수 SSD1306 `128x64`만 확인

#### 추가한 파일

- `boards/shields/tomak79/tomak79_dongle_ssd1306_min.conf`
- `boards/shields/tomak79/tomak79_dongle_ssd1306_min.overlay`
- `boards/shields/tomak79/tomak79_dongle_ssd1306_min.zmk.yml`

#### 반영한 설정

- `build.yaml`
  - `tomak79_dongle_ssd1306_min` 빌드 타깃 추가
  - `snippet: studio-rpc-usb-uart`
  - `cmake-args: -DCONFIG_ZMK_KEYBOARD_NAME=\"Tomak79OLED\"`
- `boards/shields/tomak79/Kconfig.shield`
  - `SHIELD_TOMAK79_DONGLE_SSD1306_MIN` 추가
- `boards/shields/tomak79/Kconfig.defconfig`
  - 새 실드도 공통 `tomak79` 계열 기본값을 사용하도록 조건 확장

#### SSD1306 최소 실드 기준

- USB/Studio/BLE central 경로는 `usbfix`와 동일 유지
- 표시 기능만 추가:
  - `CONFIG_ZMK_DISPLAY=y`
  - `CONFIG_LV_Z_VDB_SIZE=32`
- 실제 OLED 테스트 배선용 핀:
  - `SDA = P1.15`
  - `SCL = P1.13`
- OLED 노드:
  - 주소 `0x3C`
  - 크기 `128x64`
  - `segment-offset = 0`
  - `inversion-on`, `segment-remap`, `com-invdir`는 일단 제외

#### 빌드 결과

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_ssd1306_min -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_ssd1306_min -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config -DSNIPPET="studio-rpc-usb-uart" -DCONFIG_ZMK_KEYBOARD_NAME=\"Tomak79OLED\"
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_ssd1306_min/zephyr/zmk.uf2`
  - export 복사본:
    `/home/bisang0609/ZMK_Keyboard/firmware_exports/tomak79_dongle_ssd1306_min.uf2`
  - 메모리 사용량: FLASH `324308 B` / RAM `76394 B`
  - UF2 크기: `648704 B`

#### 현재 다음 작업 후보

- `tomak79_dongle_ssd1306_min.uf2`를 실보드에 올려
  - USB 인식 유지 여부
  - 화면 정상 표시 여부
  - 화면 반전/깨짐 여부
  를 먼저 확인
- 이 최소판이 안정적이면 그다음에만 `dongle_display`를 다시 얹기

#### 참고 저장소 대조 후 추가 반영

- `redmasters/zmk-config-dongle`의 `corne_dongle_pro_micro` 구현과 다시 대조
- 추가로 맞춘 항목
  - OLED node에 `segment-remap`, `com-invdir`, `inversion-on` 추가
  - `CONFIG_BT_BAS=n`
  - `CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING=y`
  - `CONFIG_ZMK_BATTERY_REPORT_INTERVAL=60`
  - `SHIELD_TOMAK79_DONGLE_SSD1306_MIN` 에서 `ZMK_DISPLAY_WORK_QUEUE_DEDICATED` 기본 적용
  - LVGL 기본값을 참고 저장소와 같은 1bpp 계열로 정렬

#### 참고 저장소 반영 후 재빌드 결과

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_ssd1306_min/zephyr/zmk.uf2`
  - export 복사본 갱신:
    `/home/bisang0609/ZMK_Keyboard/firmware_exports/tomak79_dongle_ssd1306_min.uf2`
  - 메모리 사용량: FLASH `336412 B` / RAM `68578 B`
  - UF2 크기: `673280 B`

### 2026-06-20 SH1106 비교본 추가

#### 배경

- `tomak79_dongle_ssd1306_min`에서 USB는 정상이나 화면이 깨짐
- 따라서 USB 경로는 유지한 채, OLED 컨트롤러만 `SH1106`으로 바꾼 비교본 추가

#### 추가한 파일

- `boards/shields/tomak79/tomak79_dongle_sh1106_min.conf`
- `boards/shields/tomak79/tomak79_dongle_sh1106_min.overlay`
- `boards/shields/tomak79/tomak79_dongle_sh1106_min.zmk.yml`

#### 반영 내용

- `build.yaml`
  - `tomak79_dongle_sh1106_min` 타깃 추가
  - `cmake-args: -DCONFIG_ZMK_KEYBOARD_NAME=\"Tomak79SH11\"`
- `boards/shields/tomak79/Kconfig.shield`
  - `SHIELD_TOMAK79_DONGLE_SH1106_MIN` 추가
- `boards/shields/tomak79/Kconfig.defconfig`
  - 새 비교본도 동일한 display work queue / LVGL 기본값 사용

#### SH1106 비교본 기준

- 배선은 동일
  - `SDA = P1.15`
  - `SCL = P1.13`
- 표시 노드만 변경
  - `compatible = "sinowealth,sh1106"`
  - `width = <129>`
  - `height = <64>`
  - `segment-offset = <1>`

#### 빌드 결과

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_sh1106_min -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_sh1106_min -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config -DSNIPPET="studio-rpc-usb-uart" -DCONFIG_ZMK_KEYBOARD_NAME=\"Tomak79SH11\"
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_sh1106_min/zephyr/zmk.uf2`
  - export 복사본:
    `/home/bisang0609/ZMK_Keyboard/firmware_exports/tomak79_dongle_sh1106_min.uf2`
  - 메모리 사용량: FLASH `336412 B` / RAM `68586 B`
  - UF2 크기: `673280 B`

#### 현재 다음 작업 후보

- `tomak79_dongle_sh1106_min.uf2`를 올려 화면 깨짐이 사라지는지 확인
- SH1106가 정상이고 SSD1306만 깨지면 실제 모듈은 SH1106 계열로 판단

### 2026-06-20 SSD1306 plain 비교본 추가

#### 배경

- `tomak79_dongle_ssd1306_min`은
  `segment-remap`, `com-invdir`, `inversion-on` 을 포함한 상태
- 해당 옵션 조합 자체가 노이즈 원인인지 확인하기 위해
  완전 plain SSD1306 비교본 추가

#### 추가한 파일

- `boards/shields/tomak79/tomak79_dongle_ssd1306_plain.conf`
- `boards/shields/tomak79/tomak79_dongle_ssd1306_plain.overlay`
- `boards/shields/tomak79/tomak79_dongle_ssd1306_plain.zmk.yml`

#### plain 비교본 기준

- 배선 동일
  - `SDA = P1.15`
  - `SCL = P1.13`
- 컨트롤러는 그대로 `solomon,ssd1306fb`
- OLED 노드에서 아래 옵션만 제거
  - `segment-remap`
  - `com-invdir`
  - `inversion-on`

#### 빌드 결과

- 실행 명령:

```bash
west build -p -d build/tomak79_dongle_ssd1306_plain -b nice_nano_v2 ../zmk/app -- -DSHIELD=tomak79_dongle_ssd1306_plain -DZMK_CONFIG=/home/bisang0609/ZMK_Keyboard/Keyboard_ZMK_config/config -DSNIPPET="studio-rpc-usb-uart" -DCONFIG_ZMK_KEYBOARD_NAME=\"Tomak79PLN\"
```

- 결과:
  - 빌드 성공
  - 생성물: `build/tomak79_dongle_ssd1306_plain/zephyr/zmk.uf2`
  - export 복사본:
    `/home/bisang0609/ZMK_Keyboard/firmware_exports/tomak79_dongle_ssd1306_plain.uf2`
  - 메모리 사용량: FLASH `336408 B` / RAM `68570 B`
  - UF2 크기: `673280 B`

#### 현재 다음 작업 후보

- 아래 3개를 비교 테스트
  - `tomak79_dongle_ssd1306_min.uf2`
  - `tomak79_dongle_ssd1306_plain.uf2`
  - `tomak79_dongle_sh1106_min.uf2`
- 이 비교로
  - 초기화 옵션 문제인지
  - 패널 타입 문제인지
  를 우선 판별

### 2026-06-20 02:05:24 KST 자동 작업 기록

#### 변경 파일

-  M boards/shields/tomak79/Kconfig.defconfig
-  M boards/shields/tomak79/Kconfig.shield
-  M boards/shields/tomak79/tomak79_dongle.conf
-  M boards/shields/tomak79/tomak79_dongle.overlay
-  M build.yaml
-  M codex.md
-  M west.yml
- ?? boards/shields/tomak79/tomak79_dongle_sh1106_min.conf
- ?? boards/shields/tomak79/tomak79_dongle_sh1106_min.overlay
- ?? boards/shields/tomak79/tomak79_dongle_sh1106_min.zmk.yml
- ?? boards/shields/tomak79/tomak79_dongle_ssd1306_min.conf
- ?? boards/shields/tomak79/tomak79_dongle_ssd1306_min.overlay
- ?? boards/shields/tomak79/tomak79_dongle_ssd1306_min.zmk.yml
- ?? boards/shields/tomak79/tomak79_dongle_ssd1306_plain.conf
- ?? boards/shields/tomak79/tomak79_dongle_ssd1306_plain.overlay
- ?? boards/shields/tomak79/tomak79_dongle_ssd1306_plain.zmk.yml
- ?? boards/shields/tomak79/tomak79_dongle_usbfix.conf
- ?? boards/shields/tomak79/tomak79_dongle_usbfix.overlay
- ?? boards/shields/tomak79/tomak79_dongle_usbfix.zmk.yml

#### Diff stat

 boards/shields/tomak79/Kconfig.defconfig      |  27 +-
 boards/shields/tomak79/Kconfig.shield         |  12 +
 boards/shields/tomak79/tomak79_dongle.conf    |   4 +
 boards/shields/tomak79/tomak79_dongle.overlay |   6 +-
 build.yaml                                    |  24 +
 codex.md                                      | 940 ++++++++++++++++++++++++++
 west.yml                                      |   5 +
 7 files changed, 1014 insertions(+), 4 deletions(-)

#### 다음 작업 메모

- 필요 시 이 항목을 수동으로 보완한다.
