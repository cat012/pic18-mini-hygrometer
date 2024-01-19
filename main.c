//18-09-2023


#include <stdio.h>
#include <stdint.h>
#include "ssd1306.h"
#include "main.h"
#include "aht10.h"


volatile uint8_t syscnt=0;

char strbuff[16];
volatile uint8_t lowbat=0;


static const uint32_t foxpic[21] =
{
0b00000000000001000000001000000000,
0b00000000000011100000011100000000,
0b00000000000010110000110100000000,
0b00000000000010011000100100000000,
0b00000000000010011000100100000000,
0b00000000000011111111111100000000,
0b00000000000011111111111100000000,
0b00100000001111001111100111000000,
0b01100000000011001100100100000000,
0b11100000011111111111111111000000,
0b11110000001111111000111110000000,
0b11111000000000001111000000000000,
0b11111100000111111111100000000000,
0b01111110111111111111100000000000,
0b00111100111111111111100000000000,
0b00000001111011111111100000000000,
0b00000001110000111011100000000000,
0b00000001110110011011000000000000,
0b00000000110110011011000000000000,
0b00000000110110011011000000000000,
0b00000000010010001010000000000000
};

static const uint32_t upfoxpic[21] =
{
0b00000000000001000000001000000000,
0b00000000000011100000011100000000,
0b00000000000010110000110100000000,
0b00000000000010011000100100000000,
0b00000000000010011000100100000000,
0b00000000000011111111111100000000,
0b00000000000011111111111100000000,
0b00000000001111001111100111000000,
0b00000000000011001100100100000000,
0b00000000011111111111111111000000,
0b00000000001111111000111110000000,
0b00000000000000000111100000000000,
0b00111100000111111111100000000000,
0b01111110111111111111100000000000,
0b11111100111111111111100000000000,
0b11111001111011111111100000000000,
0b11110001110000111011100000000000,
0b11100001110110011011000000000000,
0b01100000110110011011000000000000,
0b00100000110110011011000000000000,
0b00000000010010001010000000000000
};


static const uint8_t drypic[10] =
{
0b10001000,
0b01000100,
0b00100010,
0b01000100,
0b10001000,
0b01000100,
0b00100010,
0b01000100,
0b10001000,
0b00000000
};

static const uint8_t wetpic[10] =
{
0b00000000,
0b00000000,
0b00000000,
0b00100000,
0b01110000,
0b01110000,
0b11111000,
0b11111000,
0b01110000,
0b00000000
};


static const uint16_t compic[9] =
{
0b0011100011100000,
0b0111110111110000,
0b0111111111110000,
0b0111111111110000,
0b0011111111100000,
0b0001111111000000,
0b0000111110000000,
0b0000011100000000,
0b0000001000000000
};


static const uint8_t batpic[16] =
{
0b00111000,
0b00111000,
0b11111110,
0b10000010,
0b10000010,
0b10010010,
0b10010010,
0b10010010,
0b10010010,
0b10000010,
0b10010010,
0b10000010,
0b10000010,
0b11111110
};


//=============================================================================
void __interrupt isr_high(void)
    {
    if(TMR0IF && TMR0IE)  //timer0 overflow - 100Hz ticks
        {
        TMR0IF=0;
        TMR0=TMR0_OVF_PRELOAD;
        if(syscnt) syscnt--;
        return;
        }
    
    if(LVDIF && LVDIE)  // low-voltage detect interrupt
        {
        LVDIF=0;
        LVDIE=0;
        lowbat=1;
        return;
        }
    }


//-----------------------------------------------------------------------------
inline void delay_1us(void)
    {
    __delay_us(1);
    }


//-----------------------------------------------------------------------------
void delay_ms(volatile uint16_t val)
    {
    while(val-->0) __delay_us(1000);
    }


