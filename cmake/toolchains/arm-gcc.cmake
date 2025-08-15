# Bare-metal ARM GCC toolchain for STM32
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Do not try link in try-compile
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Tool prefix
set(TOOLCHAIN_PREFIX arm-none-eabi-)

# Compilers/binutils
find_program(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc   REQUIRED)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
find_program(CMAKE_AR           ${TOOLCHAIN_PREFIX}ar     REQUIRED)
find_program(CMAKE_RANLIB       ${TOOLCHAIN_PREFIX}ranlib REQUIRED)
find_program(CMAKE_OBJCOPY      ${TOOLCHAIN_PREFIX}objcopy REQUIRED)
find_program(CMAKE_SIZE         ${TOOLCHAIN_PREFIX}size     REQUIRED)

# Keep CMake from searching host headers/libs
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
