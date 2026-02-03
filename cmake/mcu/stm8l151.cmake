# ============================================================================
# STM8L151 — SDCC toolchain
# ============================================================================

add_library(platform_mcu INTERFACE)
add_library(platform::mcu ALIAS platform_mcu)

set(STM8_ROOT ${CMAKE_SOURCE_DIR}/platform/stm8)

set(STM8_STARTUP "${STM8_ROOT}/startup/stm8l151x.asm")
set(STM8_LINKER  "${STM8_ROOT}/ld/stm8l151.lk")

if(NOT EXISTS "${STM8_STARTUP}")
  message(FATAL_ERROR "Startup file not found: ${STM8_STARTUP}")
endif()

if(NOT EXISTS "${STM8_LINKER}")
  message(FATAL_ERROR "Linker script not found: ${STM8_LINKER}")
endif()

find_program(SDASSTM8 sdasstm8)
if(NOT SDASSTM8)
  message(FATAL_ERROR "sdasstm8 not found (comes with SDCC).")
endif()

# output .rel
set(STM8_STARTUP_REL "${CMAKE_BINARY_DIR}/stm8_startup.rel")

add_custom_command(
  OUTPUT  "${STM8_STARTUP_REL}"
  COMMAND "${SDASSTM8}" -plosgff -o "${STM8_STARTUP_REL}" "${STM8_STARTUP}"
  DEPENDS "${STM8_STARTUP}"
  COMMENT "Assembling STM8 startup: ${STM8_STARTUP_REL}"
  VERBATIM
)

add_custom_target(stm8_startup ALL DEPENDS "${STM8_STARTUP_REL}")

# ---------------------------------------------------------------------------
# Compile options
# ---------------------------------------------------------------------------
target_compile_options(platform_mcu INTERFACE
  -mstm8
  --std-c99
  --no-peep
  --noinvariant
)

# ---------------------------------------------------------------------------
# Link options
# NOTE: SDCC does not accept .lk script files directly. Use SDCC linker flags
# (code/data locations and sizes) instead so the sdcc driver can produce an
# Intel HEX (.ihx) output. Keep startup.rel before other inputs.
# ---------------------------------------------------------------------------
# Derived from ${STM8_LINKER} contents: VECTORS=0x8000, HOME=0x8080, CODE=0x8080
# CODE size=0x7F80, DATA size=0x0800
target_link_options(platform_mcu INTERFACE
  -mstm8
  "${STM8_STARTUP_REL}"
  --code-loc
  0x8080
  --code-size
  0x7F80
  --data-loc
  0x0000
  --out-fmt-ihx
)

# Platform sources
target_sources(platform_mcu INTERFACE
  "${STM8_ROOT}/impl/stm8l151/platform_clock_stm8l151.c"
  "${STM8_ROOT}/impl/stm8l151/platform_gpio_stm8l151.c"
)

target_include_directories(platform_mcu INTERFACE
  "${STM8_ROOT}"
  "${STM8_ROOT}/STM8L15x_StdPeriph_Driver/inc"
  "${CMAKE_SOURCE_DIR}/Platform_API"
)

# make sure startup.rel is ready before linking
add_dependencies(platform_mcu stm8_startup)
