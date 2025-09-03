# cmake/toolchains/arm-gcc.cmake
# Tell CMake this is a bare-metal cross build, not macOS.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Avoid try-compile using host rules
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Use ARM GCC
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy CACHE FILEPATH "")

# Nuke macOS-specific flags CMake might add
set(CMAKE_OSX_ARCHITECTURES "" CACHE STRING "" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "" CACHE STRING "" FORCE)
set(CMAKE_OSX_SYSROOT "" CACHE PATH "" FORCE)

# CPU flags for STM32F103 (Cortex-M3)
set(MCU_FLAGS "-mcpu=cortex-m3 -mthumb -mfloat-abi=soft")

set(CMAKE_C_FLAGS_INIT "${MCU_FLAGS} -ffunction-sections -fdata-sections -fno-builtin -fno-common")
set(CMAKE_ASM_FLAGS_INIT "${MCU_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${MCU_FLAGS} -Wl,--gc-sections")
