# cmake/mcu/stm32h750.cmake
add_library(platform_mcu INTERFACE)
add_library(platform::mcu ALIAS platform_mcu)

set(STM32_ROOT   ${CMAKE_SOURCE_DIR}/platform/stm32)
set(CUBE_DRIVERS ${STM32_ROOT}/STM32H7XX/Drivers)

set(CMSIS_DIR ${CUBE_DRIVERS}/CMSIS)
set(HAL_DIR   ${CUBE_DRIVERS}/STM32H7xx_HAL_Driver)

if(NOT EXISTS "${HAL_DIR}/Src")
  message(FATAL_ERROR
    "Missing STM32CubeH7 Drivers in platform/stm32/STM32H7XX/Drivers"
  )
endif()

set(LINKER_SCRIPT "${STM32_ROOT}/ld/stm32h750vb_flash.ld")
set(MCU_PART      "STM32H750xx")

# Cortex-M7 + FPU
set(MCU_FLAGS "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard")

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
  ${CMSIS_DIR}/Device/ST/STM32H7xx/Include
  ${CMSIS_DIR}/Include
  ${HAL_DIR}/Inc
)

target_sources(platform_mcu INTERFACE
  ${STM32_ROOT}/startup/gcc/startup_stm32h750xx.s
  ${CMSIS_DIR}/Device/ST/STM32H7xx/Source/Templates/system_stm32h7xx.c
  ${STM32_ROOT}/common/stm32h7xx_it.c

  ${STM32_ROOT}/pal/stm32h7/pal_clock_stm32h7.c
  ${STM32_ROOT}/pal/stm32h7/pal_gpio_stm32h7.c
  ${STM32_ROOT}/pal/stm32h7/pal_uart_stm32h7.c

  ${HAL_DIR}/Src/stm32h7xx_hal.c
  ${HAL_DIR}/Src/stm32h7xx_hal_rcc.c
  ${HAL_DIR}/Src/stm32h7xx_hal_rcc_ex.c
  ${HAL_DIR}/Src/stm32h7xx_hal_cortex.c
  ${HAL_DIR}/Src/stm32h7xx_hal_pwr.c
  ${HAL_DIR}/Src/stm32h7xx_hal_pwr_ex.c

  $<$<BOOL:${USE_GPIO}>:${HAL_DIR}/Src/stm32h7xx_hal_gpio.c>
  $<$<BOOL:${USE_UART}>:${HAL_DIR}/Src/stm32h7xx_hal_uart.c>
  $<$<BOOL:${USE_I2C}>:${HAL_DIR}/Src/stm32h7xx_hal_i2c.c;${HAL_DIR}/Src/stm32h7xx_hal_i2c_ex.c>
  $<$<BOOL:${USE_SPI}>:${HAL_DIR}/Src/stm32h7xx_hal_spi.c;${HAL_DIR}/Src/stm32h7xx_hal_spi_ex.c>
  $<$<BOOL:${USE_FLASH}>:${HAL_DIR}/Src/stm32h7xx_hal_flash.c;${HAL_DIR}/Src/stm32h7xx_hal_flash_ex.c>
)
