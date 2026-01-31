# cmake/mcu/stm8l151.cmake
add_library(platform_mcu INTERFACE)
add_library(platform::mcu ALIAS platform_mcu)

set(STM8_ROOT ${CMAKE_SOURCE_DIR}/platform/stm8)

set(STM8_STARTUP "${STM8_ROOT}/startup/stm8l151x.s")
set(STM8_LINKER  "${STM8_ROOT}/ld/stm8l151.lk")

if(NOT EXISTS "${STM8_STARTUP}" OR NOT EXISTS "${STM8_LINKER}")
  message(FATAL_ERROR
    "STM8L151 platform files are missing.\n"
    "Expected:\n"
    "  ${STM8_STARTUP}\n"
    "  ${STM8_LINKER}\n"
    "Add STM8L151 startup/linker files under platform/stm8/ to enable builds."
  )
endif()

target_compile_options(platform_mcu INTERFACE
  -mstm8
  --std-c99
  -Wall
  -Wextra
)

target_link_options(platform_mcu INTERFACE
  -mstm8
  -Wl,-M
  -Wl,--out-fmt-elf
  -Wl,-b
  ${STM8_LINKER}
)

target_sources(platform_mcu INTERFACE
  ${STM8_STARTUP}
)

target_include_directories(platform_mcu INTERFACE
  ${STM8_ROOT}
  ${CMAKE_SOURCE_DIR}/Platform_API
)
