# Minimal GCC ARM toolchain file. Adjust paths for your setup.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Try default arm-none-eabi-*
find_program(ARM_CC arm-none-eabi-gcc)
if(NOT ARM_CC)
  message(FATAL_ERROR "arm-none-eabi-gcc not found in PATH")
endif()

set(CMAKE_C_COMPILER ${ARM_CC})
get_filename_component(ARM_PREFIX ${ARM_CC} DIRECTORY)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${ARM_PREFIX}/arm-none-eabi-g++)
set(CMAKE_AR ${ARM_PREFIX}/arm-none-eabi-ar)
set(CMAKE_OBJCOPY ${ARM_PREFIX}/arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP ${ARM_PREFIX}/arm-none-eabi-objdump)
set(CMAKE_SIZE ${ARM_PREFIX}/arm-none-eabi-size)

# Generic flags (tune per family/board in platform subdirs)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
