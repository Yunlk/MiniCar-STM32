#ifndef __CAR_H
#define __CAR_H

#include "stm32f10x.h"											//uint8_t为在STM32中定义

/************************** 小车结构体 *****************************/
typedef struct {
    uint8_t status;             // 小车状态：启动、停止、运行、加速、减速等；
    uint8_t mode;               // 小车模式：自动驾驶、人工控制
    uint8_t speed;              // 小车速度
    uint8_t lane;               // 小车车道：车道0、车道1、车道2
    int x;                      // 小车坐标：x坐标
    int y;                      // 小车坐标：y坐标
} __CAR;

/*************************** 全局变量 ******************************/
extern __CAR car;               // 仿真车
extern __CAR obs;               // 障碍车

/*************************** 函数声明 ******************************/
void car_init(__CAR * pCar);                        // 小车初始化
void car_turnLeft(__CAR *pCar);                     // 小车往左切换道路
void car_turnRight(__CAR *pCar);                    // 小车往右切换道路
void car_autoDrive(__CAR *pCar, __CAR *pObs);       // 小车自动驾驶策略
void car_updatePos(__CAR *pCar);                    // 小车状态改变
void car_cancelAutoDrive(__CAR *pCar);               // 取消自动换道目标

#endif
