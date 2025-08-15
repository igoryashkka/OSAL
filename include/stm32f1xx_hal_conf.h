/* include/stm32f1xx_hal_conf.h */
#ifndef STM32F1xx_HAL_CONF_H
#define STM32F1xx_HAL_CONF_H

/* ---- Включаемые HAL-модули ---- */
#define HAL_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED     /* нужно для UART */
#define HAL_UART_MODULE_ENABLED

/* ---- Значения источников тактирования и таймаутов ---- */
#define HSE_VALUE              8000000U
#define HSE_STARTUP_TIMEOUT    100U
#define LSE_VALUE              32768U
#define LSE_STARTUP_TIMEOUT    5000U
#define HSI_VALUE              8000000U
#define LSI_VALUE              40000U
#define VDD_VALUE              3300U
#define TICK_INT_PRIORITY      0U
#define USE_RTOS               0U
#define PREFETCH_ENABLE        1U

#include "stm32f1xx_hal_def.h"

/* ---- Подключаем заголовки модулей (порядок важен) ---- */
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_cortex.h"
#include "stm32f1xx_hal_pwr.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_uart.h"

#ifdef  USE_FULL_ASSERT
/* Если хотите фулл-ассерты — реализуйте этот хук у себя */
void assert_failed(uint8_t *file, uint32_t line);
#endif

/* ---------- Assert macro ---------- */
/* Не вмикаємо USE_FULL_ASSERT — assert_param перетвориться на no-op. */
#if !defined(USE_FULL_ASSERT)
/* За замовчуванням: не робити нічого, просто існувати як макрос */
#define assert_param(expr)     ((void)0U)
#else
void assert_failed(uint8_t *file, uint32_t line);
#define assert_param(expr)     ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
#endif

#endif /* STM32F1xx_HAL_CONF_H */
