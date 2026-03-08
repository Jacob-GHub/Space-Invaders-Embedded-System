// ssd1306.h
// I used the SSD1306 datasheet to understand how to control the OLED display
// Datasheet: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
// Also looked at example code where i was able to understand and implement for my own use cases here: https://github.com/adafruit/Adafruit_SSD1306

#ifndef SSD1306_H
#define SSD1306_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

extern unsigned char screen_buffer[1024];

void setup_display(void);
void update_display(void);
void clear_display(void);
void draw_pixel(unsigned char x, unsigned char y, unsigned char color);
void draw_char(unsigned char x, unsigned char y, char c);
void draw_string(unsigned char x, unsigned char y, const char *str);
void draw_number(unsigned char x, unsigned char y, int num);
void draw_rectangle(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char color);

extern const unsigned char font5x7[][5] PROGMEM;

#endif