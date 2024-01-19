// ssd1306.c
//
// 05-07-2022 v19-11-23//06-12-23


#include "ssd1306.h"


extern const uint8_t num16x30[];
extern const uint8_t font5x7[];


extern void delay_ms(volatile uint16_t val);


static uint8_t scrbuff[SSD1306_BUFF_SIZE];

static const uint8_t SSD1306_I2C_ADDRESS = 0x78;

static const uint8_t SSD1306_COMM_MODE = 0x00;
static const uint8_t SSD1306_DATA_MODE = 0x40;

//static const uint8_t SSD1306_SLEEP_CMD = 0xae;


static const uint8_t initc[] =  //init commands
{
  0xae, //display off sleep mode
  0xd5, //display clock divide
  0x80, //
  0xa8, //set multiplex ratio
  0x3f, //
  0xd3, //display offset
  0x00, //
  0x40, //set display start line
  0x8d, //charge pump setting
  0x14, //
  0x20, //memory addressing mode
  0x00, //horizontal addressing mode
#if SSD1306_SCREEN_ROTATE
  0xa0, //segment re-map
  0xc0, //COM output scan directio
  0xda, //COM pins hardware configuration
  0x02, //0b00xx0010
#else
  0xa1, //segment re-map
  0xc8, //COM output scan direction
  0xda, //COM pins hardware configuration
  0x22, //0b00xx0010
#endif
  0x81, //set contrast (brightness)
  SSD1306_DEFAULT_BRIGHTNESS,
  0xd9, //pre-charge period
  0xf1, //
  0xdb, //VCOMH deselect level
  0x40, //
  0xa4, //entire display off
  0xa6, //normal display, 0xa7 inverse display
  0xaf  //display turned on
};


static const uint8_t gotoc[] =
{
  0x21, //set column address
  0,    //start address
  127,  //end address

  0x22, //set page address
  0,
  7,
};



//-------------------------------------------------------------------------------------------------
void oled_comm(uint8_t data)  //send a command
    {
    i2c_start();
    i2c_send_byte(SSD1306_I2C_ADDRESS);
    i2c_send_byte(SSD1306_COMM_MODE);
    i2c_send_byte(data);
    i2c_stop();
    }

/*
//-------------------------------------------------------------------------------------------------
static void ssd1306_write(uint8_t mode, uint8_t *data, uint16_t size)
    {
    i2c_start();
    i2c_send_byte(SSD1306_I2C_ADDRESS);
    i2c_send_byte(mode);
    for(uint16_t i=0; i<size; i++) i2c_send_byte(*data++);
    i2c_stop();
    }
*/


//-------------------------------------------------------------------------------------------------
static void buff_clear(void)  //clear buffer
    {
    for(uint16_t k=0; k<SSD1306_BUFF_SIZE; k++) scrbuff[k]=0x00;
    }


//-------------------------------------------------------------------------------------------------
void oled_update(void) //write buffer to screen
    {
    i2c_start();
    i2c_send_byte(SSD1306_I2C_ADDRESS);
    i2c_send_byte(SSD1306_COMM_MODE);
    for(uint8_t i=0; i<(sizeof gotoc/sizeof gotoc[0]); i++) i2c_send_byte(gotoc[i]);
    i2c_stop();

    i2c_start();
    i2c_send_byte(SSD1306_I2C_ADDRESS);
    i2c_send_byte(SSD1306_DATA_MODE);
    for(uint16_t i=0; i<SSD1306_BUFF_SIZE; i++) i2c_send_byte(scrbuff[i]);
    i2c_stop();

    buff_clear();
    }


/*
//-------------------------------------------------------------------------------------------------
void oled_goto_start(void)
    {
    i2c_start();
    i2c_send_byte(SSD1306_I2C_ADDRESS);
    i2c_send_byte(SSD1306_COMM_MODE);
    for(uint8_t i=0; i<(sizeof gotoc/sizeof gotoc[0]); i++) i2c_send_byte(gotoc[i]);
    i2c_stop();
    }

//-------------------------------------------------------------------------------------------------
void oled_write_byte(uint8_t byte)
    {
    i2c_start();
    i2c_send_byte(SSD1306_I2C_ADDRESS);
    i2c_send_byte(SSD1306_DATA_MODE);
    i2c_send_byte(byte);
    i2c_stop();
    }


//-------------------------------------------------------------------------------------------------
inline void buff_write_byte(uint16_t num, uint8_t byte)
    {
    scrbuff[num]=byte;
    }
*/


