# agent.md

## 작업 원칙

이 저장소는 ZMK 0.3.0 기반 tomak79 keyboard/dongle config 저장소이다.

ZMK 본체는 수정하지 않는다.
Zephyr 본체는 수정하지 않는다.
modules 폴더는 수정하지 않는다.

수정 가능한 범위는 기본적으로 아래로 제한한다.

- `boards/shields/tomak79/`
- `config/`
- `build.yaml`
- `zephyr/module.yml`
- `west.yml`
- `codex.md`
- `agent.md`
- `.gitignore`
- `.vscode/settings.json`

## 금지 사항

- `zmk/` 내부 직접 수정 금지
- `zephyr/` 내부 직접 수정 금지
- `modules/` 내부 직접 수정 금지
- ZMK main 기준 문법으로 무단 변경 금지
- ZMK 4.1/HWMv2 구조로 무단 변경 금지
- 검증 없이 핀맵 변경 금지
- 검증 없이 matrix transform 변경 금지
- 기존 저장소 전체 복사 금지
- 실험 파일 무단 포함 금지

## 우선순위

1. ZMK 0.3.0에서 빌드 성공
2. 기존 tomak79 동작 유지
3. left/right matrix 핀맵 유지
4. common physical layout / matrix transform 유지
5. dongle I2C 핀 유지
6. pro_micro_serial 비활성화
7. 불필요한 실험 파일 제거
8. 최소 구성 유지

## 중요 핀 설정

Dongle I2C:

- D1 / P0.06 = SDA
- D0 / P0.08 = SCL

즉:

- SDA = P0.06
- SCL = P0.08

`pro_micro_serial`은 비활성화한다.

## 작업 방식

작업 전에는 현재 상태를 확인한다.

작업 후에는 다음을 수행한다.

1. 변경 파일 확인
2. 빌드 가능 여부 확인
3. `codex.md`에 작업 내용 기록
4. 의미 있는 단위로 commit

## 빌드 기준

기본 빌드 대상:

- `tomak79_left`
- `tomak79_right`
- `tomak79_dongle`

빌드 명령은 ZMK 0.3.0 기준을 우선한다.

## 판단 기준

빌드가 깨지는 경우 기능 추가보다 빌드 성공을 우선한다.

ZMK Studio 관련 설정이 ZMK 0.3.0에서 문제를 만들면 제거하거나 주석 처리한다.

목표는 최신 기능 유지가 아니라, ZMK 0.3.0에서 동작 가능한 최소 필수 구성이다.
