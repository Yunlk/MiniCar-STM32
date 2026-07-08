/************************************************************************
* 大  灯: A4, A5(左、右)
* 电  机: A6, A7(左、右)
* 喇  叭: C13
* 按  键: C14（模式）, C15（鸣笛）, B1（左转）, B0（右转）
* 流水灯: 
*   左侧：B4, B5, B6, B7, B8；
*   右侧：A12,A11,A10,A9, A8；
* 串  口: Tx: A2, Rx: A3 (UART2)
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
    delay_init();           // 延时时钟初始化
    buzzer_init();          // 蜂鸣器初始化
    key_init();             // 按键初始化
    led_init();             // LED 初始化
    motor_init();           // 电机初始化
    uart_init(COM_BAUD);    // 串口初始化
    
    car_init(&car);         // 汽车初始化
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
    start();    // 初始化完成，闪灯、鸣笛
		motor_run();        // 两侧电机转动
	
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
				
				
        if(mode == CAR_AUTO)                //自动驾驶模式
        {
            car_autoDrive(&car, &obs);      //自动避障
            car_updatePos(&car);            //更新车辆位置
            uart_sendCarInfo(&car);         //向仿真端发送本车位置
        }
        else                                //手动驾驶模式
        {
            if(TL_RKEY_PRESSED)              //左转
            {
                car_turnLeft(&car);
								leftTurnFlag = 1;
            }

            if(TR_RKEY_PRESSED)              //右转
            {
                 car_turnRight(&car);
								 rightTurnFlag = 1;
            }

            uart_sendCarInfo(&car);         //向仿真端发送本车位置
        }

        if(leftTurnFlag)
        {
            leftTurnFlag = 0;
            
            // 左侧流水灯循环点亮
            for(i = 0; i < 10; i++)
            {
                ledx_off(ZUO_LED, ALL_LED);
                ledx_on(ZUO_LED, i % 5 + 1);
                delay_ms(50);
            }
            ledx_off(BOTH_SIDE, ALL_LED);   // 两侧流水灯熄灭
            
            motor_rightOnly();  // 右侧电机转动，左侧电机停止
            delay_ms(500);
            motor_run();        // 两侧电机转动
        }
        
        if(rightTurnFlag)
        {
            rightTurnFlag = 0;
            
            // 左侧流水灯循环点亮
            for(i = 0; i < 10; i++)
            {
                ledx_off(YOU_LED, ALL_LED);
                ledx_on(YOU_LED, i % 5 + 1);
                delay_ms(50);
            }
            ledx_off(BOTH_SIDE, ALL_LED);   // 两侧流水灯熄灭
            
            motor_leftOnly();  // 右侧电机转动，左侧电机停止
            delay_ms(500);
            motor_run();        // 两侧电机转动
        }

        delay_ms(CYCLE_TIME);
    }
}
