# AGENTS.md

## 필수 시작 절차

이 저장소에서 작업을 시작하는 Codex/AI 에이전트는 반드시 아래 파일을 먼저 읽는다.

1. `agent.md`
2. `codex.md`

`agent.md`는 작업 원칙과 금지 사항이다.
`codex.md`는 실제 작업 이력, 현재 상태, 빌드 결과, 다음 작업 기록이다.

두 파일을 읽지 않고 수정 작업을 시작하지 않는다.

## 저장소 목적

이 저장소는 ZMK 0.3.0 기반 `tomak79` keyboard/dongle config 저장소이다.

관리 대상은 이 저장소 하나뿐이다.

- Git 저장소: `Keyboard_ZMK_config`
- 상위 작업 폴더: `~/ZMK_Keyboard`
- ZMK 버전: `v0.3`
- Zephyr: west manifest로 받은 의존성
- ZMK 본체: west manifest로 받은 의존성

## 절대 수정 금지

아래 폴더는 직접 수정하지 않는다.

- `../zmk/`
- `../zephyr/`
- `../modules/`
- `../bootloader/`

필요한 수정은 반드시 `Keyboard_ZMK_config` 안에서 처리한다.

## 수정 가능 범위

기본적으로 아래 파일/폴더만 수정한다.

- `boards/shields/tomak79/`
- `config/`
- `build.yaml`
- `zephyr/module.yml`
- `west.yml`
- `codex.md`
- `agent.md`
- `AGENTS.md`
- `.gitignore`
- `.vscode/settings.json`

## 유지해야 할 핵심 조건

- ZMK 본체를 fork하지 않는다.
- ZMK 본체를 직접 clone해서 작업하지 않는다.
- `Keyboard_ZMK_config`만 Git으로 관리한다.
- ZMK는 `west.yml`에서 `v0.3`으로 고정한다.
- `west update`로 외부 의존성을 받는다.
- left/right matrix 핀맵을 임의 변경하지 않는다.
- matrix transform을 검증 없이 변경하지 않는다.
- dongle I2C 핀을 유지한다.
  - SDA = P0.06
  - SCL = P0.08
- `pro_micro_serial`은 비활성화한다.
- ZMK Studio 관련 설정이 ZMK 0.3 빌드를 깨면 제거하거나 비활성화한다.
- 빌드 성공을 기능 추가보다 우선한다.

## 기본 빌드 명령

작업 폴더는 반드시 상위 폴더에서 시작한다.

```bash
cd ~/ZMK_Keyboard
source .venv/bin/activate

export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
export ZEPHYR_SDK_INSTALL_DIR=$HOME/zephyr-sdk-0.17.0
