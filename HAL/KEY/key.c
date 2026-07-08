#include "key.h"
#include "delay.h"

uint8_t statusArray[4] = { KEY_NONE }; 
uint8_t pressCounters[4] = { 0 }; 

void key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(LKEY_RCC | RKEY_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = BP_LKEY_PIN | MD_LKEY_PIN;
    GPIO_Init(LKEY_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = TR_RKEY_PIN | TL_RKEY_PIN;
    GPIO_Init(RKEY_PORT, &GPIO_InitStructure);
}

/*********************************************************
* 名  称：get_index
* 参  数：key_port - 按键所在的GPIO端口（如LKEY_PORT或RKEY_PORT）
*         key_pin  - 按键对应的引脚号（如MD_LKEY_PIN、BP_LKEY_PIN等）
* 功  能：将物理按键映射为状态数组的索引值
* 返  回：
*   0 ：模式开关(MD_LKEY_PIN)对应的索引
*   1 : 鸣笛开关(BP_LKEY_PIN)对应的索引
*   2 : 左转开关(TL_RKEY_PIN)对应的索引
*   3 : 右转开关(TR_RKEY_PIN)对应的索引
*   默认返回0（无效引脚时）
**********************************************************/
uint8_t get_index(GPIO_TypeDef* key_port, uint16_t key_pin)
{
    uint8_t ret = 0;
    
    if(key_port == LKEY_PORT)
    {
        switch(key_pin)
        {
            case MD_LKEY_PIN:
                ret = 0;
                break;
            case BP_LKEY_PIN:
                ret = 1;
                break;
            default:
                break;
        }
    }
    else if(key_port == RKEY_PORT)
    {
        switch(key_pin)
        {
            case TL_RKEY_PIN:
                ret = 2;
                break;
            case TR_RKEY_PIN:
                ret = 3;
                break;
            default:
                break;
        }
    }
    
    return ret;
}

/*********************************************************
* 名  称：get_key
* 参  数：key_pin 引脚号
* 功  能：判断左转，右转键有没有被按下
* 返  回：
*   0 ：没有键被按下
*   1 : key_pin引脚对应的按键被按下了
*   2 : key_pin引脚对应的按键被按下了
**********************************************************/
enum status get_key(GPIO_TypeDef* key_port, uint16_t key_pin)
{
    uint8_t keyIndex = get_index(key_port, key_pin);
    uint8_t lastStatus = statusArray[keyIndex];
    uint8_t pressCount = pressCounters[keyIndex];
    
    enum status ret = KEY_NONE;
    
    if (GPIO_ReadInputDataBit(key_port, key_pin) == 0)
    {
        if(lastStatus == KEY_NONE)
        {
            delay_ms(40);           // 按键去抖，防止短时间内多次检测到电平变化
            
            ret =  KEY_ONPRESS;
            pressCount = 4;
        }
        else
        {
            if(pressCount > 100)
            {
                ret =  KEY_LONG_PRESS;
            }
            else
            {
                ret =  KEY_ONPRESS;
                pressCount++;
            }
            
        }
    }
    else
    {
        if(lastStatus == KEY_ONPRESS || lastStatus == KEY_LONG_PRESS)
        {
            pressCount = 0;
        
            if(lastStatus == KEY_ONPRESS)
            {
                ret =  KEY_PRESSED;
            }
            
            delay_ms(40);           // 按键去抖，防止短时间内多次检测到电平变化
        }
        else
        {
            ret =  KEY_NONE;
        }
    }
    
    // 保存为上一次的数据
    statusArray[keyIndex] = (uint8_t)ret;
    pressCounters[keyIndex] = pressCount;
    
    return ret;
}
