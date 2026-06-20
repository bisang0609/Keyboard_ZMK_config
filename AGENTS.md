# AGENTS.md

## 0. 최우선 지시

이 저장소에서 작업하는 Codex/AI 에이전트는 작업 시작 전에 반드시 아래 파일을 순서대로 읽는다.

1. `AGENTS.md`
2. `codex.md`

읽은 뒤 사용자에게 먼저 아래 형식으로 요약한다.

1. 이 저장소의 목적
2. 수정 금지 범위
3. 수정 가능 범위
4. 반드시 유지해야 할 핀 설정
5. 현재까지 완료된 작업
6. 마지막 빌드 결과
7. 다음 작업 후보

이 요약을 하기 전에는 파일 수정, 빌드, commit, push를 수행하지 않는다.

`git status`, `git log`, `ls`, `find`, `cat` 같은 읽기 전용 확인 명령은 가능하다.  
하지만 그 결과만 보고 작업을 시작하지 않는다. 반드시 `AGENTS.md`와 `codex.md`를 읽은 뒤 판단한다.

---

## 1. 저장소 목적

이 저장소는 ZMK 0.3.0 기반 `tomak79` keyboard/dongle config 저장소이다.

목표는 기존 tomak79 동작을 유지하면서, ZMK 0.3.0 기준으로 빌드 가능한 최소 필수 구성만 깔끔하게 관리하는 것이다.

관리 대상은 이 저장소 하나뿐이다.

- 상위 작업 폴더: `~/ZMK_Keyboard`
- Git 저장소: `~/ZMK_Keyboard/Keyboard_ZMK_config`
- 원격 저장소: `https://github.com/bisang0609/Keyboard_ZMK_config`
- ZMK 버전: `v0.3`
- Zephyr/ZMK/modules: `west update`로 받은 외부 의존성
- Git 관리 대상: `Keyboard_ZMK_config`만

---

## 2. 전체 원칙

반드시 아래 원칙을 지킨다.

1. ZMK 본체를 fork하지 않는다.
2. ZMK 본체를 직접 clone해서 작업하지 않는다.
3. `Keyboard_ZMK_config`만 Git으로 관리한다.
4. `west.yml`에서 ZMK를 `v0.3`으로 고정한다.
5. `west update`로 ZMK 본체와 외부 모듈을 의존성으로만 받는다.
6. ZMK 본체, Zephyr, modules 폴더는 직접 수정하지 않는다.
7. Git Source Control에는 `Keyboard_ZMK_config`만 표시되어야 한다.
8. 홈 PC와 회사 PC에서 동일한 절차로 같은 환경을 재현할 수 있어야 한다.
9. 빌드 성공을 기능 추가보다 우선한다.
10. ZMK Studio 관련 설정이 빌드를 깨면 제거하거나 비활성화한다.
11. 임의로 최신 ZMK/HWMv2 문법으로 변경하지 않는다.
12. 검증 없이 핀맵이나 matrix transform을 변경하지 않는다.

---

## 3. 절대 수정 금지

아래 폴더는 직접 수정하지 않는다.

- `../zmk/`
- `../zephyr/`
- `../modules/`
- `../bootloader/`

이 폴더들은 west 의존성이다.

ZMK 본체, Zephyr, modules를 수정해서 문제를 해결하지 않는다.  
필요한 수정은 반드시 `Keyboard_ZMK_config` 내부에서 처리한다.

---

## 4. 수정 가능 범위

기본적으로 아래 파일/폴더만 수정한다.

- `boards/shields/tomak79/`
- `config/`
- `build.yaml`
- `zephyr/module.yml`
- `west.yml`
- `codex.md`
- `AGENTS.md`
- `.gitignore`
- `.vscode/settings.json`

---

## 5. 삭제/포함 금지 항목

아래 항목은 저장소에 포함하지 않는다.

- `build/`
- `*.uf2`
- `*.hex`
- `*.elf`
- `*.map`
- `*.bin`
- `*.log`
- `.venv/`
- `zmk/`
- `zephyr/` 본체
- `modules/`
- `_old_zmk_config/`
- `artifacts/`
- 임시 로그
- 임시 스크립트
- ZMK 4.1 전용 실험 파일
- OLED/I2C 테스트용 실험 shield
- 기존 `agent.md`

현재는 아래 두 파일만 유지한다.

- `AGENTS.md`: Codex/AI 작업 지침
- `codex.md`: 실제 작업 이력, 현재 상태, 빌드 결과, 다음 작업

---

## 6. 반드시 유지해야 할 핀 설정

### 6.1 Keyboard-side Fuel Gauge I2C 핀 고정

`left`, `right`, 그리고 향후 추가할 `keypad`의 `MAX17048` I2C 핀은 반드시 유지한다.

- SDA = P0.06
- SCL = P0.08

보드 표기 기준:

- D1 / P0.06 = SDA
- D0 / P0.08 = SCL

즉 overlay에서는 아래 의도가 유지되어야 한다.

```dts
NRF_PSEL(TWIM_SDA, 0, 6)
NRF_PSEL(TWIM_SCL, 0, 8)
```

### 6.2 Dongle Display I2C 핀 주의

- `tomak79_dongle_ssd1306_block`는 별도 실기 검증 핀을 사용한다.
- `tomak79_dongle_yads`는 ST7789 기반이며 위 `P0.06/P0.08` keyboard-side fuel gauge 핀 규칙과 별개로 취급한다.
- 따라서 keyboard-side `MAX17048` 핀과 dongle display 핀을 혼동해서 합치지 않는다.
