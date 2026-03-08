#ifndef ILI9341_H
#define ILI9341_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "spi.h"
#include "helper.h"

// Pin definitions for display control
#define TFT_DC PB1  // Pin 9 - Data/Command
#define TFT_RST PB0 // Pin 8 - Reset
#define TFT_CS PB2  // Pin 10 - Chip Select

// Display dimensions
#define ILI9341_WIDTH 240
#define ILI9341_HEIGHT 320

// ILI9341 Commands
#define ILI9341_NOP 0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_SLPOUT 0x11
#define ILI9341_DISPON 0x29
#define ILI9341_CASET 0x2A
#define ILI9341_PASET 0x2B
#define ILI9341_RAMWR 0x2C
#define ILI9341_MADCTL 0x36
#define ILI9341_PIXFMT 0x3A
#define ILI9341_FRMCTR1 0xB1
#define ILI9341_DFUNCTR 0xB6
#define ILI9341_PWCTR1 0xC0
#define ILI9341_PWCTR2 0xC1
#define ILI9341_VMCTR1 0xC5
#define ILI9341_VMCTR2 0xC7

// Control pin macros
#define TFT_DC_LOW() PORTB &= ~(1 << TFT_DC)
#define TFT_DC_HIGH() PORTB |= (1 << TFT_DC)
#define TFT_CS_LOW() PORTB &= ~(1 << TFT_CS)
#define TFT_CS_HIGH() PORTB |= (1 << TFT_CS)
#define TFT_RST_LOW() PORTB &= ~(1 << TFT_RST)
#define TFT_RST_HIGH() PORTB |= (1 << TFT_RST)

void ILI9341_writeCommand(uint8_t cmd)
{
    TFT_CS_LOW(); // Select display (keep low)
    TFT_DC_LOW(); // Command mode
    SPI_write(cmd);
    TFT_DC_HIGH(); // Back to data mode
}

void ILI9341_writeData(uint8_t data)
{
    TFT_DC_HIGH(); // Data mode
    SPI_write(data);
}

void ILI9341_writeData16(uint16_t data)
{
    TFT_DC_HIGH();          // Data mode
    TFT_CS_LOW();           // Select display
    SPI_write(data >> 8);   // High byte
    SPI_write(data & 0xFF); // Low byte
    TFT_CS_HIGH();          // Deselect display
}

void ILI9341_setAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // Column address set
    ILI9341_writeCommand(ILI9341_CASET);
    ILI9341_writeData16(x1);
    ILI9341_writeData16(x2);

    // Page address set
    ILI9341_writeCommand(ILI9341_PASET);
    ILI9341_writeData16(y1);
    ILI9341_writeData16(y2);

    // Write to RAM
    ILI9341_writeCommand(ILI9341_RAMWR);
}

