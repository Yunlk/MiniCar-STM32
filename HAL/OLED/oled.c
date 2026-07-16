#include "oled.h"
#include <string.h>

#define OLED_I2C_PORT       GPIOB
#define OLED_I2C_RCC        RCC_APB2Periph_GPIOB
#define OLED_I2C_SCL_PIN    GPIO_Pin_10
#define OLED_I2C_SDA_PIN    GPIO_Pin_11
#define OLED_I2C_ADDRESS    0x3C
#define OLED_PAGE_COUNT     (OLED_HEIGHT / 8)
#define OLED_BUFFER_SIZE    (OLED_WIDTH * OLED_PAGE_COUNT)

#define OLED_SCL_HIGH()     GPIO_SetBits(OLED_I2C_PORT, OLED_I2C_SCL_PIN)
#define OLED_SCL_LOW()      GPIO_ResetBits(OLED_I2C_PORT, OLED_I2C_SCL_PIN)
#define OLED_SDA_HIGH()     GPIO_SetBits(OLED_I2C_PORT, OLED_I2C_SDA_PIN)
#define OLED_SDA_LOW()      GPIO_ResetBits(OLED_I2C_PORT, OLED_I2C_SDA_PIN)

static uint8_t oledBuffer[OLED_BUFFER_SIZE];
static uint8_t oledShadow[OLED_BUFFER_SIZE];
static uint8_t oledForceUpdate = 1;

static const uint8_t digitFont[10][5] =
{
    {0x3E, 0x51, 0x49, 0x45, 0x3E},
    {0x00, 0x42, 0x7F, 0x40, 0x00},
    {0x42, 0x61, 0x51, 0x49, 0x46},
    {0x21, 0x41, 0x45, 0x4B, 0x31},
    {0x18, 0x14, 0x12, 0x7F, 0x10},
    {0x27, 0x45, 0x45, 0x45, 0x39},
    {0x3C, 0x4A, 0x49, 0x49, 0x30},
    {0x01, 0x71, 0x09, 0x05, 0x03},
    {0x36, 0x49, 0x49, 0x49, 0x36},
    {0x06, 0x49, 0x49, 0x29, 0x1E}
};

static const uint8_t upperFont[26][5] =
{
    {0x7E, 0x11, 0x11, 0x11, 0x7E},
    {0x7F, 0x49, 0x49, 0x49, 0x36},
    {0x3E, 0x41, 0x41, 0x41, 0x22},
    {0x7F, 0x41, 0x41, 0x22, 0x1C},
    {0x7F, 0x49, 0x49, 0x49, 0x41},
    {0x7F, 0x09, 0x09, 0x09, 0x01},
    {0x3E, 0x41, 0x49, 0x49, 0x7A},
    {0x7F, 0x08, 0x08, 0x08, 0x7F},
    {0x00, 0x41, 0x7F, 0x41, 0x00},
    {0x20, 0x40, 0x41, 0x3F, 0x01},
    {0x7F, 0x08, 0x14, 0x22, 0x41},
    {0x7F, 0x40, 0x40, 0x40, 0x40},
    {0x7F, 0x02, 0x0C, 0x02, 0x7F},
    {0x7F, 0x04, 0x08, 0x10, 0x7F},
    {0x3E, 0x41, 0x41, 0x41, 0x3E},
    {0x7F, 0x09, 0x09, 0x09, 0x06},
    {0x3E, 0x41, 0x51, 0x21, 0x5E},
    {0x7F, 0x09, 0x19, 0x29, 0x46},
    {0x46, 0x49, 0x49, 0x49, 0x31},
    {0x01, 0x01, 0x7F, 0x01, 0x01},
    {0x3F, 0x40, 0x40, 0x40, 0x3F},
    {0x1F, 0x20, 0x40, 0x20, 0x1F},
    {0x3F, 0x40, 0x38, 0x40, 0x3F},
    {0x63, 0x14, 0x08, 0x14, 0x63},
    {0x07, 0x08, 0x70, 0x08, 0x07},
    {0x61, 0x51, 0x49, 0x45, 0x43}
};

