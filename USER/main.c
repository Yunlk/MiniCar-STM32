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
#include "dashboard.h"

uint8_t leftTurnFlag = 0;
uint8_t rightTurnFlag = 0;
uint8_t lampStatus = LAMP_OFF;

static void showModeStatus(uint8_t mode)
{
    ledx_off(BOTH_SIDE, 1);
    ledx_off(BOTH_SIDE, 3);

    if(mode == CAR_AUTO)
    {
        ledx_on(BOTH_SIDE, 3);
    }
    else
			{
        ledx_on(BOTH_SIDE, 1);
    }
}
void init(void)
{
    delay_init();           // ��ʱʱ�ӳ�ʼ��
    buzzer_init();          // ��������ʼ��
    key_init();             // ������ʼ��
    led_init();             // LED ��ʼ��
    motor_init();           // �����ʼ��?
    uart_init(COM_BAUD);    // ���ڳ�ʼ��

    car_init(&car);         // ������ʼ��
    delay_ms(100);
    dashboard_init();       // SSD1306 dashboard on PB10/PB11
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
    uint8_t modeChanged = 0;
    uint8_t modeKeyHeld = 0;
    uint8_t leftKeyHeld = 0;
    uint8_t rightKeyHeld = 0;
    uint8_t lampLongHandled = 0;
    enum status modeKey = KEY_NONE;
    enum status beepKey = KEY_NONE;
    enum status leftKey = KEY_NONE;
    enum status rightKey = KEY_NONE;

    init();
    start();    // ��ʼ����ɣ���ˮ�ơ���������ʾ
    motor_run();
    car.status = CAR_RUN;

    while(1)
    {
        modeChanged = 0;
        modeKey = get_key(LKEY_PORT, MD_LKEY_PIN);
        beepKey = get_key(LKEY_PORT, BP_LKEY_PIN);
        leftKey = get_key(RKEY_PORT, TL_RKEY_PIN);
        rightKey = get_key(RKEY_PORT, TR_RKEY_PIN);

        if(modeKey == KEY_ONPRESS && !modeKeyHeld)
        {
            if(car.mode == CAR_MANUAL)
            {
                car.mode = CAR_AUTO;
            }
            else
            {
                car.mode = CAR_MANUAL;
                car_cancelAutoDrive(&car);
            }
            modeKeyHeld = 1;
            modeChanged = 1;
        }
        else if(modeKey == KEY_NONE)
        {
            modeKeyHeld = 0;
        }

        if(modeChanged)
        {
            showModeStatus(car.mode);
            uart_sendCarInfoNow(&car);
        }

        if(beepKey == KEY_PRESSED)
        {
            buzzer_beep();
        }

        if(beepKey == KEY_LONG_PRESS && !lampLongHandled)
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
            lampLongHandled = 1;
        }
        else if(beepKey == KEY_NONE)
        {
            lampLongHandled = 0;
        }

        if(car.mode == CAR_AUTO)
        {
            car_autoDrive(&car, &obs);
            car_updatePos(&car);
            uart_sendCarInfo(&car);
        }
        else
        {
            car_updatePos(&car);

            if(leftKey == KEY_ONPRESS && !leftKeyHeld)
            {
                car_turnLeft(&car);
                uart_sendCarInfoNow(&car);
                leftTurnFlag = 1;
                leftKeyHeld = 1;
            }
            else if(leftKey == KEY_NONE)
            {
                leftKeyHeld = 0;
            }

            if(rightKey == KEY_ONPRESS && !rightKeyHeld)
            {
                car_turnRight(&car);
                uart_sendCarInfoNow(&car);
                rightTurnFlag = 1;
                rightKeyHeld = 1;
            }
            else if(rightKey == KEY_NONE)
            {
                rightKeyHeld = 0;
            }

            uart_sendCarInfo(&car);
        }

        if(leftTurnFlag)
        {
            leftTurnFlag = 0;

            for(i = 0; i < 10; i++)
            {
                ledx_off(ZUO_LED, ALL_LED);
                ledx_on(ZUO_LED, i % 5 + 1);
                delay_ms(50);
            }
            ledx_off(BOTH_SIDE, ALL_LED);

            motor_rightOnly();
            delay_ms(500);
            motor_run();
            car.status = CAR_RUN;
        }

        if(rightTurnFlag)
        {
            rightTurnFlag = 0;

            for(i = 0; i < 10; i++)
            {
                ledx_off(YOU_LED, ALL_LED);
                ledx_on(YOU_LED, i % 5 + 1);
                delay_ms(50);
            }
            ledx_off(BOTH_SIDE, ALL_LED);

            motor_leftOnly();
            delay_ms(500);
            motor_run();
            car.status = CAR_RUN;
        }

        dashboard_task(&car);
        delay_ms(CYCLE_TIME);
    }
}
