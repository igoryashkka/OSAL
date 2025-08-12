#include "pal/pal_clock.h"
#include "stm32f1xx_hal.h"

static uint32_t g_sysclk;

int pal_clock_init(const pal_clock_cfg_t *cfg)
{
  HAL_Init();

  RCC_OscInitTypeDef osc = {0};
  RCC_ClkInitTypeDef clk = {0};

  if (cfg->src == PAL_CLK_SRC_HSE_8MHz || cfg->src == PAL_CLK_SRC_HSE_16MHz) {
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc.HSEState       = RCC_HSE_ON;
    osc.HSEPredivValue = (cfg->src == PAL_CLK_SRC_HSE_16MHz) ? RCC_HSE_PREDIV_DIV2 : RCC_HSE_PREDIV_DIV1;
    osc.PLL.PLLState   = RCC_PLL_ON;
    osc.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    // e.g. 8 MHz * 9 = 72 MHz
    osc.PLL.PLLMUL     = RCC_PLL_MUL9;
  } else {
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState       = RCC_HSI_ON;
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    osc.PLL.PLLState   = RCC_PLL_ON;
    osc.PLL.PLLSource  = RCC_PLLSOURCE_HSI_DIV2;
    osc.PLL.PLLMUL     = RCC_PLL_MUL16; // 8MHz/2 *16 = 64 MHz (example)
  }
  if (HAL_RCC_OscConfig(&osc) != HAL_OK) return -1;

  clk.ClockType      = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  clk.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  clk.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  clk.APB1CLKDivider = RCC_HCLK_DIV2;
  clk.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2) != HAL_OK) return -2;

  g_sysclk = HAL_RCC_GetSysClockFreq();
  return 0;
}

uint32_t pal_clock_get_sysclk(void) { return g_sysclk; }
