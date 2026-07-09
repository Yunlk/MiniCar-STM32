/************************************************************************
* ��  ��: A4, A5(����)
* ��  ��: A6, A7(����)
* ��  ��: C13
* ��  ��: C14��ģʽ��, C15�����ѣ�, B1����ת��, B0����ת��
* ��ˮ��:
*   ��ࣺB4, B5, B6, B7, B8��
*   �ҲࣺA12,A11,A10,A9, A8��
* ��  ��: Tx: A2, Rx: A3 (UART2)
*************************************************************************/
#include "command.h"
#include "beep.h"
#include "car.h"
#include "delay.h"
#include "key.h"
#include "led.h"
#include "uart.h"
#include "motor.h"

uint8_t mode = CAR_MANUAL;
uint8_t leftTurnFlag = 0;
uint8_t rightTurnFlag = 0;
uint8_t lampStatus = LAMP_OFF;

void init(void)
{
    delay_init();           // ��ʱʱ�ӳ�ʼ��
    buzzer_init();          // ��������ʼ��
    key_init();             // ������ʼ��
    led_init();             // LED ��ʼ��
    motor_init();           // �����ʼ��
    uart_init(COM_BAUD);    // ���ڳ�ʼ��

    car_init(&car);         // ������ʼ��
    delay_ms(100);
}

void start(void)
{
    uint8_t i = 0;
    for(i = 0; i < 15; i++)
    {
        ledx_off(BOTH_SIDE, ALL_LED);
        ledx_on(BOTH_SIDE, i % 5 + 1);
        delay_ms(200);
    }

    ledx_off(BOTH_SIDE, ALL_LED);

    buzzer_beep();
}


int main(void)
{
    uint8_t i = 0;
    init();
    start();    // ��ʼ����ɣ����ơ�����
		motor_run();        // run both motors
    car.status = CAR_RUN;

    while(1)
    {
        if(MD_LKEY_PRESSED)
        {
            if(mode == CAR_MANUAL)
            {
                mode = CAR_AUTO;
            }
            else
            {
                mode = CAR_MANUAL;
            }
        }
        car.mode = mode;

        if(BP_LKEY_PRESSED)
        {
            buzzer_beep();
        }

        if(BP_LKEY_LONG_PRESS)
        {
            if(lampStatus == LAMP_OFF)
            {
                lampStatus = LAMP_ON;
                lamp_on(BOTH_SIDE);
            }
            else
            {
                lampStatus = LAMP_OFF;
                lamp_off(BOTH_SIDE);
            }
        }


        if(mode == CAR_AUTO)                //�Զ���ʻģʽ
        {
            car_autoDrive(&car, &obs);      //�Զ�����
            car_updatePos(&car);            //���³���λ��
            uart_sendCarInfo(&car);         //�����˷��ͱ���λ��
        }
        else                                //�ֶ���ʻģʽ
        {
            if(TL_RKEY_PRESSED)              //��ת
            {
                car_turnLeft(&car);
								leftTurnFlag = 1;
            }

            if(TR_RKEY_PRESSED)              //��ת
            {
                 car_turnRight(&car);
								 rightTurnFlag = 1;
            }

            uart_sendCarInfo(&car);         //�����˷��ͱ���λ��
        }

        if(leftTurnFlag)
        {
            leftTurnFlag = 0;

            // �����ˮ��ѭ������
            for(i = 0; i < 10; i++)
            {
                ledx_off(ZUO_LED, ALL_LED);
                ledx_on(ZUO_LED, i % 5 + 1);
                delay_ms(50);
            }
            ledx_off(BOTH_SIDE, ALL_LED);   // ������ˮ��Ϩ��

            motor_rightOnly();  // �Ҳ���ת���������ֹͣ
            delay_ms(500);
            motor_run();        // run both motors
    car.status = CAR_RUN;
        }

        if(rightTurnFlag)
        {
            rightTurnFlag = 0;

            // �����ˮ��ѭ������
            for(i = 0; i < 10; i++)
            {
                ledx_off(YOU_LED, ALL_LED);
                ledx_on(YOU_LED, i % 5 + 1);
                delay_ms(50);
            }
            ledx_off(BOTH_SIDE, ALL_LED);   // ������ˮ��Ϩ��

            motor_leftOnly();  // �Ҳ���ת���������ֹͣ
            delay_ms(500);
            motor_run();        // run both motors
    car.status = CAR_RUN;
        }

        delay_ms(CYCLE_TIME);
    }
}
