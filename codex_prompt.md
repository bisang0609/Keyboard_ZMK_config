# Codex Start Prompt

AGENTS.md와 codex.md를 먼저 읽고 현재 작업 상태를 파악해 주세요.

파일을 읽기 전에는 수정, 빌드, commit, push를 하지 마세요.

작업 시 주의사항:

- ZMK 본체, Zephyr, modules는 수정하지 마세요.
- Keyboard_ZMK_config 내부만 수정하세요.
- pro_micro_serial은 비활성화 상태를 유지하세요.
- 작업 후 변경사항과 빌드 결과는 codex.md에 기록하세요.

 <!--
    cd ~/ZMK_Keyboard
    source .venv/bin/activate

    export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
    export ZEPHYR_SDK_INSTALL_DIR=$HOME/zephyr-sdk-0.17.0

    west zephyr-export
  -->