static const uint8_t bigDigitSegments[10] =
{
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static void oled_i2c_delay(void)
{
    volatile uint8_t i;
    for(i = 0; i < 3; i++)
    {
        __NOP();
    }
}

static void oled_i2c_start(void)
{
    OLED_SDA_HIGH();
    OLED_SCL_HIGH();
    oled_i2c_delay();
    OLED_SDA_LOW();
    oled_i2c_delay();
    OLED_SCL_LOW();
}

static void oled_i2c_stop(void)
{
    OLED_SDA_LOW();
    OLED_SCL_HIGH();
    oled_i2c_delay();
    OLED_SDA_HIGH();
    oled_i2c_delay();
}

static uint8_t oled_i2c_write_byte(uint8_t value)
{
    uint8_t bit;
    uint8_t ack;

    for(bit = 0; bit < 8; bit++)
    {
        if(value & 0x80)
        {
            OLED_SDA_HIGH();
        }
        else
        {
            OLED_SDA_LOW();
        }

        oled_i2c_delay();
        OLED_SCL_HIGH();
        oled_i2c_delay();
        OLED_SCL_LOW();
        value <<= 1;
    }

    OLED_SDA_HIGH();
    oled_i2c_delay();
    OLED_SCL_HIGH();
    oled_i2c_delay();
    ack = GPIO_ReadInputDataBit(OLED_I2C_PORT, OLED_I2C_SDA_PIN) == Bit_RESET;
    OLED_SCL_LOW();

    return ack;
}

static uint8_t oled_write(const uint8_t *data, uint16_t length, uint8_t control)
{
    uint16_t index;

    oled_i2c_start();
    if(!oled_i2c_write_byte((uint8_t)(OLED_I2C_ADDRESS << 1)))
    {
        oled_i2c_stop();
        return 0;
    }

    if(!oled_i2c_write_byte(control))
    {
        oled_i2c_stop();
        return 0;
    }

    for(index = 0; index < length; index++)
    {
        if(!oled_i2c_write_byte(data[index]))
        {
            oled_i2c_stop();
            return 0;
        }
    }

    oled_i2c_stop();
    return 1;
}

static const uint8_t *oled_get_glyph(char ch)
{
    static const uint8_t blank[5] = {0, 0, 0, 0, 0};
    static const uint8_t colon[5] = {0x00, 0x36, 0x36, 0x00, 0x00};
    static const uint8_t period[5] = {0x00, 0x60, 0x60, 0x00, 0x00};
    static const uint8_t dash[5] = {0x08, 0x08, 0x08, 0x08, 0x08};
    static const uint8_t slash[5] = {0x20, 0x10, 0x08, 0x04, 0x02};

    if(ch >= '0' && ch <= '9')
    {
        return digitFont[ch - '0'];
    }

    if(ch >= 'A' && ch <= 'Z')
    {
        return upperFont[ch - 'A'];
    }

    switch(ch)
    {
    case ':': return colon;
    case '.': return period;
    case '-': return dash;
    case '/': return slash;
    default: return blank;
    }
}

uint8_t oled_init(void)
{
    GPIO_InitTypeDef gpio;
    static const uint8_t initCommands[] =
    {
        0xAE,
        0x20, 0x02,
        0xB0,
        0xC8,
        0x00,
        0x10,
        0x40,
        0x81, 0x7F,
        0xA1,
        0xA6,
        0xA8, 0x3F,
        0xA4,
        0xD3, 0x00,
        0xD5, 0x80,
        0xD9, 0xF1,
        0xDA, 0x12,
        0xDB, 0x40,
        0x8D, 0x14,
        0xAF
    };

    RCC_APB2PeriphClockCmd(OLED_I2C_RCC, ENABLE);
    gpio.GPIO_Pin = OLED_I2C_SCL_PIN | OLED_I2C_SDA_PIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(OLED_I2C_PORT, &gpio);
    OLED_SCL_HIGH();
    OLED_SDA_HIGH();

    if(!oled_write(initCommands, sizeof(initCommands), 0x00))
    {
        return 0;
    }

    memset(oledBuffer, 0, sizeof(oledBuffer));
    memset(oledShadow, 0xFF, sizeof(oledShadow));
    oledForceUpdate = 1;
    return oled_update();
}

void oled_clear(void)
{
    memset(oledBuffer, 0, sizeof(oledBuffer));
}

uint8_t oled_update(void)
{
    uint8_t page;
    uint8_t commands[3];
    uint16_t offset;

    for(page = 0; page < OLED_PAGE_COUNT; page++)
    {
        offset = (uint16_t)page * OLED_WIDTH;
        if(!oledForceUpdate && memcmp(&oledBuffer[offset], &oledShadow[offset], OLED_WIDTH) == 0)
        {
            continue;
        }

        commands[0] = (uint8_t)(0xB0 | page);
        commands[1] = 0x00;
        commands[2] = 0x10;

        if(!oled_write(commands, sizeof(commands), 0x00))
        {
            return 0;
        }

        if(!oled_write(&oledBuffer[offset], OLED_WIDTH, 0x40))
        {
            return 0;
        }

        memcpy(&oledShadow[offset], &oledBuffer[offset], OLED_WIDTH);
    }

    oledForceUpdate = 0;
    return 1;
}

void oled_draw_pixel(int16_t x, int16_t y, uint8_t color)
{
    uint16_t index;
    uint8_t mask;

    if(x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT)
    {
        return;
    }

    index = (uint16_t)x + ((uint16_t)y / 8U) * OLED_WIDTH;
    mask = (uint8_t)(1U << ((uint16_t)y & 7U));

    if(color)
    {
        oledBuffer[index] |= mask;
    }
    else
    {
        oledBuffer[index] &= (uint8_t)~mask;
    }
}

void oled_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color)
{
    int16_t dx = x1 > x0 ? x1 - x0 : x0 - x1;
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t dy = y1 > y0 ? y0 - y1 : y1 - y0;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t error = dx + dy;
    int16_t error2;

    while(1)
    {
        oled_draw_pixel(x0, y0, color);
        if(x0 == x1 && y0 == y1)
        {
            break;
        }

        error2 = (int16_t)(2 * error);
        if(error2 >= dy)
        {
            error += dy;
            x0 += sx;
        }
        if(error2 <= dx)
        {
            error += dx;
            y0 += sy;
        }
    }
}

