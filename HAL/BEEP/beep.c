#include "beep.h"
#include "delay.h"

/****************************************************************************
* 名  称: buzzer_init
* 输  入: 无
* 输  出: 无
* 返  回: 无
* 功  能: 蜂鸣器引脚初始化
*****************************************************************************/
void buzzer_init(void)
{ 
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(BUZZER_RCC, ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;            // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;           // IO口速度为50MHz

    // 蜂鸣器引脚配置
    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;                   // 喇叭
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);
    
    BUZZER_OFF;
}

void buzzer_beep(void)
{
    uint8_t i = 0;
    for(i = 0; i < 3; i++)
    {
        BUZZER_ON;
        delay_ms(200);
        BUZZER_OFF;
        delay_ms(200);
    }
}
