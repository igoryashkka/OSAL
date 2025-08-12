add_library(logging STATIC
  src/log.c
  backends/log_backend_uart.c
  backends/log_backend_rtt.c
)
target_include_directories(logging PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(logging PUBLIC core_interfaces)
