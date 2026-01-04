#include "Platform_Flash/platform_flash.h"

/* TODO: Implement for STM32H7 */
int Platform_Flash_Read(uint32_t addr, void* dst, size_t len){(void)addr;(void)dst;return (int)len;}
int Platform_Flash_Write(uint32_t addr, const void* src, size_t len){(void)addr;(void)src;return (int)len;}
