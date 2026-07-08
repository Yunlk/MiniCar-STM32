#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define LKEY_RCC    RCC_APB2Periph_GPIOC
#define RKEY_RCC    RCC_APB2Periph_GPIOB

#define LKEY_PORT       GPIOC
#define RKEY_PORT       GPIOB

#define MD_LKEY_PIN     GPIO_Pin_14      // 模式开关
#define BP_LKEY_PIN     GPIO_Pin_15      // 鸣笛开关
#define TL_RKEY_PIN     GPIO_Pin_0       // 左转开关
#define TR_RKEY_PIN     GPIO_Pin_1       // 右转开关

#define BP_LKEY_PRESSED     (get_key(LKEY_PORT, BP_LKEY_PIN) == KEY_PRESSED)    // 鸣笛开关按下
#define MD_LKEY_PRESSED     (get_key(LKEY_PORT, MD_LKEY_PIN) == KEY_PRESSED)    // 模式开关按下
#define TL_RKEY_PRESSED     (get_key(RKEY_PORT, TL_RKEY_PIN) == KEY_PRESSED)    // 左转开关按下
#define TR_RKEY_PRESSED     (get_key(RKEY_PORT, TR_RKEY_PIN) == KEY_PRESSED)    // 右转开关按下

#define BP_LKEY_LONG_PRESS  (get_key(LKEY_PORT, BP_LKEY_PIN) == KEY_LONG_PRESS) // 鸣笛开关长按

enum status
{
    KEY_NONE        = 0,
    KEY_ONPRESS     = 1,
    KEY_LONG_PRESS  = 2,
    KEY_PRESSED     = 3,
};

void key_init(void);
enum status get_key(GPIO_TypeDef*, uint16_t key_pin);

#endif
