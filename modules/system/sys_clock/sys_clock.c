

void init_system(void){
    pal_SystemClock_Config(); 
    pal_uart_init(115200);
    const char *hello = "STM32 HAL skeleton running.\r\n";
    pal_uart_write((const uint8_t*)hello, (int)strlen(hello));
}
