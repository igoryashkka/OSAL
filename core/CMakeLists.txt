add_library(core_interfaces INTERFACE)
target_include_directories(core_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Host or STM32 specific PAL/OSAL will be added in platform/* subdirs via target_link_libraries.
