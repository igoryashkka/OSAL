cmake_minimum_required(VERSION 3.22)
project(stm32_multi LANGUAGES C ASM)

# ---- Options ---------------------------------------------------------------
option(PLATFORM_STM32 "Build for STM32" ON)
set(STM32_FAMILY "F1" CACHE STRING "STM32 family")
set(MCU_PART "STM32F103xB" CACHE STRING "CMSIS define for part")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/platform/stm32/ld/stm32f103c8_flash.ld" CACHE FILEPATH "")

option(USE_GPIO "Enable GPIO" ON)
option(USE_UART "Enable UART" ON)
option(USE_I2C  "Enable I2C"  OFF)
option(USE_SPI  "Enable SPI"  OFF)
option(USE_FLASH "Enable FLASH" ON)
# add more peripherals as needed

# ---- Target ---------------------------------------------------------------
add_executable(app
  app/main.c
  modules/blink/blink.c
  platform/stm32/startup/gcc/startup_stm32f103xb.s
)

target_include_directories(app PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/platform/stm32/STM32F1XX/Drivers/CMSIS/Device/ST/STM32F1xx/Include
  ${CMAKE_SOURCE_DIR}/platform/stm32/STM32F1XX/Drivers/CMSIS/Include
  ${CMAKE_SOURCE_DIR}/platform/stm32/STM32F1XX/Drivers/STM32F1xx_HAL_Driver/Inc
)

# CMSIS system file
target_sources(app PRIVATE platform/stm32/common/system_stm32f1xx.c)

# PAL implementations (STM32F1)
target_sources(app PRIVATE
  platform/stm32/pal/stm32f1/pal_clock_stm32f1.c
  platform/stm32/pal/stm32f1/pal_gpio_stm32f1.c
  platform/stm32/pal/stm32f1/pal_uart_stm32f1.c
  platform/stm32/pal/stm32f1/pal_flash_stm32f1.c
)

# HAL sources (selectively add; you said you'll provide driver files)
set(HAL_DIR ${CMAKE_SOURCE_DIR}/platform/stm32/STM32F1XX/Drivers/STM32F1xx_HAL_Driver)
set(HAL_SRC
  ${HAL_DIR}/Src/stm32f1xx_hal.c
  ${HAL_DIR}/Src/stm32f1xx_hal_rcc.c
  ${HAL_DIR}/Src/stm32f1xx_hal_rcc_ex.c
  ${HAL_DIR}/Src/stm32f1xx_hal_cortex.c
)
if(USE_GPIO)  list(APPEND HAL_SRC ${HAL_DIR}/Src/stm32f1xx_hal_gpio.c)  endif()
if(USE_UART)  list(APPEND HAL_SRC ${HAL_DIR}/Src/stm32f1xx_hal_uart.c ${HAL_DIR}/Src/stm32f1xx_hal_uart_ex.c) endif()
if(USE_I2C)   list(APPEND HAL_SRC ${HAL_DIR}/Src/stm32f1xx_hal_i2c.c  ${HAL_DIR}/Src/stm32f1xx_hal_i2c_ex.c)  endif()
if(USE_SPI)   list(APPEND HAL_SRC ${HAL_DIR}/Src/stm32f1xx_hal_spi.c  ${HAL_DIR}/Src/stm32f1xx_hal_spi_ex.c)  endif()
if(USE_FLASH) list(APPEND HAL_SRC ${HAL_DIR}/Src/stm32f1xx_hal_flash.c ${HAL_DIR}/Src/stm32f1xx_hal_flash_ex.c) endif()

target_sources(app PRIVATE ${HAL_SRC})

# HAL module enable defines
target_compile_definitions(app PRIVATE
  ${MCU_PART}
  USE_HAL_DRIVER
  $<$<BOOL:${USE_GPIO}>:HAL_GPIO_MODULE_ENABLED>
  $<$<BOOL:${USE_UART}>:HAL_UART_MODULE_ENABLED>
  $<$<BOOL:${USE_I2C}>:HAL_I2C_MODULE_ENABLED>
  $<$<BOOL:${USE_SPI}>:HAL_SPI_MODULE_ENABLED>
  $<$<BOOL:${USE_FLASH}>:HAL_FLASH_MODULE_ENABLED>
)

# Flags
set(CPU "-mcpu=cortex-m3 -mthumb -mfloat-abi=soft")
target_compile_options(app PRIVATE ${CPU} -ffunction-sections -fdata-sections -Wall -Wextra)
target_link_options(app PRIVATE ${CPU} -Wl,--gc-sections -Wl,-Map=${CMAKE_BINARY_DIR}/app.map -T ${LINKER_SCRIPT})

# Optional: output .bin/.hex
add_custom_command(TARGET app POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex  $<TARGET_FILE:app> app.hex
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:app> app.bin
)
