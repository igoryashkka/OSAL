# Skeleton target for STM32 platforms. Fill in HAL/LL files and startup/linker per MCU.
add_library(pal_stm32 STATIC
  common/pal_uart_stm32.c
  common/pal_gpio_stm32.c
  common/pal_spi_stm32.c
  common/pal_i2c_stm32.c
  common/pal_time_stm32.c
  common/osal_baremetal.c
  common/bsp_clock.c
)
target_include_directories(pal_stm32 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(pal_stm32 PUBLIC core_interfaces)

# Example family switch (extend per family)
if(ST_FAMILY STREQUAL "F4")
  add_compile_definitions(STM32F4xx)
elseif(ST_FAMILY STREQUAL "F7")
  add_compile_definitions(STM32F7xx)
elseif(ST_FAMILY STREQUAL "H7")
  add_compile_definitions(STM32H7xx)
elseif(ST_FAMILY STREQUAL "F1")
  add_compile_definitions(STM32F1xx)
elseif(ST_FAMILY STREQUAL "G0")
  add_compile_definitions(STM32G0xx)
endif()

# Link pal_stm32 into apps or modules as needed in your top-level app CMake.
