/****************************************************************************
* ��  ��: A11, A12(����)
* ��  ��: B11
* ��ˮ��:
*   ��ࣺB12, B13, B14, B15
*   �ҲࣺA4, A5, A6, A7
*****************************************************************************/
#include "motor.h"
#include "delay.h"

static uint8_t leftMotorRunning = 0;
static uint8_t rightMotorRunning = 0;
/****************************************************************************
* ��  ��: motor_init
* ��  ��: ��
* ��  ��: ��
* ��  ��: ��
* ��  ��: ����������ų�ʼ��
*****************************************************************************/
void motor_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(MOTOR_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;            // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;           // IO���ٶ�Ϊ50MHz

    // PA�˿�����
    GPIO_InitStructure.GPIO_Pin = L_MOTOR_PIN | R_MOTOR_PIN;    // �������ҵ��
    GPIO_Init(MOTOR_PORT, &GPIO_InitStructure);

	  LMOTOR_OFF;                 // close left motor
    RMOTOR_OFF;                 // close right motor
    leftMotorRunning = 0;
    rightMotorRunning = 0;
}

void motor_run(void)
{
    LMOTOR_ON;                  // run left motor
    RMOTOR_ON;                  // run right motor
    leftMotorRunning = 1;
    rightMotorRunning = 1;
}

void motor_stop(void)
{
    LMOTOR_OFF;                 // close left motor
    RMOTOR_OFF;                 // close right motor
    leftMotorRunning = 0;
    rightMotorRunning = 0;
}

void motor_leftOnly(void)
{
    LMOTOR_OFF;                 // stop left motor
    RMOTOR_ON;                  // run right motor
    leftMotorRunning = 0;
    rightMotorRunning = 1;
}

void motor_rightOnly(void)
{
    LMOTOR_ON;                  // run left motor
    RMOTOR_OFF;                 // stop right motor
    leftMotorRunning = 1;
    rightMotorRunning = 0;
}

uint8_t motor_leftRunning(void)
{
    return leftMotorRunning;
}

uint8_t motor_rightRunning(void)
{
    return rightMotorRunning;
}