void ILI9341_init(void)
{
    // Set control pins as outputs
    DDRB |= (1 << TFT_DC) | (1 << TFT_RST) | (1 << TFT_CS);

    // Keep CS low for entire operation
    TFT_CS_LOW();

    // Hardware reset
    TFT_RST_HIGH();
    _delay_ms(5);
    TFT_RST_LOW();
    _delay_ms(20);
    TFT_RST_HIGH();
    _delay_ms(150);

    // Software reset
    ILI9341_writeCommand(ILI9341_SWRESET);
    _delay_ms(150);

    // Power control A
    ILI9341_writeCommand(0xCB);
    ILI9341_writeData(0x39);
    ILI9341_writeData(0x2C);
    ILI9341_writeData(0x00);
    ILI9341_writeData(0x34);
    ILI9341_writeData(0x02);

    // Power control B
    ILI9341_writeCommand(0xCF);
    ILI9341_writeData(0x00);
    ILI9341_writeData(0xC1);
    ILI9341_writeData(0x30);

    // Driver timing control A
    ILI9341_writeCommand(0xE8);
    ILI9341_writeData(0x85);
    ILI9341_writeData(0x00);
    ILI9341_writeData(0x78);

    // Driver timing control B
    ILI9341_writeCommand(0xEA);
    ILI9341_writeData(0x00);
    ILI9341_writeData(0x00);

    // Power on sequence control
    ILI9341_writeCommand(0xED);
    ILI9341_writeData(0x64);
    ILI9341_writeData(0x03);
    ILI9341_writeData(0x12);
    ILI9341_writeData(0x81);

    // Pump ratio control
    ILI9341_writeCommand(0xF7);
    ILI9341_writeData(0x20);

    // Power control 1
    ILI9341_writeCommand(ILI9341_PWCTR1);
    ILI9341_writeData(0x23);

    // Power control 2
    ILI9341_writeCommand(ILI9341_PWCTR2);
    ILI9341_writeData(0x10);

    // VCOM control 1
    ILI9341_writeCommand(ILI9341_VMCTR1);
    ILI9341_writeData(0x3E);
    ILI9341_writeData(0x28);

    // VCOM control 2
    ILI9341_writeCommand(ILI9341_VMCTR2);
    ILI9341_writeData(0x86);

    // Memory access control (rotation)
    ILI9341_writeCommand(ILI9341_MADCTL);
    ILI9341_writeData(0x48); // MX, BGR

    // Pixel format
    ILI9341_writeCommand(ILI9341_PIXFMT);
    ILI9341_writeData(0x55); // 16-bit color

    // Frame rate
    ILI9341_writeCommand(ILI9341_FRMCTR1);
    ILI9341_writeData(0x00);
    ILI9341_writeData(0x18);

    // Display function control
    ILI9341_writeCommand(ILI9341_DFUNCTR);
    ILI9341_writeData(0x08);
    ILI9341_writeData(0x82);
    ILI9341_writeData(0x27);

    // Enable 3G
    ILI9341_writeCommand(0xF2);
    ILI9341_writeData(0x00);

    // Gamma set
    ILI9341_writeCommand(0x26);
    ILI9341_writeData(0x01);

    // Positive gamma correction
    ILI9341_writeCommand(0xE0);
    ILI9341_writeData(0x0F);
    ILI9341_writeData(0x31);
    ILI9341_writeData(0x2B);
    ILI9341_writeData(0x0C);
    ILI9341_writeData(0x0E);
    ILI9341_writeData(0x08);
    ILI9341_writeData(0x4E);
    ILI9341_writeData(0xF1);
    ILI9341_writeData(0x37);
    ILI9341_writeData(0x07);
    ILI9341_writeData(0x10);
    ILI9341_writeData(0x03);
    ILI9341_writeData(0x0E);
    ILI9341_writeData(0x09);
    ILI9341_writeData(0x00);

    // Negative gamma correction
    ILI9341_writeCommand(0xE1);
    ILI9341_writeData(0x00);
    ILI9341_writeData(0x0E);
    ILI9341_writeData(0x14);
    ILI9341_writeData(0x03);
    ILI9341_writeData(0x11);
    ILI9341_writeData(0x07);
    ILI9341_writeData(0x31);
    ILI9341_writeData(0xC1);
    ILI9341_writeData(0x48);
    ILI9341_writeData(0x08);
    ILI9341_writeData(0x0F);
    ILI9341_writeData(0x0C);
    ILI9341_writeData(0x31);
    ILI9341_writeData(0x36);
    ILI9341_writeData(0x0F);

    // Exit sleep
    ILI9341_writeCommand(ILI9341_SLPOUT);
    _delay_ms(120);

    // Display on
    ILI9341_writeCommand(ILI9341_DISPON);
    _delay_ms(20);
}

void ILI9341_drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= ILI9341_WIDTH || y >= ILI9341_HEIGHT)
        return;

    ILI9341_setAddress(x, y, x, y);
    ILI9341_writeData16(color);
}

void ILI9341_fillScreen(uint16_t color)
{
    ILI9341_setAddress(0, 0, ILI9341_WIDTH - 1, ILI9341_HEIGHT - 1);

    TFT_DC_HIGH();
    TFT_CS_LOW();

    for (uint32_t i = 0; i < (uint32_t)ILI9341_WIDTH * ILI9341_HEIGHT; i++)
    {
        SPI_write(color >> 8);
        SPI_write(color & 0xFF);
    }

    TFT_CS_HIGH();
}

