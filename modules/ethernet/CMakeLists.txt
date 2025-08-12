add_library(ethernet STATIC
  src/eth_core.c
  drivers/w5500/w5500.c
  drivers/enc28j60/enc28j60.c
)
target_include_directories(ethernet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ethernet PUBLIC core_interfaces)
