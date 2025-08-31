/* Example for STM32F103C6 (no EEPROM; Flash backend is used). */
#include "stm32f1xx_hal.h"
/* Select F1 platform at compile time, e.g. add -DNVM_PLATFORM_STM32F1=1 */
#include "nvm_storage.h"

static NvmContext g_nvm;
//void SystemClock_Config(void);

int main_app(void) {
    HAL_Init();
//    SystemClock_Config();
    nvm_init(&g_nvm);

    uint32_t t0 = HAL_GetTick();
    for (;;) {
        if (HAL_GetTick() - t0 > 10000) {
            NvmConfigBlob cfg = *nvm_get(&g_nvm);
            cfg.volume = (uint8_t)((cfg.volume + 1u) % 100u);
            g_nvm.working = cfg;
            t0 = HAL_GetTick();
        }
        nvm_tick(&g_nvm, HAL_GetTick());
    }
}
