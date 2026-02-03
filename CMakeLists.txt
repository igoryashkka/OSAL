cmake_minimum_required(VERSION 3.22)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(osal LANGUAGES C)

set(MCU "" CACHE STRING "Target MCU: stm32f103 | stm32h750 | stm8l151")

if(MCU STREQUAL "")
  message(FATAL_ERROR
    "MCU is not set.\n"
    "Example:\n"
    "  ./tools/build.sh stm8l151\n"
  )
endif()

string(TOLOWER "${MCU}" MCU_LOWER)
set(MCU_CMAKE "${CMAKE_SOURCE_DIR}/cmake/mcu/${MCU_LOWER}.cmake")

if(NOT EXISTS "${MCU_CMAKE}")
  message(FATAL_ERROR
    "Unknown MCU '${MCU}'.\n"
    "Expected: ${MCU_CMAKE}"
  )
endif()

include(${MCU_CMAKE})  # defines platform::mcu

# ---------------------------------------------------------------------------
# Platform API
# ---------------------------------------------------------------------------
if(MCU_LOWER STREQUAL "stm8l151")
  add_library(platform_api INTERFACE)
  target_include_directories(platform_api INTERFACE ${CMAKE_SOURCE_DIR}/Platform_API)
  target_compile_definitions(platform_api INTERFACE BUILD_FOR_STM8=1 STM8L15X_MD=1)
else()
  add_library(platform_api STATIC
    Platform_API/Platform_GPIO/Platform_gpio.c
    Platform_API/Platform_Clock/Platform_clock.c
    Platform_API/Platform_UART/Platform_uart.c
    Platform_API/Platform_Flash/Platform_flash.c
  )
  target_include_directories(platform_api PUBLIC ${CMAKE_SOURCE_DIR}/Platform_API)
endif()

# ---------------------------------------------------------------------------
# Application
# ---------------------------------------------------------------------------
add_executable(app
  app/main.c
  app/init/application.c
  app/tasks/tasks_peripheral.c
  app/tasks/tasks_comms.c
  app/tasks/tasks_stats.c

  modules/blink/module_blink.c
  modules/system/module_system.c
  modules/scheduler/scheduler.c
)

target_include_directories(app PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/app
  ${CMAKE_SOURCE_DIR}/app/init
  ${CMAKE_SOURCE_DIR}/app/tasks
  ${CMAKE_SOURCE_DIR}/app/config
  ${CMAKE_SOURCE_DIR}/modules/blink
  ${CMAKE_SOURCE_DIR}/modules/system
  ${CMAKE_SOURCE_DIR}/modules/scheduler
  ${CMAKE_SOURCE_DIR}/modules/gps.nmea
)

target_link_libraries(app PRIVATE
  platform_api
  platform::mcu
)

# STM8: add small GPS stub implementations to satisfy SDCC linking when
# the real GPS module is disabled (compiler incompatibility / crash)
if(MCU_LOWER STREQUAL "stm8l151")
  target_sources(app PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/gps.nmea/gps_nmea_stm8.c
  )
endif()

# Run sdsize after build if available (optional)
find_program(SD_SIZE sdsize)
if(SD_SIZE)
  add_custom_command(TARGET app POST_BUILD
    COMMAND ${SD_SIZE} $<TARGET_FILE:app>
  )
endif()
