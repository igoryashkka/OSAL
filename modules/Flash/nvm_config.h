/**
 * @file    nvm_config.h
 * @brief   Build-time configuration and memory mapping for STM32 NVM storage library.
 *
 * Recommended linker script additions (CubeIDE .ld):
 *   MEMORY
 *   {
 *     FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 60K
 *     NVM   (rw) : ORIGIN = 0x0800F000, LENGTH = 4K
 *     RAM   (rwx): ORIGIN = 0x20000000, LENGTH = 20K
 *   }
 *   PROVIDE(__nvm_start__ = ORIGIN(NVM));
 *   PROVIDE(__nvm_end__   = ORIGIN(NVM) + LENGTH(NVM));
 *
 * Then compile with: -DNVM_USE_LINKER_REGION
 */
#pragma once
#include <stdint.h>
#include "nvm_platform.h"

/*============================= Backend Selection =============================*/
/* Prefer EEPROM if available; otherwise use Flash backend */
#ifndef NVM_USE_DATA_EEPROM
  #if NVM_HAS_DATA_EEPROM
    #define NVM_USE_DATA_EEPROM  1
  #else
    #define NVM_USE_DATA_EEPROM  0
  #endif
#endif

/*=========================== Data EEPROM Parameters ==========================*/
#if NVM_HAS_DATA_EEPROM
  #ifndef NVM_DATA_EEPROM_BASE
    #define NVM_DATA_EEPROM_BASE  (0x08080000UL) /* STM32L0 typical */
  #endif
  #ifndef NVM_DATA_EEPROM_SIZE
    #define NVM_DATA_EEPROM_SIZE  (2048UL)
  #endif
#endif

/*============================= Flash Region Mapping ==========================*/
/*
 * Strategy:
 *  1) If you define -DNVM_USE_LINKER_REGION and your linker exports __nvm_start__/__nvm_end__,
 *     use those (best).
 *  2) Else, fall back to platform defaults below (adjust if needed).
 *
 * Notes for STM32F103C8 (64 KB):
 *  - Flash page size = 1 KB.
 *  - If last 4 KB are reserved in linker, NVM = [0x0800F000 .. 0x08010000).
 */
#if !NVM_USE_DATA_EEPROM  /* Flash backend in use */

  /* -------- Page size by platform -------- */
  #ifndef NVM_FLASH_PAGE_SIZE
    #if NVM_PLATFORM_STM32L0
      #define NVM_FLASH_PAGE_SIZE   (128U)
    #elif NVM_PLATFORM_STM32F1
      #define NVM_FLASH_PAGE_SIZE   (1024U)  /* 1 KB on low/med-density F1 */
    #else
      #error "Set NVM_FLASH_PAGE_SIZE for this platform"
    #endif
  #endif

  /* -------- Prefer linker-provided region when requested -------- */
  #ifndef NVM_FLASH_REGION_BASE

    #ifdef NVM_USE_LINKER_REGION
      /* These symbols are addresses provided by the linker (see .ld PROVIDE lines). */
      extern const uint8_t __nvm_start__[];
      extern const uint8_t __nvm_end__[];

      #define NVM_FLASH_REGION_BASE  ((uint32_t)__nvm_start__)
      #define NVM_FLASH_REGION_END   ((uint32_t)__nvm_end__)
      #define NVM_FLASH_REGION_BYTES (NVM_FLASH_REGION_END - NVM_FLASH_REGION_BASE)
      #define NVM_FLASH_NUM_PAGES    (NVM_FLASH_REGION_BYTES / NVM_FLASH_PAGE_SIZE)

    #else
      /* -------- Fallback defaults (no linker symbols) -------- */
      #if NVM_PLATFORM_STM32L0
        /* Example: last ~1 KB on small L0 parts */
        #define NVM_FLASH_REGION_BASE (0x08007C00UL)
        #define NVM_FLASH_NUM_PAGES   (8U)    /* 8 * 128 B = 1 KB */
      #elif NVM_PLATFORM_STM32F1
        /* F103C8: reserve last 4 KB by default (pages 60..63) */
        #define NVM_FLASH_REGION_BASE (0x0800F000UL)
        #define NVM_FLASH_NUM_PAGES   (4U)    /* 4 * 1 KB = 4 KB */
      #endif
    #endif /* NVM_USE_LINKER_REGION */

  #endif /* !NVM_FLASH_REGION_BASE */

  /* Derive region end/size if not already defined */
  #ifndef NVM_FLASH_REGION_END
    #define NVM_FLASH_REGION_END    (NVM_FLASH_REGION_BASE + (NVM_FLASH_NUM_PAGES * NVM_FLASH_PAGE_SIZE))
  #endif
  #ifndef NVM_FLASH_REGION_BYTES
    #define NVM_FLASH_REGION_BYTES  (NVM_FLASH_REGION_END - NVM_FLASH_REGION_BASE)
  #endif

#endif /* !NVM_USE_DATA_EEPROM */

/*=============================== Common Constants ============================*/
#ifndef NVM_MAGIC_VALUE
  #define NVM_MAGIC_VALUE       (0x4E564D31UL) /* 'NVM1' */
#endif
#ifndef NVM_STRUCT_VERSION
  #define NVM_STRUCT_VERSION    (0x00010000UL)
#endif
#ifndef NVM_PERIODIC_MS
  #define NVM_PERIODIC_MS       (3000U)
#endif




      /*
      //  runtime check: confirm NVM region is page-aligned and inside flash
      static inline int nvm_config_sanity_ok(void)
      {
        #if !NVM_USE_DATA_EEPROM

          if ((NVM_FLASH_REGION_BASE % NVM_FLASH_PAGE_SIZE) != 0u) return 0;
          if ((NVM_FLASH_REGION_END  % NVM_FLASH_PAGE_SIZE) != 0u) return 0;


          #if NVM_PLATFORM_STM32F1

            uint32_t kb = (uint32_t)(*(__IO uint16_t*)FLASHSIZE_BASE);
            uint32_t flash_end = 0x08000000u + kb * 1024u;
            if (NVM_FLASH_REGION_END > flash_end) return 0;
          #endif
        #endif
        return 1;
      }
      */
