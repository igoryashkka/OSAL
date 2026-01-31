cmake_minimum_required(VERSION 3.22)

# Avoid linking during try-compile (bare-metal)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(stm32_multi LANGUAGES C ASM)

# ---- User options ----------------------------------------------------------
set(MCU "" CACHE STRING "Target MCU: stm32f103, stm32h750, or stm8l151")

option(USE_GPIO  "Enable GPIO"  ON)
option(USE_UART  "Enable UART"  ON)
option(USE_I2C   "Enable I2C"   OFF)
option(USE_SPI   "Enable SPI"   OFF)
option(USE_FLASH "Enable FLASH" ON)

if(MCU STREQUAL "")
  message(FATAL_ERROR
    "MCU is not set.\n"
    "Use: cmake -S . -B build -DMCU=stm32h750 (or stm32f103/stm8l151)\n"
  )
endif()

# ---- Load MCU config -------------------------------------------------------
string(TOLOWER "${MCU}" MCU_LOWER)
set(MCU_CMAKE "${CMAKE_SOURCE_DIR}/cmake/mcu/${MCU_LOWER}.cmake")

if(NOT EXISTS "${MCU_CMAKE}")
  message(FATAL_ERROR
    "Unknown MCU='${MCU}'. Expected cmake/mcu/${MCU_LOWER}.cmake\n"
    "Supported: stm32f103, stm32h750, stm8l151"
  )
endif()

include("${MCU_CMAKE}")  # must define platform::mcu (INTERFACE library)

# ---- Platform_API (weak default implementations) ------------------------
add_library(platform_api STATIC
  Platform_API/Platform_GPIO/Platform_gpio.c
  Platform_API/Platform_Clock/Platform_clock.c
  Platform_API/Platform_UART/Platform_uart.c
  Platform_API/Platform_Flash/Platform_flash.c
)
target_include_directories(platform_api PUBLIC ${CMAKE_SOURCE_DIR}/Platform_API)


# ---- App target (including all modules and tasks) ----------------------------
add_executable(app
  app/main.c
  app/init/application.c
  app/tasks/tasks_peripheral.c
  app/tasks/tasks_comms.c
  app/tasks/tasks_stats.c
  modules/blink/module_blink.c
  modules/system/module_system.c
  modules/scheduler/scheduler.c
  modules/gps.nmea/gps_nmea.c
)

target_include_directories(app PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/app
  ${CMAKE_SOURCE_DIR}/app/init
  ${CMAKE_SOURCE_DIR}/app/tasks
  ${CMAKE_SOURCE_DIR}/app/config
  ${CMAKE_SOURCE_DIR}/Platform_API
  ${CMAKE_SOURCE_DIR}/Platform_API/Platform_GPIO
  ${CMAKE_SOURCE_DIR}/Platform_API/Platform_UART
  ${CMAKE_SOURCE_DIR}/Platform_API/Platform_Clock
  ${CMAKE_SOURCE_DIR}/Platform_API/Platform_Flash
  ${CMAKE_SOURCE_DIR}/modules/blink
  ${CMAKE_SOURCE_DIR}/modules/system
  ${CMAKE_SOURCE_DIR}/modules/scheduler
  ${CMAKE_SOURCE_DIR}/modules/gps.nmea
  ${CMAKE_SOURCE_DIR}/modules/flash
)

# Link to the selected MCU platform (startup + HAL + flags + ld, all there)
target_link_libraries(app PRIVATE platform_api platform::mcu)

# ---- HEX/BIN/SIZE ----------------------------------------------------------
add_custom_command(TARGET app POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex  $<TARGET_FILE:app> app.hex
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:app> app.bin
  COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:app>
)
