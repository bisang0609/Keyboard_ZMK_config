if(CONFIG_SHIELD_TOMAK79_DONGLE_SSD1306_BLOCK)
  zephyr_library_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../zmk/app/include)
  zephyr_library_sources(tomak79_dongle_ssd1306_block.c)
endif()
