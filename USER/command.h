/**
  ******************************************************************************
  * @file    command.h
  * @author  东软
  * @version V1.0.0
  * @date    11-March-2023
    */

#ifndef __COMMAND_H
#define __COMMAND_H

#include "stm32f10x.h"

/************************指令列*********************************/
#define CYCLE_TIME              10                             //循环周期 单位：ms
#define Soft_command_beging     '%'                             //起始符      指令格式：%********$
#define Soft_command_end        '$'                             //结束符
#define Soft_command_return     '\0'                            //结束符（换行）

#define CAR_RUN                 0                               //汽车状态：运行
#define CAR_STOP                1                               //汽车状态：停止
#define CAR_SPEED_UP            2                               //汽车状态：加速
#define CAR_SPEED_DOWN          3                               //汽车状态：减速

#define CAR_AUTO                0                               //汽车巡航：自动驾驶状态
#define CAR_MANUAL              1                               //汽车巡航：人工控制状态

#define CAR_SPEED_1             1                               //汽车速度：1档
#define CAR_SPEED_2             2                               //汽车速度：2档
#define CAR_SPEED_3             3                               //汽车速度：3档
#define CAR_SPEED_4             4                               //汽车速度：4档
#define CAR_SPEED_5             5                               //汽车速度：5档
#define CAR_SPEED_6             6                               //汽车速度：6档
#define CAR_SPEED_7             7                               //汽车速度：7档
#define CAR_SPEED_8             8                               //汽车速度：8档
#define CAR_SPEED_9             9                               //汽车速度：9档
#define CAR_SPEED_10            10                              //汽车速度：10档
#define CAR_SPEED_11            11                              //汽车速度：11档
#define CAR_SPEED_12            12                              //汽车速度：12档
#define CAR_SPEED_13            13                              //汽车速度：13档
#define CAR_SPEED_14            14                              //汽车速度：14档
#define CAR_SPEED_15            15                              //汽车速度：15档
#define CAR_SPEED_16            16                              //汽车速度：16档
#define CAR_SPEED_17            17                              //汽车速度：17档
#define CAR_SPEED_18            18                              //汽车速度：18档
#define CAR_SPEED_19            19                              //汽车速度：19档
#define CAR_SPEED_20            20                              //汽车速度：20档

#define CHANGE(SPEED)           2 * SPEED                       //汽车换道速度
//切换策略：与当前速度speed有关
#define DATA_OCT                10                              //十进制

#define LANE_0                  0                               //道路0
#define LANE_1                  1                               //道路1
#define LANE_2                  2                               //道路2
#define LANE_0_X                10                              //道路0中，车的X坐标
#define LANE_1_X                130                             //道路1中，车的X坐标
#define LANE_2_X                240                             //道路2中，车的X坐标

#define INITIAL_POS_Y           420                             //汽车的初始位置y坐标

#define COM_BAUD                9600                            //波特率
#define COM_DATA_LENGTH         7                               //接收数据长度

#define UART2_TX_BUF_SIZE       100                             //串口发送数据缓冲区大小
#define UART2_RX_BUF_SIZE       100                             //串口接收数据缓冲区大小

extern uint16_t x;                                              //障碍车x坐标
extern uint16_t y;                                              //障碍车y坐标

/*********************** 定义 UART FIFO 缓冲区 ***********************/
extern char RxBuf1[UART2_RX_BUF_SIZE];                          //接收数据缓冲区
extern char TxBuf1[UART2_TX_BUF_SIZE];                          //发送数据缓冲区
extern uint8_t RxDataLen;                                       //接收数据缓冲区大小
extern uint8_t serialStatus;                                    //1：开始接收数据  0：暂停接收数据

#endif
