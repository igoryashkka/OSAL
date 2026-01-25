# OSAL - Operating System Abstraction Layer for STM32

Multi-MCU embedded project with modular Platform API abstraction layer. Supports STM32F103 and STM32H750 with CMake + Ninja + arm-none-eabi-gcc.

## Project Layout
```
OSAL/
├─ CMakeLists.txt
├─ app/main.c                                    # Application entry point
├─ modules/                                      # Functional modules
│  ├─ blink/module_blink.{c,h}
│  ├─ system/module_system.{c,h}
│  └─ flash/nvm_*.{c,h}                          # Non-volatile memory abstraction
├─ Platform_API/                                 # HAL abstraction layer (weak defaults)
│  ├─ Platform_Clock/Platform_clock.{c,h}
│  ├─ Platform_GPIO/Platform_gpio.{c,h}
│  ├─ Platform_UART/platform_uart.{c,h}
│  └─ Platform_Flash/Platform_flash.{c,h}
├─ platform/stm32/                              # MCU-specific implementations
│  ├─ common/                                    # Shared code (syscalls, startup)
│  ├─ impl/stm32f1/ & impl/stm32h7/             # MCU-specific drivers
│  ├─ ld/stm32f103c8_flash.ld                   # Linker scripts
│  ├─ startup/gcc/                              # Startup assembly
│  └─ STM32F1XX/Drivers/                         # ST HAL (provided separately)
└─ cmake/
   ├─ mcu/stm32f103.cmake & stm32h750.cmake     # MCU configurations
   └─ toolchains/arm-gcc.cmake                  # Cross-compile toolchain
```

## Prerequisites
- CMake ≥ 3.22
- Ninja build system
- ARM GCC toolchain (`arm-none-eabi-gcc`, `arm-none-eabi-objcopy`, `arm-none-eabi-size`)
- STM32 HAL Drivers from ST CubeMX (for supported MCU)

## Build Configuration

MCU selection via CMake cache variable:
- `stm32f103` - STM32F103C8 (Blue Pill, default)
- `stm32h750` - STM32H750VB (high-performance)

Feature flags (all enabled by default):
- `USE_GPIO`, `USE_UART`, `USE_FLASH`, `USE_I2C`, `USE_SPI`

## Building (macOS/Linux)

```bash
# For STM32F103
rm -rf build_stm32f103
cmake -S . -B build_stm32f103 -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
  -DMCU=stm32f103
cmake --build build_stm32f103 -v

# For STM32H750
rm -rf build_stm32h750
cmake -S . -B build_stm32h750 -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
  -DMCU=stm32h750
cmake --build build_stm32h750 -v
```

Build artifacts: `app.elf`, `app.hex`, `app.bin`, `app.map`

## Quick Build (macOS)

```bash
./tools/build.sh          # Build for STM32F103
./tools/clean.sh          # Clean build directories
```

## Architecture Notes

- **Platform_API**: Weak default implementations allow MCU-specific overrides
- **Flash Module (NVM)**: Abstracted non-volatile memory with CRC32 support
- **System Module**: Clock and initialization management per-MCU
- **Blink Module**: LED control with GPIO abstraction

## Configuration Files

- Board-specific settings: Define in your application or cmake/mcu/*.cmake
- STM32F1XX HAL config: Provide `stm32f1xx_hal_conf.h` in `include/`
- Linker script: `platform/stm32/ld/stm32f103c8_flash.ld` (customizable)
