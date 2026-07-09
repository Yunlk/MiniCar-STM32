#ifndef __GPIO_H
#define __GPIO_H	 

#include "stm32f10x.h"

#define MOTOR_RCC       RCC_APB2Periph_GPIOA
#define MOTOR_PORT      GPIOA

#define L_MOTOR_PIN     GPIO_Pin_6
#define R_MOTOR_PIN     GPIO_Pin_7 

#define LMOTOR_ON       GPIO_SetBits(MOTOR_PORT, L_MOTOR_PIN)         // ��������
#define LMOTOR_OFF      GPIO_ResetBits(MOTOR_PORT, L_MOTOR_PIN)       // ֹͣ����

#define RMOTOR_ON       GPIO_SetBits(MOTOR_PORT, R_MOTOR_PIN)         // ��������
#define RMOTOR_OFF      GPIO_ResetBits(MOTOR_PORT, R_MOTOR_PIN)       // ֹͣ�ҵ��

void motor_init(void);      //��ʼ��
void motor_run(void);
void motor_stop(void);
void motor_leftOnly(void);
void motor_rightOnly(void);
uint8_t motor_leftRunning(void);
uint8_t motor_rightRunning(void);
#endif
