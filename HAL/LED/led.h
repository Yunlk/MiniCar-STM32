#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

// 两侧大灯，引脚定义
#define QDLED_PORT      GPIOA

#define ZQLED_PIN       GPIO_Pin_4           // 左前大灯
#define YQLED_PIN       GPIO_Pin_5           // 右前大灯

// 左前流水灯，引脚定义
#define ZxLED_PORT      GPIOB

#define Z1LED_PIN       GPIO_Pin_4          // 左前灯一
#define Z2LED_PIN       GPIO_Pin_5          // 左前灯二
#define Z3LED_PIN       GPIO_Pin_6          // 左前灯三
#define Z4LED_PIN       GPIO_Pin_7          // 左前灯四
#define Z5LED_PIN       GPIO_Pin_8          // 左前灯五

// 右前流水灯，引脚定义
#define YxLED_PORT       GPIOA

#define Y1LED_PIN       GPIO_Pin_12          // 右前灯一
#define Y2LED_PIN       GPIO_Pin_11          // 右前灯二
#define Y3LED_PIN       GPIO_Pin_10          // 右前灯三
#define Y4LED_PIN       GPIO_Pin_9           // 右前灯四
#define Y5LED_PIN       GPIO_Pin_8           // 右前灯五

#define ZQLED_ON        GPIO_ResetBits(QDLED_PORT, ZQLED_PIN);      // 开启左前大灯
#define YQLED_ON        GPIO_ResetBits(QDLED_PORT, YQLED_PIN);      // 开启右前大灯
#define ZQLED_OFF       GPIO_SetBits(QDLED_PORT, ZQLED_PIN);        // 关闭左前大灯
#define YQLED_OFF       GPIO_SetBits(QDLED_PORT, YQLED_PIN);        // 关闭右前大灯

#define Y1LED_ON        GPIO_SetBits(YxLED_PORT, Y1LED_PIN);        // 开启右前灯一
#define Y2LED_ON        GPIO_SetBits(YxLED_PORT, Y2LED_PIN);        // 开启右前灯二
#define Y3LED_ON        GPIO_SetBits(YxLED_PORT, Y3LED_PIN);        // 开启右前灯三
#define Y4LED_ON        GPIO_SetBits(YxLED_PORT, Y4LED_PIN);        // 开启右前灯四
#define Y5LED_ON        GPIO_SetBits(YxLED_PORT, Y5LED_PIN);        // 开启右前灯五

#define Y1LED_OFF       GPIO_ResetBits(YxLED_PORT, Y1LED_PIN);      // 关闭右前灯一
#define Y2LED_OFF       GPIO_ResetBits(YxLED_PORT, Y2LED_PIN);      // 关闭右前灯二
#define Y3LED_OFF       GPIO_ResetBits(YxLED_PORT, Y3LED_PIN);      // 关闭右前灯三
#define Y4LED_OFF       GPIO_ResetBits(YxLED_PORT, Y4LED_PIN);      // 关闭右前灯四
#define Y5LED_OFF       GPIO_ResetBits(YxLED_PORT, Y5LED_PIN);      // 关闭右前灯五

#define Z1LED_ON        GPIO_SetBits(ZxLED_PORT, Z1LED_PIN);        // 开启左前灯一
#define Z2LED_ON        GPIO_SetBits(ZxLED_PORT, Z2LED_PIN);        // 开启左前灯二
#define Z3LED_ON        GPIO_SetBits(ZxLED_PORT, Z3LED_PIN);        // 开启左前灯三
#define Z4LED_ON        GPIO_SetBits(ZxLED_PORT, Z4LED_PIN);        // 开启左前灯四
#define Z5LED_ON        GPIO_SetBits(ZxLED_PORT, Z5LED_PIN);        // 开启左前灯五

#define Z1LED_OFF       GPIO_ResetBits(ZxLED_PORT, Z1LED_PIN);      // 关闭左前灯一
#define Z2LED_OFF       GPIO_ResetBits(ZxLED_PORT, Z2LED_PIN);      // 关闭左前灯二
#define Z3LED_OFF       GPIO_ResetBits(ZxLED_PORT, Z3LED_PIN);      // 关闭左前灯三
#define Z4LED_OFF       GPIO_ResetBits(ZxLED_PORT, Z4LED_PIN);      // 关闭左前灯四
#define Z5LED_OFF       GPIO_ResetBits(ZxLED_PORT, Z5LED_PIN);      // 关闭左前灯五

#define ALL_LED         0xff
#define LED0            0
#define LAMP_ON         0
#define LAMP_OFF        1

enum led_side
{
    BOTH_SIDE   = 0,
    ZUO_LED     = 1,
    YOU_LED     = 2,
};

void led_init(void);
void ledx_on(enum led_side side, uint8_t index);
void ledx_off(enum led_side side, uint8_t index);
void lamp_on(enum led_side side);           // 开启大灯
void lamp_off(enum led_side side);          // 关闭大灯

#endif
