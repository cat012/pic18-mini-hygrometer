// i2c.h
// software i2c
// 30-Sep-2023


#ifndef I2C_H_
#define I2C_H_


#include <xc.h>
#include <stdint.h>


extern inline void delay_1us(void);


#define I2C1_ENABLE  1


#define I2C_DELAY  delay_1us()

#if I2C1_ENABLE
#define I2C1_DELAY  delay_1us()
#endif


///////////////////////////////////////////////////////////////////////////////

#define I2C_SDA_PIN   PORTAbits.RA0
#define I2C_SDA_TRIS  TRISAbits.RA0
#define I2C_SDA_LAT   LATAbits.LATA0

#define I2C_SCK_PIN   PORTAbits.RA2
#define I2C_SCK_TRIS  TRISAbits.RA2
#define I2C_SCK_LAT   LATAbits.LATA2

#if I2C1_ENABLE
#define I2C1_SDA_PIN   PORTCbits.RC4
#define I2C1_SDA_TRIS  TRISCbits.RC4
#define I2C1_SDA_LAT   LATCbits.LATC4

#define I2C1_SCK_PIN   PORTCbits.RC6
#define I2C1_SCK_TRIS  TRISCbits.RC6
#define I2C1_SCK_LAT   LATCbits.LATC6
#endif

///////////////////////////////////////////////////////////////////////////////


#define I2C_SDA_CLR  I2C_SDA_LAT=0
#define I2C_SDA_SET  I2C_SDA_LAT=1
#define I2C_SDA_OUT  I2C_SDA_TRIS=0
#define I2C_SDA_INP  I2C_SDA_TRIS=1

#define I2C_SCK_CLR  I2C_SCK_LAT=0
#define I2C_SCK_SET  I2C_SCK_LAT=1
#define I2C_SCK_OUT  I2C_SCK_TRIS=0
#define I2C_SCK_INP  I2C_SCK_TRIS=1


#define I2C_SDA_PULL  I2C_SDA_INP
#define I2C_SDA_NULL  I2C_SDA_CLR; I2C_SDA_OUT

#define I2C_SDA_IS_PULL  I2C_SDA_PIN==1

#define I2C_SCK_PULL  I2C_SCK_INP
#define I2C_SCK_NULL  I2C_SCK_CLR; I2C_SCK_OUT


#if I2C1_ENABLE
#define I2C1_SDA_CLR  I2C1_SDA_LAT=0
#define I2C1_SDA_SET  I2C1_SDA_LAT=1
#define I2C1_SDA_OUT  I2C1_SDA_TRIS=0
#define I2C1_SDA_INP  I2C1_SDA_TRIS=1

#define I2C1_SCK_CLR  I2C1_SCK_LAT=0
#define I2C1_SCK_SET  I2C1_SCK_LAT=1
#define I2C1_SCK_OUT  I2C1_SCK_TRIS=0
#define I2C1_SCK_INP  I2C1_SCK_TRIS=1


#define I2C1_SDA_PULL  I2C1_SDA_INP
#define I2C1_SDA_NULL  I2C1_SDA_CLR; I2C1_SDA_OUT

#define I2C1_SDA_IS_PULL  I2C1_SDA_PIN==1

#define I2C1_SCK_PULL  I2C1_SCK_INP
#define I2C1_SCK_NULL  I2C1_SCK_CLR; I2C1_SCK_OUT
#endif


#define I2C_ACK   0
#define I2C_NACK  1


void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_send_byte(uint8_t);
uint8_t i2c_read_byte(uint8_t);
void i2c_read_bytes(uint8_t, uint8_t*);

#if I2C1_ENABLE
void i2c1_init(void);
void i2c1_start(void);
void i2c1_stop(void);
void i2c1_send_byte(uint8_t data);
uint8_t i2c1_read_byte(uint8_t ack);
void i2c1_read_bytes(uint8_t, uint8_t*);
#endif




#endif  //end of I2C_H_