//-----------------------------------------------------------------------------
void main(void)
    {
    IRCF2=1; IRCF1=1; IRCF0=1; //111=8M 110=4M 101=2M 100=1M 011=500k 010=250k 001=125k 000=31k

    //INTSRC PLLEN - TUN4 TUN3 TUN2 TUN1 TUN0
    OSCTUNE=0b00000000;

    SBOREN=1; //1=enable brown out reset
    SWDTEN=1; //0=disable watchdog timer
    RBPU=1;   //0=enable pull-up

    PEIE=1;
    GIE=1;

    //TMR0ON T08BIT T0CS T0SE PSA T0PS2 T0PS1 T0PS0
    T0CON=0b00000000 | TMR0_PRESCALER;
    TMR0=TMR0_OVF_PRELOAD;
    TMR0ON=1;
    TMR0IF=0;
    TMR0IE=1;

    //RD16 T1RUN T1CKPS1 T1CKPS0 T1OSCEN T1SYNC TMR1CS TMR1ON
    T1CON=0b00000000 | TMR1_PRESCALER;
    TMR1=TMR1_OVF_PRELOAD;
    TMR1ON=0;
    TMR1IF=0;
    TMR1IE=0;

    //- TOUTPS3 TOUTPS2 TOUTPS1 TOUTPS0 TMR2ON T2CKPS1 T2CKPS0
    T2CON=0b00000000 | TMR2_PRESCALER;
    TMR2=TMR2_OVF_PRELOAD;
    TMR2ON=0;
    TMR2IF=0;
    TMR2IE=0;

    ////RD16 T3CCP2 T3CKPS1 T3CKPS0 T3CCP1 T3SYNC TMR3CS TMR3ON
    T3CON=0b00000000 | TMR3_PRESCALER;
    TMR3=TMR3_OVF_PRELOAD;;
    TMR3ON=0;
    TMR3IF=0;
    TMR3IE=0;

    //- - DCxB1 DCxB0 CCPxM3 CCPxM2 CCPxM1 CCPxM0
    CCP2CON=0b00000000;

    //P1M1 P1M0 DC1B1 DC1B0 CCP1M3 CCP1M2 CCP1M1 CCP1M0
    CCP1CON=0b00000000;

    CMCON=0b000111; //Comparators Off

    ADCON1 = 0b00001111;
    ADCON0 = 0b00000000;
    ADCON2 = 0b00000000;

    UPIN_ALL_INIT;
    
    oled_init();

    oled_line(0,0,127,0,1);  
    oled_line(0,31,127,31,1);
    oled_line(0,31,0,0,1);
    oled_line(127,0,127,31,1);
    oled_line(0,15,127,15,1);
    oled_line(63,0,63,31,1);
    oled_line(31,0,31,31,1);
    oled_line(95,0,95,31,1);

    oled_update();

    LVDEN=1;  //enable low-voltage detect
    delay_ms(100);
    LVDL3=1; LVDL2=0; LVDL1=0; LVDL0=1; //1001=3.5V-3.72V //1000=3.3V-3.52V
    while(LVDCONbits.IRVST==0);
    LVDIF=0;
    LVDIE=1;  //low-voltage detect interrupt
    
    aht10_init();
    aht10_start_meas();
    delay_ms(100);

    uint8_t scrupd=0;

    for(;;)
        {
        if(syscnt==0)
            {
            syscnt=EVENT_PERIOD(500);

            scrupd=0;
            CLRWDT();
            }

        if(scrupd==0)
            {
            scrupd=1;
            
            static float t=0;
            static float h=0;
            uint8_t r=255;
            
            static uint8_t k=0;
            if(k==0)aht10_read_data(&r ,&t, &h);
            if(++k>1) k=0;

            sprintf(strbuff, "%+5.1f",t);
            oled_print_w(90,7,strbuff);
            
            sprintf(strbuff, ".%1u",(uint16_t)(h*10)%10);
            oled_print_w(80,31,strbuff);
            
            sprintf(strbuff, "%02u",(uint8_t)h);
            oled_print16x30(47,31,strbuff);

            if(lowbat) draw_pic8(0,0,sizeof batpic,batpic);

            if(h>30 && h<60) draw_pic16(27,0,9, compic);
            else if(h<30) draw_pic8(30,0,sizeof drypic,drypic);
            else if(h>60) draw_pic8(30,0,sizeof wetpic,wetpic);

            static uint8_t c=0;
            if(c) draw_pic32x21(0,8,upfoxpic);
            else  draw_pic32x21(0,10,foxpic);
            if(++c>1) c=0;
            
            oled_update();

            aht10_start_meas();
            }
        }
    }

