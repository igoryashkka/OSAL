# STM8 Platform (Scaffold)

This directory contains the STM8 platform-specific startup and linker assets for
STM8L151 targets. The files are currently **placeholders** and should be
replaced with the correct STM8L151 vector table, startup routine, and linker
script before attempting to flash hardware.

Expected files for the STM8L151 build:

- `startup/stm8l151x.s` - startup/interrupt vectors
- `ld/stm8l151.lk` - SDCC linker script for STM8L151
