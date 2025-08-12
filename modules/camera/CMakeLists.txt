add_library(camera STATIC
  src/camera_core.c
  drivers/ov7670/ov7670.c
  drivers/ov2640/ov2640.c
)
target_include_directories(camera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(camera PUBLIC core_interfaces)
