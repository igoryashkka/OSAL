add_library(pal_host STATIC pal_uart_host.c pal_gpio_host.c pal_spi_host.c pal_i2c_host.c pal_time_host.c)
target_include_directories(pal_host PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(pal_host PUBLIC core_interfaces)

add_library(osal_host STATIC osal_posix.c)
target_include_directories(osal_host PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(osal_host PUBLIC core_interfaces)
