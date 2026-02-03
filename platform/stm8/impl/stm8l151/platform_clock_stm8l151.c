#include "Platform_Clock/Platform_clock.h"

/* Minimal STM8 clock init for SDCC builds.
 * The full STM SPL depends on compiler-specific headers that are
 * not available for SDCC. For now, keep this as a no-op so the
 * project can build. Replace with proper SDCC-friendly clock
 * initialization when the toolchain support is ready.
 */
void RCC_InitDefault(void)
{
    /* TODO: implement HSI enable and system clock switch for STM8 */
    (void)0;
}
