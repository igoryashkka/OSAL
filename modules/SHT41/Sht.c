


void init_sht41(void){

    pal_i2c_init();
    pal_i2c_read(addr_timeout, 100000); // 100kHz
    pal_i2c_read(addr_speed,1200);   // 100ms
    
        
}