void ILI9341_fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (x >= ILI9341_WIDTH || y >= ILI9341_HEIGHT)
        return;
    if (x + w > ILI9341_WIDTH)
        w = ILI9341_WIDTH - x;
    if (y + h > ILI9341_HEIGHT)
        h = ILI9341_HEIGHT - y;

    ILI9341_setAddress(x, y, x + w - 1, y + h - 1);

    TFT_DC_HIGH();
    TFT_CS_LOW();

    for (uint16_t i = 0; i < w * h; i++)
    {
        SPI_write(color >> 8);
        SPI_write(color & 0xFF);
    }

    TFT_CS_HIGH();
}

void ILI9341_drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    // Draw hollow rectangle
    ILI9341_fillRect(x, y, w, 1, color);         // Top
    ILI9341_fillRect(x, y + h - 1, w, 1, color); // Bottom
    ILI9341_fillRect(x, y, 1, h, color);         // Left
    ILI9341_fillRect(x + w - 1, y, 1, h, color); // Right
}

void ILI9341_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    int16_t dx = x1 - x0;
    int16_t dy = y1 - y0;

    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;

    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;

    while (1)
    {
        ILI9341_drawPixel(x0, y0, color);

        if (x0 == x1 && y0 == y1)
            break;

        int16_t e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void ILI9341_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                          int16_t x2, int16_t y2, uint16_t color)
{
    // Sort vertices by y coordinate
    if (y0 > y1)
    {
        int16_t t = y0;
        y0 = y1;
        y1 = t;
        t = x0;
        x0 = x1;
        x1 = t;
    }
    if (y1 > y2)
    {
        int16_t t = y1;
        y1 = y2;
        y2 = t;
        t = x1;
        x1 = x2;
        x2 = t;
    }
    if (y0 > y1)
    {
        int16_t t = y0;
        y0 = y1;
        y1 = t;
        t = x0;
        x0 = x1;
        x1 = t;
    }

    if (y0 == y2)
    {
        return; // All points on same line
    }

    // Draw triangle
    for (int16_t y = y0; y <= y2; y++)
    {
        int16_t xa, xb;

        if (y < y1)
        {
            xa = x0 + (int32_t)(x2 - x0) * (y - y0) / (y2 - y0);
            xb = x0 + (int32_t)(x1 - x0) * (y - y0) / (y1 - y0);
        }
        else
        {
            xa = x0 + (int32_t)(x2 - x0) * (y - y0) / (y2 - y0);
            xb = x1 + (int32_t)(x2 - x1) * (y - y1) / (y2 - y1);
        }

        if (xa > xb)
        {
            int16_t t = xa;
            xa = xb;
            xb = t;
        }
        ILI9341_drawLine(xa, y, xb, y, color);
    }
}

// Simple 5x7 font (stored in PROGMEM to save RAM)
const uint8_t font5x7[][5] PROGMEM = {
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Space
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
};

void ILI9341_drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size)
{
    uint8_t index;

    if (c >= 'A' && c <= 'Z')
        index = c - 'A';
    else if (c >= 'a' && c <= 'z')
        index = c - 'a'; // Same as uppercase
    else if (c == ' ')
        index = 26;
    else if (c >= '0' && c <= '9')
        index = 27 + (c - '0');
    else if (c == ':')
        index = 37;
    else
        return; // Unsupported character

    for (uint8_t col = 0; col < 5; col++)
    {
        uint8_t line = pgm_read_byte(&font5x7[index][col]);
        for (uint8_t row = 0; row < 8; row++)
        {
            if (line & 0x01)
            {
                if (size == 1)
                    ILI9341_drawPixel(x + col, y + row, color);
                else
                    ILI9341_fillRect(x + col * size, y + row * size, size, size, color);
            }
            else if (bg != color)
            {
                if (size == 1)
                    ILI9341_drawPixel(x + col, y + row, bg);
                else
                    ILI9341_fillRect(x + col * size, y + row * size, size, size, bg);
            }
            line >>= 1;
        }
    }
}

void ILI9341_drawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size)
{
    while (*str)
    {
        ILI9341_drawChar(x, y, *str++, color, bg, size);
        x += 6 * size;
    }
}

#endif // ILI9341_H