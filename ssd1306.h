// SSD1306 Display Module 128*31 I2C
//
// ssd1306.h
// Microchip XC8 Compiler v1.33
//
// 05-07-2022 //06-12-23


#ifndef SSD1306_H
#define	SSD1306_H


#include <stdlib.h>
#include <stdint.h>

#include "swi2c.h"


#define SSD1306_BUFF_SIZE  (128U*4U)

#define SSD1306_SCREEN_ROTATE       0
#define SSD1306_DEFAULT_BRIGHTNESS  100U

void oled_sleep(void); //power off
void oled_init(void); //init i2c and display
void oled_update(void); //write buffer to screen

//void oled_fill(void);  //fill buffer
void oled_pixel(uint8_t x, uint8_t y, uint8_t on);  //x: 0..127  //y: 0..31  //on: 0..1
void oled_pixel_inv(uint8_t x, uint8_t y);  //x: 0..127  //y: 0..31
void oled_v(uint8_t x, uint8_t y, uint8_t db);  //db - data byte
void oled_h(uint8_t x, uint8_t y, uint8_t db);  //db - data byte
void oled_char(uint8_t x, uint8_t y, uint8_t sign);  //draw a character
void oled_print(uint8_t x, uint8_t y, char *str);  //print a string
void oled_char_inv(uint8_t x, uint8_t y, uint8_t l);  //x: 0..127 //y: 0..31 //l: 1..128
void oled_char_w(uint8_t x, uint8_t y, uint8_t sign);
void oled_print_w(uint8_t x, uint8_t y, char *str);
void oled_print_wn(uint8_t x, uint8_t y, uint8_t n, char *str);

void oled_num16x30(uint8_t x, uint8_t y, uint8_t sign);  //drawing the 16x30pixels digit
void oled_print16x30(uint8_t x, uint8_t y, char *str);  //print a string 16x30pixels digit
void oled_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t on); //draw a line

void oled_h32(uint8_t x, uint8_t y, uint32_t data);
void oled_h16(uint8_t x, uint8_t y, uint16_t data);
void draw_pic32x21(uint8_t x, uint8_t y, const uint32_t* data);
void draw_pic8(uint8_t x, uint8_t y, uint8_t h, const uint8_t* data);
void draw_pic16(uint8_t x, uint8_t y, uint8_t h, const uint16_t* data);

void oled_goto_start(void);
void oled_write_byte(uint8_t byte);
inline void buff_write_byte(uint16_t num, uint8_t byte);


#endif	/* SSD1306_H */

