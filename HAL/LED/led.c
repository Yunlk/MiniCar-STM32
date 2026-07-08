/************************************************************************
*   名称: led.c
*   日期: 2024-6-6
*   作者: wtltom
*   描述：
*       通过控制指定引脚的输出电平，控制LED灯的开关。
*************************************************************************/
#include "led.h"

#define LAMP_NUM    2
#define ZLED_NUM    5
#define YLED_NUM    5

uint16_t lamp_pins[LAMP_NUM] =
{
    ZQLED_PIN,
    YQLED_PIN
};

uint16_t zled_pins[ZLED_NUM] = 
{
    Z1LED_PIN,
    Z2LED_PIN,
    Z3LED_PIN,
    Z4LED_PIN,
    Z5LED_PIN
};

uint16_t yled_pins[YLED_NUM] = 
{
    Y1LED_PIN,
    Y2LED_PIN,
    Y3LED_PIN,
    Y4LED_PIN,
    Y5LED_PIN
};

void zled_on(uint8_t index);         // 开启左侧 LED
void yled_on(uint8_t index);         // 开启右侧 LED
void zled_off(uint8_t index);        // 关闭左侧 LED
void yled_off(uint8_t index);        // 关闭右侧 LED

void led_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = ZQLED_PIN | YQLED_PIN;
    GPIO_Init(QDLED_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = Z1LED_PIN | Z2LED_PIN | Z3LED_PIN | Z4LED_PIN | Z5LED_PIN;
    GPIO_Init(ZxLED_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = Y1LED_PIN | Y2LED_PIN | Y3LED_PIN | Y4LED_PIN | Y5LED_PIN;
    GPIO_Init(YxLED_PORT, &GPIO_InitStructure);
    
    ledx_off(BOTH_SIDE, ALL_LED);
}

void ledx_on(enum led_side side, uint8_t index)
{
    if(side == BOTH_SIDE)
    {
        zled_on(index);
        yled_on(index);
    }
    else if(side == ZUO_LED)
    {
        zled_on(index);
    }
    else if(side == YOU_LED)
    {
        yled_on(index);
    }
}

void ledx_off(enum led_side side, uint8_t index)
{
    if(side == BOTH_SIDE)
    {
        zled_off(index);
        yled_off(index);
    }
    else if(side == ZUO_LED)
    {
        zled_off(index);
    }
    else if(side == YOU_LED)
    {
        yled_off(index);
    }
}

void lamp_on(enum led_side side)
{
    if(side == BOTH_SIDE)
    {
        ZQLED_ON;
        YQLED_ON;
    }
    else if(side == ZUO_LED)
    {
        ZQLED_ON;
    }
    else if(side == YOU_LED)
    {
        YQLED_ON;
    }
}

void lamp_off(enum led_side side)
{
    if(side == BOTH_SIDE)
    {
        ZQLED_OFF;
        YQLED_OFF;
    }
    else if(side == ZUO_LED)
    {
        ZQLED_OFF;
    }
    else if(side == YOU_LED)
    {
        YQLED_OFF;
    }
}

void zled_on(uint8_t index)
{
    uint8_t i = 0;
    
    if(index == ALL_LED)
    {
        GPIO_ResetBits(QDLED_PORT, ZQLED_PIN);
        
        for(i = 0; i < ZLED_NUM; i++)
        {
            GPIO_SetBits(ZxLED_PORT, zled_pins[i]);
        }
    }
    else
    {
        if(index == LED0)
        {
            GPIO_ResetBits(QDLED_PORT, ZQLED_PIN);
        }
        else
        {
            i = index - 1;
            GPIO_SetBits(ZxLED_PORT, zled_pins[i]);
        }
    }
}

void yled_on(uint8_t index)
{
    uint8_t i = 0;
    
    if(index == ALL_LED)
    {
        GPIO_ResetBits(QDLED_PORT, YQLED_PIN);
        
        for(i = 0; i < YLED_NUM; i++)
        {
            GPIO_SetBits(YxLED_PORT, yled_pins[i]);
        }
    }
    else
    {
        if(index == LED0)
        {
            GPIO_ResetBits(QDLED_PORT, YQLED_PIN);
        }
        else
        {
            i = index - 1;
            GPIO_SetBits(YxLED_PORT, yled_pins[i]);
        }
    }
}

void zled_off(uint8_t index)
{
    uint8_t i = 0;
    
    if(index == ALL_LED)
    {
        GPIO_SetBits(QDLED_PORT, ZQLED_PIN);
        
        for(i = 0; i < ZLED_NUM; i++)
        {
            GPIO_ResetBits(ZxLED_PORT, zled_pins[i]);
        }
    }
    else
    {
        if(index == LED0)
        {
            GPIO_SetBits(QDLED_PORT, ZQLED_PIN);
        }
        else
        {
            i = index - 1;
            GPIO_ResetBits(ZxLED_PORT, zled_pins[i]);
        }
    }
}

void yled_off(uint8_t index)
{
    uint8_t i = 0;
    
    if(index == ALL_LED)
    {
        GPIO_SetBits(QDLED_PORT, YQLED_PIN);
        
        for(i = 0; i < YLED_NUM; i++)
        {
            GPIO_ResetBits(YxLED_PORT, yled_pins[i]);
        }
    }
    else
    {
        if(index == LED0)
        {
            GPIO_SetBits(QDLED_PORT, YQLED_PIN);
        }
        else
        {
            i = index - 1;
            GPIO_ResetBits(YxLED_PORT, yled_pins[i]);
        }
    }
}
