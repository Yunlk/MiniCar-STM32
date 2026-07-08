#ifndef __GPIO_H
#define __GPIO_H	 

#include "stm32f10x.h"

#define MOTOR_RCC       RCC_APB2Periph_GPIOA
#define MOTOR_PORT      GPIOA

#define L_MOTOR_PIN     GPIO_Pin_6
#define R_MOTOR_PIN     GPIO_Pin_7 

#define LMOTOR_ON       GPIO_SetBits(MOTOR_PORT, L_MOTOR_PIN)         // 启动左电机
#define LMOTOR_OFF      GPIO_ResetBits(MOTOR_PORT, L_MOTOR_PIN)       // 停止左电机

#define RMOTOR_ON       GPIO_SetBits(MOTOR_PORT, R_MOTOR_PIN)         // 启动左电机
#define RMOTOR_OFF      GPIO_ResetBits(MOTOR_PORT, R_MOTOR_PIN)       // 停止右电机

void motor_init(void);      //初始化
void motor_run(void);
void motor_stop(void);
void motor_leftOnly(void);
void motor_rightOnly(void);

#endif
