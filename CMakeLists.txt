cmake_minimum_required(VERSION 3.22)

# Avoid linking during try-compile (no syscalls/stdlib for bare-metal)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(stm32_multi LANGUAGES C ASM)

# ---- Options ---------------------------------------------------------------
option(PLATFORM_STM32 "Build for STM32" ON)
set(STM32_FAMILY "F1" CACHE STRING "STM32 family")
set(MCU_PART "STM32F103xB" CACHE STRING "CMSIS define for part")

# Linker script (adjust if you use a different device)
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/platform/stm32/ld/stm32f103c8_flash.ld" CACHE FILEPATH "")


option(USE_GPIO  "Enable GPIO"  ON)
option(USE_UART  "Enable UART"  ON)
option(USE_I2C   "Enable I2C"   OFF)
option(USE_SPI   "Enable SPI"   OFF)
option(USE_FLASH "Enable FLASH" ON)

# ---- Target ---------------------------------------------------------------

function(add_stm32_model_target TARGET_NAME FAMILY MODEL)
  set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/platform/stm32/${FAMILY}/linker/linker.ld")

  set(HAL_DIR ${CMAKE_SOURCE_DIR}/platform/stm32/${FAMILY}/drivers/HAL_Driver)
  add_executable(${TARGET_NAME}
    app/main.c
    platform/stm32/${FAMILY}/startup/startup.s
    modules/blink/module_blink.c
    modules/system/module_system.c
    platform/stm32/${FAMILY}/pal/pal_clock.c
    platform/stm32/${FAMILY}/pal/pal_flash.c
    platform/stm32/${FAMILY}/pal/pal_gpio.c
    platform/stm32/${FAMILY}/pal/pal_uart.c
    
  )

  target_compile_definitions(${TARGET_NAME} PRIVATE
        USE_HAL_DRIVER
        ${MCU_PART}
        STM32_MODEL=${FAMILY}
    )
  
  target_include_directories(${TARGET_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/platform/stm32/${FAMILY}/include
  ${CMAKE_SOURCE_DIR}/platform/stm32/${FAMILY}/drivers
  ${CMAKE_SOURCE_DIR}/platform/stm32/${FAMILY}/drivers/CMSIS/Device/ST/${FAMILY}/Include
  ${CMAKE_SOURCE_DIR}/platform/stm32/${FAMILY}/drivers/CMSIS/Include
  ${CMAKE_SOURCE_DIR}/platform/stm32/${FAMILY}/drivers/HAL_Driver/Inc
  )

  if(FAMILY STREQUAL "f1xx")
        set(CPU_FLAGS -mcpu=cortex-m3 -mthumb -mfloat-abi=soft)
    elseif(FAMILY STREQUAL "g0xx")
        set(CPU_FLAGS -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft)
    else()
        message(FATAL_ERROR "Unknown STM32 FAMILY: ${FAMILY}")
  endif()

  set(HAL_SRC
  ${HAL_DIR}/Src/stm32${FAMILY}_hal.c
  ${HAL_DIR}/Src/stm32${FAMILY}_hal_rcc.c
  ${HAL_DIR}/Src/stm32${FAMILY}_hal_rcc_ex.c
  ${HAL_DIR}/Src/stm32${FAMILY}_hal_cortex.c
  )

  if(USE_GPIO)
  list(APPEND HAL_SRC ${HAL_DIR}/Src/stm32${FAMILY}_hal_gpio.c)
  endif()

  if(USE_UART)
  list(APPEND HAL_SRC ${HAL_DIR}/Src/stm32${FAMILY}_hal_uart.c)
  endif()

  if(USE_I2C)
  list(APPEND HAL_SRC ${HAL_DIR}/Src/stm32${FAMILY}_hal_i2c.c ${HAL_DIR}/Src/stm32${FAMILY}_hal_i2c_ex.c)
  endif()

  if(USE_SPI)
  list(APPEND HAL_SRC ${HAL_DIR}/Src/stm32${FAMILY}_hal_spi.c ${HAL_DIR}/Src/stm32${FAMILY}_hal_spi_ex.c)
  endif()

  if(USE_FLASH)
  list(APPEND HAL_SRC ${HAL_DIR}/Src/stm32${FAMILY}_hal_flash.c ${HAL_DIR}/Src/stm32${FAMILY}_hal_flash_ex.c)
  endif()
  if(NOT FAMILY STREQUAL "g0xx")
    target_sources(${TARGET_NAME} PRIVATE
      platform/stm32/${FAMILY}/drivers/CMSIS/Device/ST/${FAMILY}/Source/Templates/system_stm32${FAMILY}.c
    )
  endif()



  target_sources(${TARGET_NAME} PRIVATE ${HAL_SRC})

  
  target_compile_options(${TARGET_NAME} PRIVATE
  ${CPU_FLAGS}
  -ffunction-sections -fdata-sections
  -Wall -Wextra -Werror=implicit-function-declaration
  ) 

  target_link_options(${TARGET_NAME} PRIVATE
  ${CPU_FLAGS}
  -Wl,--gc-sections
  -Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET_NAME}.map
  -T ${LINKER_SCRIPT}
  -specs=nosys.specs
  -Wl,--start-group -lc -lm -lgcc -Wl,--end-group
  )

  
  find_program(OBJCOPY arm-none-eabi-objcopy)
  find_program(SIZETOOL arm-none-eabi-size)

  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${OBJCOPY} -O ihex  $<TARGET_FILE:${TARGET_NAME}> ${TARGET_NAME}.hex
      COMMAND ${OBJCOPY} -O binary $<TARGET_FILE:${TARGET_NAME}> ${TARGET_NAME}.bin
      COMMAND ${SIZETOOL} --format=berkeley $<TARGET_FILE:${TARGET_NAME}>
  )

endfunction()

add_stm32_model_target(app_f1xx f1xx STM32F103xx)
add_stm32_model_target(app_g0xx g0xx STM32G030xx)