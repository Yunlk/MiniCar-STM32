#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>
#include "car.h"

void uart_init(u32 bound);
void uart_sendCarInfo(__CAR *pCar);       // 发送车数据

#endif
