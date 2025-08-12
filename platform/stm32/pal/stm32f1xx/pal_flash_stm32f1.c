#include "pal/pal_flash.h"
#include "stm32f1xx_hal.h"

int pal_flash_unlock(void){ return (HAL_FLASH_Unlock()==HAL_OK)?0:-1; }
int pal_flash_lock(void){ HAL_FLASH_Lock(); return 0; }
int pal_flash_erase_page(uint32_t page_addr){
  FLASH_EraseInitTypeDef e = { .TypeErase = FLASH_TYPEERASE_PAGES, .PageAddress = page_addr, .NbPages=1 };
  uint32_t err = 0;
  return (HAL_FLASHEx_Erase(&e, &err)==HAL_OK)?0:-1;
}
int pal_flash_program_halfword(uint32_t addr, uint16_t data){
  return (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, data)==HAL_OK)?0:-1;
}