//-------------------------------------------------------------------------------------------------
void oled_init(void) //init i2c0 and display
    {
    delay_ms(100);  //delay for power up

    i2c_init();

    for(uint8_t i=0; i<(sizeof initc); i++) oled_comm(initc[i]); //send the init commands

    buff_clear();

    oled_update();
    }


//-------------------------------------------------------------------------------------------------
void oled_sleep(void)
    {
    i2c_start();
    i2c_send_byte(SSD1306_I2C_ADDRESS);
    i2c_send_byte(SSD1306_COMM_MODE);
    i2c_send_byte(0xae);
    i2c_stop();
    }


//-------------------------------------------------------------------------------------------------
void oled_fill(void)
    {
    for(uint16_t k=0; k<SSD1306_BUFF_SIZE; k++) scrbuff[k]=0xff;
    }


//-------------------------------------------------------------------------------------------------
void oled_pixel(uint8_t x, uint8_t y, uint8_t on)  //x: 0..127  //y: 0..31  //on: 0..1
    {
    if(x<=127 && y<=31)
        {
        if(on) scrbuff[x+128*(y/8)] |= 1<<(y%8);
        else scrbuff[x+128*(y/8)] &= ~(1<<(y%8));
        }
    }


//-------------------------------------------------------------------------------------------------
void oled_pixel_inv(uint8_t x, uint8_t y)  //x: 0..127  //y: 0..31
    {
    if(x<=127 && y<=31) scrbuff[x+128*(y/8)] ^= 1<<(y%8);
    }


//-------------------------------------------------------------------------------------------------
void oled_v(uint8_t x, uint8_t y, uint8_t db)  //db - data byte
    {
    for(uint8_t i=0; i<8; i++)
        {
        if( (db >> (7-i)) & 1 ) oled_pixel(x, y-i, 1);
        //else oled_pixel(x, y-i, 0);
        }
    }


//-------------------------------------------------------------------------------------------------
void oled_h(uint8_t x, uint8_t y, uint8_t db)  //db - data byte
    {
    for(uint8_t i=0; i<8; i++)
        {
        if( (db >> (7-i)) & 1 ) oled_pixel(x+i, y, 1);
        //else oled_pixel(x+i, y, 0);
        }
    }


//-------------------------------------------------------------------------------------------------
void oled_char(uint8_t x, uint8_t y, uint8_t sign)  //draw a character
    {
    if(sign<32 || sign>127) sign=128;

    //oled_v(x-1, y, 0x00);
    //oled_pixel(x-1, y-8, 0);

    for(uint8_t i=0; i<5; i++)
        {
        oled_v(x+i, y, font5x7[5*(sign-32)+i]);
        //oled_pixel(x+i, y-8, 0);
        }

    oled_v(x+5, y, 0x00);
    //oled_pixel(x+5, y-8, 0);
    }


//-------------------------------------------------------------------------------------------------
void oled_char_w(uint8_t x, uint8_t y, uint8_t sign)
    {
    if(sign<32 || sign>127) sign=128;
    for(uint8_t i=0; i<5; i++)
        {
        oled_v(x+i, y, font5x7[5*(sign-32)+i]);
        oled_v(x+i+1, y, font5x7[5*(sign-32)+i]);
        }
    oled_v(x+5, y, 0x00);
    }


/*

//-------------------------------------------------------------------------------------------------
void oled_char(uint8_t x, uint8_t y, uint8_t sign)  //x: 0..127 //y: 0..31
    {
    if(sign<32 || sign>127) sign=128;

    for(uint8_t i=0; i<5; i++) oled_v(x+i, y, font5x7[5*(sign-32)+i]);
    }
 */

//-------------------------------------------------------------------------------------------------
void oled_print(uint8_t x, uint8_t y, char *str)  //print a string
    {
    for(; (*str && x<=127 && y<=31); x+=6)
        {
        oled_char(x, y, *str++);
        }
    }


