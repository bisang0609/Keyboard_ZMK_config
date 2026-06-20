# Codex Start Prompt

AGENTS.md와 codex.md를 먼저 읽고 현재 작업 상태를 파악해 주세요.

읽은 뒤 아래 형식으로 요약해 주세요.

파일을 읽기 전에는 수정, 빌드, commit, push를 하지 마세요.

작업 시 주의사항:

- ZMK 본체, Zephyr, modules는 수정하지 마세요.
- Keyboard_ZMK_config 내부만 수정하세요.
- Dongle I2C 핀은 반드시 유지하세요.
  - SDA = P0.06
  - SCL = P0.08
- pro_micro_serial은 비활성화 상태를 유지하세요.
- 빌드는 사용자가 요청할 때만 실행하세요.
- 작업 후 변경사항과 빌드 결과는 codex.md에 기록하세요.

<!--
  cd ~/ZMK_Keyboard/
  source .venv/bin/activate
  . .venv/bin/activate
  -->