# STM32 HAL + CMake skeleton (STM32F103C8)

This is a minimal, vendor-HAL-based project you can build with CMake + Ninja and `arm-none-eabi-gcc`.

## Layout
```
stm32_multi_skeleton/
├─ CMakeLists.txt
├─ cmake/toolchains/arm-gcc.cmake
├─ app/main.c
├─ modules/blink/blink.c
├─ modules/blink/blink.h
├─ include/pal/{pal_gpio.h,pal_uart.h,pal_flash.h,pal_clock.h}
├─ include/config/board.h
├─ include/stm32f1xx_hal_conf.h
├─ platform/stm32/common/system_stm32f1xx.c
├─ platform/stm32/startup/gcc/startup_stm32f103xb.s
├─ platform/stm32/ld/stm32f103c8_flash.ld
└─ platform/stm32/STM32F1XX/Drivers/   <-- put STM32CubeF1 Drivers here
```

## Prereqs
- CMake ≥ 3.22
- Ninja
- ARM GCC toolchain in PATH (e.g. `arm-none-eabi-gcc`)
- **STM32CubeF1** Drivers copied to `platform/stm32/STM32F1XX/Drivers/`

Expected subfolders you must provide from the ST package:
- `CMSIS/Include`
- `CMSIS/Device/ST/STM32F1xx/Include`
- `STM32F1xx_HAL_Driver/Inc`
- `STM32F1xx_HAL_Driver/Src`

## Configure & build (PowerShell)
```powershell
$tc = "$PWD\cmake\toolchains\arm-gcc.cmake"
Remove-Item -Recurse -Force build -ErrorAction Ignore
cmake -S . -B build -G "Ninja" `
  -DCMAKE_TOOLCHAIN_FILE="$tc" `
  -DMCU_PART=STM32F103xB `
  -DLINKER_SCRIPT="$PWD\platform\stm32\ld\stm32f103c8_flash.ld" `
  -DUSE_GPIO=ON -DUSE_UART=ON -DUSE_I2C=OFF -DUSE_SPI=OFF -DUSE_FLASH=ON `
  -DCMAKE_OBJCOPY=arm-none-eabi-objcopy

cmake --build build -v
```

## Configure & build (MacOS)

rm -rf build
cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake
cmake --build build -v


Artifacts: `build/app.elf`, `build/app.hex`, `build/app.bin`, and `build/app.map`.

## Notes
- Default LED: **PC13** (BluePill). Change `include/config/board.h` for your board.
- UART default is USART1 on PA9/PA10 at 115200.
- `system_stm32f1xx.c` is a minimal stub; for production use ST's official file.
