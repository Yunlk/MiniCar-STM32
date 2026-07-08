/****************************************************************************
* 电  机: A11, A12(左、右)
* 喇  叭: B11
* 流水灯: 
*   左侧：B12, B13, B14, B15
*   右侧：A4, A5, A6, A7
*****************************************************************************/
#include "motor.h"
#include "delay.h"

/****************************************************************************
* 名  称: motor_init
* 输  入: 无
* 输  出: 无
* 返  回: 无
* 功  能: 电机控制引脚初始化
*****************************************************************************/
void motor_init(void)
{ 
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(MOTOR_RCC, ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;            // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;           // IO口速度为50MHz

    // PA端口配置
    GPIO_InitStructure.GPIO_Pin = L_MOTOR_PIN | R_MOTOR_PIN;    // 左电机、右电机
    GPIO_Init(MOTOR_PORT, &GPIO_InitStructure);
	
	  LMOTOR_OFF;                 // 关闭左侧电机
    RMOTOR_OFF;                 // 关闭右侧电机
}

void motor_run(void)
{
    LMOTOR_ON;                  // 启动左侧电机
    RMOTOR_ON;                  // 启动右侧电机
}

void motor_stop(void)
{
    LMOTOR_OFF;                 // 关闭左侧电机
    RMOTOR_OFF;                 // 关闭右侧电机
}

void motor_leftOnly(void)
{
    LMOTOR_OFF;                 // 停止左侧电机
    RMOTOR_ON;                  // 启动右侧电机
}

void motor_rightOnly(void)
{
    LMOTOR_ON;                  // 启动左侧电机
    RMOTOR_OFF;                 // 停止右侧电机
}
