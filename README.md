# STM32 Multi-Platform Modular Skeleton (CMake)

**Goals**
- Maximal code reuse across STM32 families (F1, F4, F7, H7, G0) and host.
- Clear separation: **App** → **Modules** → **OSAL/PAL** → **Platform BSP**.
- Pluggable drivers per module (e.g., cameras OV7670/OV2640; radios RFM69/SX1278; Ethernet W5500/ENC28J60).
- Buildable on **host** (for unit tests) and on **STM32** (for firmware).

**Layers**
```
App (business logic)
  └─ Modules: logging, camera, radio_subghz, ethernet
       └─ PAL (Platform Abstraction Layer): GPIO/UART/SPI/I2C/TIME
            └─ OSAL (threads/mutex/timer) [FreeRTOS or bare-metal]
                 └─ Platform BSP (MCU/board, HAL/LL, pins/clocks)
```
**Quick start (host build)**
```bash
cmake -S . -B build -DPLATFORM=host -DMODULE_LOGGING_BACKEND=uart
cmake --build build -j
./build/app/host_demo
```

**STM32 build (example F4)**
```bash
cmake -S . -B build-stm -DPLATFORM=stm32 -DST_FAMILY=F4 -DBOARD=NucleoF446RE       -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake
cmake --build build-stm -j
```

You must point to your STM32Cube HAL sources or provide them via `platform/stm32/*/CMakeLists.txt`.