void oled_draw_rect(int16_t x, int16_t y, int16_t width, int16_t height, uint8_t color)
{
    if(width <= 0 || height <= 0)
    {
        return;
    }

    oled_draw_line(x, y, x + width - 1, y, color);
    oled_draw_line(x, y + height - 1, x + width - 1, y + height - 1, color);
    oled_draw_line(x, y, x, y + height - 1, color);
    oled_draw_line(x + width - 1, y, x + width - 1, y + height - 1, color);
}

void oled_fill_rect(int16_t x, int16_t y, int16_t width, int16_t height, uint8_t color)
{
    int16_t row;

    for(row = 0; row < height; row++)
    {
        oled_draw_line(x, y + row, x + width - 1, y + row, color);
    }
}

void oled_draw_char(int16_t x, int16_t y, char ch, uint8_t color)
{
    const uint8_t *glyph = oled_get_glyph(ch);
    uint8_t column;
    uint8_t row;

    for(column = 0; column < 5; column++)
    {
        for(row = 0; row < 7; row++)
        {
            if(glyph[column] & (1U << row))
            {
                oled_draw_pixel(x + column, y + row, color);
            }
        }
    }
}

void oled_draw_text(int16_t x, int16_t y, const char *text, uint8_t color)
{
    while(text != 0 && *text != 0 && x <= OLED_WIDTH - 5)
    {
        oled_draw_char(x, y, *text, color);
        x += 6;
        text++;
    }
}

void oled_draw_big_digit(int16_t x, int16_t y, uint8_t digit, uint8_t color)
{
    uint8_t segments;

    if(digit > 9)
    {
        return;
    }

    segments = bigDigitSegments[digit];
    if(segments & 0x01) oled_fill_rect(x + 2, y, 9, 2, color);
    if(segments & 0x02) oled_fill_rect(x + 11, y + 2, 2, 8, color);
    if(segments & 0x04) oled_fill_rect(x + 11, y + 12, 2, 8, color);
    if(segments & 0x08) oled_fill_rect(x + 2, y + 20, 9, 2, color);
    if(segments & 0x10) oled_fill_rect(x, y + 12, 2, 8, color);
    if(segments & 0x20) oled_fill_rect(x, y + 2, 2, 8, color);
    if(segments & 0x40) oled_fill_rect(x + 2, y + 10, 9, 2, color);
}
