#include "car.h"
#include "command.h"
#include <stdlib.h>

#define LANE_NONE               5
#define AUTO_LOOKAHEAD_MIN_Y    (INITIAL_POS_Y - 260)
#define AUTO_LOOKAHEAD_MAX_Y    (INITIAL_POS_Y + 40)

/************************定义全局汽车对象***********************/
__CAR car;
__CAR obs;

/*** 5：不用并道，保持直行；0：左车道；1: 中间道；2: 右车道。***/
uint8_t nextLane = LANE_NONE;

static uint8_t lane_from_x(int x)
{
    int d0 = abs(x - LANE_0_X);
    int d1 = abs(x - LANE_1_X);
    int d2 = abs(x - LANE_2_X);

    if(d0 <= d1 && d0 <= d2)
    {
        return LANE_0;
    }

    if(d1 <= d0 && d1 <= d2)
    {
        return LANE_1;
    }

    return LANE_2;
}

static uint8_t obstacle_in_danger_zone(__CAR *pObs)
{
    return pObs->y >= AUTO_LOOKAHEAD_MIN_Y && pObs->y <= AUTO_LOOKAHEAD_MAX_Y;
}

static uint8_t choose_escape_lane(uint8_t currentLane, uint8_t blockedLane)
{
    if(currentLane != blockedLane)
    {
        return currentLane;
    }

    if(currentLane == LANE_0)
    {
        return LANE_1;
    }

    if(currentLane == LANE_2)
    {
        return LANE_1;
    }

    return LANE_0;
}

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
void car_cancelAutoDrive(__CAR *pCar)
{
    nextLane = LANE_NONE;
    pCar->lane = lane_from_x(pCar->x);
}
void car_changeLane(__CAR *pCar, uint8_t lane)
{
    nextLane = LANE_NONE;
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
        car_changeLane(pCar, LANE_1);
        break;
    case LANE_2:
        car_changeLane(pCar, LANE_1);
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
        car_changeLane(pCar, LANE_0);
        break;
    case LANE_2:
        car_changeLane(pCar, LANE_1);
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
        car_changeLane(pCar, LANE_1);
        break;
    case LANE_1:
        car_changeLane(pCar, LANE_2);
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
    uint8_t currentLane = lane_from_x(pCar->x);
    uint8_t obstacleLane = lane_from_x(pObs->x);
    uint8_t targetLane = currentLane;

    if(nextLane != LANE_NONE)
    {
        return;
    }

    pCar->lane = currentLane;

    if(!obstacle_in_danger_zone(pObs))
    {
        return;
    }

    targetLane = choose_escape_lane(currentLane, obstacleLane);
    if(targetLane != currentLane)
    {
        nextLane = targetLane;
    }
}
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
            nextLane = LANE_NONE;
        }
    }
    else
    {
        pCar->x = LANE_0_X;
        pCar->lane = LANE_0;
        nextLane = LANE_NONE;
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
void changeToLane1(__CAR *pCar)
{
    if(pCar->x > LANE_1_X)
    {
        if((pCar->x - LANE_1_X) < CHANGE(pCar->speed))
        {
            pCar->x = LANE_1_X;
            pCar->lane = LANE_1;
            nextLane = LANE_NONE;
        }
        else
        {
            pCar->x -= CHANGE(pCar->speed);
        }
    }
    else if(pCar->x < LANE_1_X)
    {
        if((LANE_1_X - pCar->x) < CHANGE(pCar->speed))
        {
            pCar->x = LANE_1_X;
            pCar->lane = LANE_1;
            nextLane = LANE_NONE;
        }
        else
        {
            pCar->x += CHANGE(pCar->speed);
        }
    }
    else
    {
        pCar->lane = LANE_1;
        nextLane = LANE_NONE;
    }
}
void changeToLane2(__CAR *pCar)
{
    if(pCar->x < LANE_2_X)
    {
        if((LANE_2_X - pCar->x) < CHANGE(pCar->speed))
        {
            pCar->x = LANE_2_X;
            pCar->lane = LANE_2;
            nextLane = LANE_NONE;
        }
        else
        {
            pCar->x += CHANGE(pCar->speed);
        }
    }
    else
    {
        pCar->x = LANE_2_X;
        pCar->lane = LANE_2;
        nextLane = LANE_NONE;
    }
}
void car_updatePos(__CAR *pCar)
{
    switch (nextLane)
    {
    case LANE_0:
        changeToLane0(pCar);
        break;
    case LANE_1:
        changeToLane1(pCar);
        break;
    case LANE_2:
        changeToLane2(pCar);
        break;
    default:
        nextLane = LANE_NONE;
        break;
    }
}
