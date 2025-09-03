/**
 * @file    nvm_platform.h
 * @brief   Platform selection and low-level flash programming traits.
 *
 * Define one of:
 *   - NVM_PLATFORM_STM32L0
 *   - NVM_PLATFORM_STM32F1   (e.g., STM32F103C6)
 *
 * If neither is defined, defaults to L0.
 */
#pragma once

/* ===================== User selection (override via compiler flags) ===================== */
#if !defined(NVM_PLATFORM_STM32L0) && !defined(NVM_PLATFORM_STM32F1)
  #define NVM_PLATFORM_STM32L0 0
#endif

/* ===================== Capabilities & programming granularity ===================== */
#if NVM_PLATFORM_STM32L0
  /* STM32L0: has true Data EEPROM; Flash programming unit is 32-bit word */
  #define NVM_HAS_DATA_EEPROM        1
  #define NVM_FLASH_PROG_UNIT_BYTES  4u
  #define NVM_HAL_FLASH_PROG_TYPE    FLASH_TYPEPROGRAM_WORD
  #define NVM_HAL_EEPROM_PROG_TYPE   FLASH_TYPEPROGRAMDATA_WORD
  #include "stm32l0xx_hal.h"
#elif NVM_PLATFORM_STM32F1
  /* STM32F1 (e.g., STM32F103C6): no Data EEPROM; Flash program unit is 16-bit halfword */
  #define NVM_HAS_DATA_EEPROM        0
  #define NVM_FLASH_PROG_UNIT_BYTES  2u
  #define NVM_HAL_FLASH_PROG_TYPE    FLASH_TYPEPROGRAM_HALFWORD
  #include "stm32f1xx_hal.h"
#else
  #error "Unsupported platform: define NVM_PLATFORM_STM32L0 or NVM_PLATFORM_STM32F1"
#endif
