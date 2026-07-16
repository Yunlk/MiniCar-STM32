#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"

#define OLED_WIDTH   128
#define OLED_HEIGHT   64

uint8_t oled_init(void);
void oled_clear(void);
uint8_t oled_update(void);

void oled_draw_pixel(int16_t x, int16_t y, uint8_t color);
void oled_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
void oled_draw_rect(int16_t x, int16_t y, int16_t width, int16_t height, uint8_t color);
void oled_fill_rect(int16_t x, int16_t y, int16_t width, int16_t height, uint8_t color);
void oled_draw_char(int16_t x, int16_t y, char ch, uint8_t color);
void oled_draw_text(int16_t x, int16_t y, const char *text, uint8_t color);
void oled_draw_big_digit(int16_t x, int16_t y, uint8_t digit, uint8_t color);

#endif
