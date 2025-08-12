add_library(radio_subghz STATIC
  src/radio_core.c
  drivers/rfm69/rfm69.c
  drivers/sx1278/sx1278.c
)
target_include_directories(radio_subghz PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(radio_subghz PUBLIC core_interfaces)
