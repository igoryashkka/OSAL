# Vendors & Third-Party Libraries

This folder contains external dependencies and vendor libraries.

## Purpose

Store third-party code, libraries, and frameworks that are integrated into the project:
- ST Microelectronics CMSIS/HAL (if used)
- External libraries (FreeRTOS, uC/OS-III, etc.)
- Protocol stacks
- Math libraries

## Guidelines

- Keep vendor code unmodified (use wrappers if changes needed)
- Document the version and source of each library
- Create a README for each major dependency

## Example Structure

```
vendors/
├── stm32f1_cmsis/
│   ├── core_cm3.h
│   ├── stm32f1xx.h
│   └── README.md
├── stm32h7_cmsis/
│   └── ...
└── third_party_lib/
    └── README.md
```
