#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>
#include "car.h"

#define UART_RX_NONE        0
#define UART_RX_COORDINATE  1
#define UART_RX_MODE        2

typedef struct
{
    uint16_t heartbeat;
    uint16_t rxFrameCount;
    uint16_t txFrameCount;
    uint16_t vddaMv;
    uint8_t pcOnline;
    uint8_t lastRxType;
    uint8_t lastRxMode;
    int lastRxX;
    int lastRxY;
} UartTelemetrySnapshot;

void uart_init(u32 bound);
void uart_sendCarInfo(__CAR *pCar);
void uart_sendCarInfoNow(__CAR *pCar);       // 发送车数据
void uart_getTelemetrySnapshot(UartTelemetrySnapshot *snapshot);

#endif
