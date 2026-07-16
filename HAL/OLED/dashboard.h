#ifndef __OLED_DASHBOARD_H
#define __OLED_DASHBOARD_H

#include "stm32f10x.h"
#include "car.h"

void dashboard_init(void);
void dashboard_task(const __CAR *pCar);
uint8_t dashboard_is_ready(void);

#endif
