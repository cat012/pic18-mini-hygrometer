//16-09-2023


#include "aht10.h"



extern void delay_ms(volatile uint16_t);


uint8_t aht10buff[6] = {0, 0, 0, 0, 0, 0};


const uint8_t AHT10_I2C_ADDR_WRITE = 0x70;
const uint8_t AHT10_I2C_ADDR_READ = 0x71;

const uint8_t AHT10_INIT_COMMAND = 0xE1;
const uint8_t AHT10_TRIGGER_MEASURMENT = 0xAC;
const uint8_t AHT10_DATA_MEASURMENT = 0x33;
const uint8_t AHT10_DATA_NOP = 0x00;
const uint8_t AHT10_SOFT_RESET = 0xBA;



//-----------------------------------------------------------------------------
void aht10_init(void)
    {
    delay_ms(100);

    i2c1_init();
    i2c1_start();
    i2c1_send_byte(AHT10_I2C_ADDR_WRITE);
    i2c1_send_byte(AHT10_SOFT_RESET);
    i2c1_stop();
    delay_ms(100);

    i2c1_start();
    i2c1_send_byte(AHT10_I2C_ADDR_WRITE);
    i2c1_send_byte(AHT10_INIT_COMMAND);
    i2c1_stop();

    delay_ms(400);
    }


//-----------------------------------------------------------------------------
void aht10_start_meas(void)
    {
    i2c1_start();
    i2c1_send_byte(AHT10_I2C_ADDR_WRITE);
    i2c1_send_byte(AHT10_TRIGGER_MEASURMENT);
    i2c1_send_byte(AHT10_DATA_MEASURMENT);
    i2c1_send_byte(AHT10_DATA_NOP);
    i2c1_stop();
    }


//-----------------------------------------------------------------------------
void aht10_read_data(uint8_t *r, float *t, float *h)
    {
    i2c1_start();
    i2c1_send_byte(AHT10_I2C_ADDR_READ);
    i2c1_read_bytes(sizeof aht10buff, aht10buff);
    i2c1_stop();

    *r = aht10buff[0];

    uint32_t temperature = ((uint32_t)(aht10buff[3] & 0x0F) << 16) | ((uint16_t)aht10buff[4] << 8) | aht10buff[5];
    *t=((float)temperature * 200 / 0x100000) - 50;

    uint32_t humidity = (((uint32_t)aht10buff[1] << 16) | ((uint16_t)aht10buff[2] << 8) | (aht10buff[3])) >> 4;
    *h = ((float)humidity * 100) / 0x100000;
    }


