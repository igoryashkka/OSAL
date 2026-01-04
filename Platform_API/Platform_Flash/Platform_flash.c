#include "platform_flash.h"

__attribute__((weak)) int FSH_Read(uint32_t addr, void* dst, size_t len)
{
    (void)addr; (void)dst;
    return (int)len;
}
__attribute__((weak)) int FSH_Write(uint32_t addr, const void* src, size_t len)
{
    (void)addr; (void)src;
    return (int)len;
}
