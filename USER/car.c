#include "car.h"
#include "command.h"
#include <stdlib.h>

/************************定义全局汽车对象***********************/
__CAR car;
__CAR obs;

/*** 5：不用并道，保持直行；0：左车道；1: 中间道；2: 右车道。***/
uint8_t nextLane = 5;   

/****************************************************************
 * 名  称：car_init
 * 功  能：小车信息初始化
 * 参  数：
 *   pCar：小车信息结构体指针
 * 返  回：无
 ***************************************************************/
void car_init(__CAR * pCar)
{
    pCar->status = CAR_STOP;
    pCar->mode = CAR_MANUAL;
    pCar->speed	= CAR_SPEED_10;
    pCar->lane = LANE_1;
    pCar->x = LANE_1_X;
    pCar->y = INITIAL_POS_Y;
}

/****************************************************************
 * 名  称：car_changeLane
 * 功  能：切换目标车道
 * 参  数：
 *   pCar：小车信息结构体指针
 *   lane：车道序号
 * 返  回：无
 ***************************************************************/
void car_changeLane(__CAR *pCar, uint8_t lane)
{
    pCar->lane = lane;
    switch(lane)
    {
    case LANE_0:
        pCar->x = LANE_0_X;
        break;
    case LANE_1:
        pCar->x = LANE_1_X;
        break;
    case LANE_2:
        pCar->x = LANE_2_X;
        break;
    default:
        break;
    }
}

/****************************************************************
 * 名  称：car_changeToRandLane
 * 功  能：随机切换车道
 * 参  数：
 *   pCar：小车信息结构体指针
 * 返  回：无
 ***************************************************************/
void car_changeToRandLane(__CAR *pCar)
{
    switch(pCar->lane)
    {
    case LANE_1:
        if(rand()%2 == 0)
        {
            pCar->lane = LANE_0;
            pCar->x = LANE_0_X;
        }
        else
        {
            pCar->lane = LANE_2;
            pCar->x = LANE_2_X;
        }
        break;
    case LANE_0:
        car_changeLane(&car, LANE_1);
        break;
    case LANE_2:
        car_changeLane(&car, LANE_1);
        break;
    default:
        break;
    }
}

/****************************************************************
 * 名  称：car_turnLeft
 * 功  能：切换到左车道
 * 参  数：
 *   pCar：小车信息结构体指针
 * 返  回：无
 ***************************************************************/
void car_turnLeft(__CAR *pCar)
{
    switch(pCar->lane)
    {
    case LANE_1:
        car_changeLane(&car, LANE_0);
        break;
    case LANE_2:
        car_changeLane(&car, LANE_1);
        break;
    default:
        break;
    }
}

/****************************************************************
 * 名  称：car_turnRight
 * 功  能：切换到右车道
 * 参  数：
 *   pCar：小车信息结构体指针
 * 返  回：无
 ***************************************************************/
void car_turnRight(__CAR *pCar)
{
    switch(pCar->lane)
    {
    case LANE_0:
        car_changeLane(&car, LANE_1);
        break;
    case LANE_1:
        car_changeLane(&car, LANE_2);
        break;
    default:
        break;
    }
}

/****************************************************************
 * 名  称：car_autoDrive
 * 功  能：当障碍车与本车在同一车道则并道（自动避障）。
 * 参  数：
 *   pCar：小车信息结构体指针
 *   pObs：障碍小车信息结构体指针
 * 返  回：无
 ***************************************************************/
void car_autoDrive(__CAR *pCar, __CAR *pObs)
{
    //补充代码
}

/****************************************************************
 * 名  称：changeToLane0
 * 功  能：
 *   向0车道并道，逐渐更改小车x值，以CHANGE(pCar->speed)为步长，
 *   直到小车x值到达0道
 * 参  数：pCar：小车信息结构体指针
 * 返  回：无
 ***************************************************************/
void changeToLane0(__CAR *pCar)
{
    if(pCar->x > LANE_0_X)
    {
        if ((pCar->x - LANE_0_X) > CHANGE(pCar->speed))
        {
            pCar->x -= CHANGE(pCar->speed);
        }
        else
        {
            pCar->x = LANE_0_X;
            pCar->lane = LANE_0;
            nextLane = 5;
        }
    }
    else
    {
        pCar->x = LANE_0_X;
        pCar->lane = LANE_0;
        nextLane = 5;
    }
}

/****************************************************************
 * 名  称：changeToLane1
 * 功  能：
 *   向1车道并道，逐渐更改小车x值，以CHANGE(pCar->speed)为步长，
 *   直到小车x值到达0道
 * 参  数：pCar：小车信息结构体指针
 * 返  回：无
 ***************************************************************/
void changeToLane1(__CAR *pCar) {

    if(pCar->x > LANE_1_X)
    {
        if((pCar->x - LANE_1_X) < CHANGE(pCar->speed))
        {
            pCar->x = LANE_1_X;
            pCar->lane = LANE_1;
            nextLane = 5;
        }
        else
        {
            pCar->x -= CHANGE(pCar->speed);
        }
    }
    else
    {
        pCar->x = LANE_1_X;
        pCar->lane = LANE_1;
        nextLane = 5;
    }

    if(pCar->x < LANE_1_X)
    {
        if((LANE_1_X - pCar->x) < CHANGE(pCar->speed))
        {
            pCar->x = LANE_1_X;
            pCar->lane = LANE_1;
            nextLane = 5;
        }
        else
        {
            pCar->x += CHANGE(pCar->speed);
        }
    }
    else
    {
        pCar->x = LANE_1_X;
        pCar->lane = LANE_1;
        nextLane = 5;
    }
}

/****************************************************************
 * 名  称：changeToLane2
 * 功  能：
 *   向2车道并道，逐渐更改小车x值，以CHANGE(pCar->speed)为步长，
 *   直到小车x值到达0道
 * 参  数：pCar：小车信息结构体指针
 * 返  回：无
 ***************************************************************/
void changeToLane2(__CAR *pCar)
{
    if(pCar->x < LANE_2_X)
    {
        if((LANE_2_X - pCar->x) < CHANGE(pCar->speed))
        {
            pCar->x = LANE_2_X;
            pCar->lane = LANE_2_X;
            nextLane = 5;
        }
        else
        {
            pCar->x += CHANGE(pCar->speed);
        }
    }
}

/****************************************************************
 * 名  称：car_updatePos
 * 参  数：pCar：小车信息结构体指针
 * 返  回：无
 * 功  能：
 *   实时刷新小车坐标，如果需要并道则调用changeToRoad0等函数
 *   来更新本车x值
 ***************************************************************/
void car_updatePos(__CAR *pCar)
{
    switch (nextLane)
    {
    case 0:
        //补充代码
        break;
				//补充代码
    default:
        break;
    }
}