//-------------------------------------------------------------------------------------------------
void oled_print_w(uint8_t x, uint8_t y, char *str)
    {
    for(; (*str && x<=127 && y<=31); x+=8)
        {
        oled_char_w(x, y, *str++);
        }
    }


//-------------------------------------------------------------------------------------------------
void oled_print_wn(uint8_t x, uint8_t y, uint8_t n, char *str)
    {
    for(; (*str && x<=127 && y<=31); )
        {
        if(n)
            {
            oled_char_w(x, y, *str++);
            x+=8;
            }
        else
            {
            oled_char(x, y, *str++);
            x+=6;
            }
        
        if(n) n--;
        }
    }


//-------------------------------------------------------------------------------------------------
void oled_char_inv(uint8_t x, uint8_t y, uint8_t l)  //x: 0..127 //y: 0..31 //l: 1..128
    {
    for(uint8_t k=0; k<l; k++)
        {
        for(uint8_t i=0; i<9; i++)
            {
            oled_pixel_inv(x+k, y-i);
            }
        }
    }


//-------------------------------------------------------------------------------------------------
void oled_num16x30(uint8_t x, uint8_t y, uint8_t sign)  //drawing the 16x30pixels digit
    {
    for(uint8_t i=0; (i<60) && (sign>=48) && (sign<=57); i+=2)
        {
        oled_h(x, y-30+i/2, num16x30[i+(sign-48)*60]);
        oled_h(x+8, y-30+i/2, num16x30[i+1+(sign-48)*60]);
        }
    }


//-------------------------------------------------------------------------------------------------
void oled_print16x30(uint8_t x, uint8_t y, char *str)  //print a string 16x30pixels digit
    {
    for(; (*str && x<=127); x+=16) oled_num16x30(x, y, *str++);
    }


//-------------------------------------------------------------------------------------------------
void oled_h32(uint8_t x, uint8_t y, uint32_t data)
    {
    for(uint8_t i=0; i<32; i++)
        {
        if( (data >> (31-i)) & 1 ) oled_pixel(x+i, y, 1);
        }
    }


//-------------------------------------------------------------------------------------------------
void oled_h16(uint8_t x, uint8_t y, uint16_t data)
    {
    for(uint8_t i=0; i<16; i++)
        {
        if( (data >> (15-i)) & 1 ) oled_pixel(x+i, y, 1);
        }
    }


//-------------------------------------------------------------------------------------------------
void draw_pic32x21(uint8_t x, uint8_t y, const uint32_t* data)
    {
    for(uint8_t k=0; k<21; k++)
        {
        oled_h32(x,y+k,data[k]);
        }
    }



//-------------------------------------------------------------------------------------------------
void draw_pic8(uint8_t x, uint8_t y, uint8_t h, const uint8_t* data)
    {
    for(uint8_t k=0; k<h; k++)
        {
        oled_h(x,y+k,data[k]);
        }
    }


//-------------------------------------------------------------------------------------------------
void draw_pic16(uint8_t x, uint8_t y, uint8_t h, const uint16_t* data)
    {
    for(uint8_t k=0; k<h; k++)
        {
        oled_h16(x,y+k,data[k]);
        }
    }


//-------------------------------------------------------------------------------------------------
void oled_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t on) //draw a line
    {
    int16_t p;

    int16_t x = x1;
    int16_t y = y1;

    int16_t dx = abs(x2-x1);
    int16_t dy = abs(y2-y1);

    int16_t addx, addy;

    if(x1>x2) addx=-1;
    else addx=1;

    if(y1>y2) addy=-1;
    else addy=1;

    if(dx>=dy)
        {
        p=2*dy-dx;

        for(uint16_t k=0; k<=dx; k++)
            {
            oled_pixel(x, y, on);

            if(p<0)
                {
                p+=2*dy;
                x+=addx;
                }
            else
                {
                p+=2*dy-2*dx;
                x+=addx;
                y+=addy;
                }
            }
        }
    else
        {
        p=2*dx-dy;

        for(uint16_t k=0; k<=dy; k++)
            {
            oled_pixel(x, y, on);

            if(p<0)
                {
                p+=2*dx;
                y+=addy;
                }
            else
                {
                p+=2*dx-2*dy;
                x+=addx;
                y+=addy;
                }
            }
        }
    }



