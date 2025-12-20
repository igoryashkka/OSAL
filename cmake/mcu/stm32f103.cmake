# cmake/mcu/stm32f103.cmake
add_library(platform_mcu INTERFACE)
add_library(platform::mcu ALIAS platform_mcu)

set(STM32_ROOT   ${CMAKE_SOURCE_DIR}/platform/stm32)
set(CUBE_DRIVERS ${STM32_ROOT}/STM32F1XX/Drivers)

set(CMSIS_DIR ${CUBE_DRIVERS}/CMSIS)
set(HAL_DIR   ${CUBE_DRIVERS}/STM32F1xx_HAL_Driver)

if(NOT EXISTS "${HAL_DIR}/Src")
  message(FATAL_ERROR
    "Missing STM32CubeF1 Drivers in platform/stm32/STM32F1XX/Drivers"
  )
endif()

set(LINKER_SCRIPT "${STM32_ROOT}/ld/stm32f103c8_flash.ld")
set(MCU_PART      "STM32F103xB")

# Cortex-M3
set(MCU_FLAGS
  -mcpu=cortex-m3
  -mthumb
  -mfloat-abi=soft
)

target_compile_options(platform_mcu INTERFACE
  ${MCU_FLAGS}
  -ffunction-sections -fdata-sections
  -Wall -Wextra -Werror=implicit-function-declaration
)

target_link_options(platform_mcu INTERFACE
  ${MCU_FLAGS}
  -Wl,--gc-sections
  -Wl,-Map=${CMAKE_BINARY_DIR}/app.map
  -T ${LINKER_SCRIPT}
  -specs=nosys.specs
  -Wl,--start-group -lc -lm -lgcc -Wl,--end-group
)

target_compile_definitions(platform_mcu INTERFACE
  USE_HAL_DRIVER
  ${MCU_PART}
  $<$<BOOL:${USE_GPIO}>:HAL_GPIO_MODULE_ENABLED>
  $<$<BOOL:${USE_UART}>:HAL_UART_MODULE_ENABLED>
  $<$<BOOL:${USE_I2C}>:HAL_I2C_MODULE_ENABLED>
  $<$<BOOL:${USE_SPI}>:HAL_SPI_MODULE_ENABLED>
  $<$<BOOL:${USE_FLASH}>:HAL_FLASH_MODULE_ENABLED>
)

target_include_directories(platform_mcu INTERFACE
  ${STM32_ROOT}
  ${CMSIS_DIR}/Device/ST/STM32F1xx/Include
  ${CMSIS_DIR}/Include
  ${HAL_DIR}/Inc
)

# ---- Sources: startup + system + it + PAL + HAL ----
target_sources(platform_mcu INTERFACE
  ${STM32_ROOT}/startup/gcc/startup_stm32f103xb.s
  ${CMSIS_DIR}/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c
  ${STM32_ROOT}/common/stm32f1xx_it.c

  ${STM32_ROOT}/pal/stm32f1/pal_clock_stm32f1.c
  ${STM32_ROOT}/pal/stm32f1/pal_gpio_stm32f1.c
  ${STM32_ROOT}/pal/stm32f1/pal_uart_stm32f1.c

  ${HAL_DIR}/Src/stm32f1xx_hal.c
  ${HAL_DIR}/Src/stm32f1xx_hal_rcc.c
  ${HAL_DIR}/Src/stm32f1xx_hal_rcc_ex.c
  ${HAL_DIR}/Src/stm32f1xx_hal_cortex.c

  $<$<BOOL:${USE_GPIO}>:${HAL_DIR}/Src/stm32f1xx_hal_gpio.c>
  $<$<BOOL:${USE_UART}>:${HAL_DIR}/Src/stm32f1xx_hal_uart.c>
  $<$<BOOL:${USE_I2C}>:${HAL_DIR}/Src/stm32f1xx_hal_i2c.c;${HAL_DIR}/Src/stm32f1xx_hal_i2c_ex.c>
  $<$<BOOL:${USE_SPI}>:${HAL_DIR}/Src/stm32f1xx_hal_spi.c;${HAL_DIR}/Src/stm32f1xx_hal_spi_ex.c>
  $<$<BOOL:${USE_FLASH}>:${HAL_DIR}/Src/stm32f1xx_hal_flash.c;${HAL_DIR}/Src/stm32f1xx_hal_flash_ex.c>
)
