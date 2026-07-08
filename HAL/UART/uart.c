/************************************************************************
*   名称: uart.c
*   日期: 2024-6-6
*   作者: wtltom
*   描述：
*       串口通讯程序，处理串口收发。
*************************************************************************/
#include "stm32f10x.h"
#include "uart.h"
#include "command.h"
#include "string.h"
#include "motor.h"
#include <stdlib.h>

#define CLR_RX_BUFF     memset(RxBuf1, 0, UART2_RX_BUF_SIZE)

/*********************** 定义串口 UART FIFO 缓冲区 **********************/
char RxBuf1[UART2_RX_BUF_SIZE];              // 接收数据缓冲区
char TxBuf1[UART2_TX_BUF_SIZE];              // 发送数据缓冲区
uint8_t RxDataLen = 0;                       // 接收数据缓冲区大小
uint8_t uart1_status = 0;                    // 0：总线空闲，1：接收数据

/************************************************************************
* 名  称: fgetc
* 功  能: 重定向c库函数scanf到USART2
* 参  数: 文件指针
* 返  回: 整型
*************************************************************************/
int fgetc(FILE *f)
{
    /* 等待发送完毕 */
    while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(USART2);
}

/***********************************************************************
* 名  称: ConfigUartNVIC
* 功  能: 重定向c库函数printf到USART2
* 参  数: 文件指针
* 返  回: 整型
************************************************************************/
int fputc(int ch, FILE *f)
{
    /* 发送一个字节数据到USART2 */
    USART_SendData(USART2, (uint8_t) ch);

    /* 等待发送完毕 */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);

    return (ch);
}

/***********************************************************************
* 名  称: uart_init
* 功  能: 串口初始化
* 参  数: 波特率bound
* 返  回: 无
************************************************************************/
void uart_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;

   // RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);      // 使能USART2时钟
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);       // 使能GPIOA时钟

    // USART2_TX     GPIOA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                   // PA.2 TX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;             // 复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);                      // 初始化GPIOA.2

    // USART2_RX     GPIOA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;                   // PA.3 RX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                      // 初始化GPIOA.3

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable USART2 */
    USART_InitStructure.USART_BaudRate = bound;                                         // 串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                         // 字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                              // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                                 // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     // 无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                     // 收发模式
    USART_Init(USART2, &USART_InitStructure);                                           // 初始化串口1
    USART_Cmd(USART2, ENABLE);                                                          // 使能串口1

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                                      // 开启串口接收中断
}

/***********************************************************************
* 名  称: uart_SendByte
* 功  能: 串口发送
* 参  数: Byte：按字节发送
* 返  回: 无
************************************************************************/
void uart_SendByte(uint16_t txData)
{
    USART_SendData(USART2, txData);

    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

/***********************************************************************
* 名  称: uart_sendCarInfo
* 功  能: 串口发送
* 参  数: Byte：按字节发送
* 返  回: 无
************************************************************************/
void uart_sendCarInfo(__CAR *pCar)
{
    // 串口发送帧设置：
    // 0-2byte---控制车X坐标，3byte---结束符\0
    // 4-6byte---控制车Y坐标，7bute---结束符\0

    printf("%c%3d%c%3d%c", Soft_command_beging, pCar->x, Soft_command_return, pCar->y, Soft_command_end);
}

/***********************************************************************
* 名  称: DataAnalysis
* 功  能: 解析串口接收数据
* 参  数: 无
* 返  回: 无
************************************************************************/
void DataAnalysis(void)
{
    // ********************TODO:请定义自己的数据格式********************
    // *************************代码补充在这里面************************
    // *************************在***与***之间，补充代码****************
    // 串口接收帧设置：
    // 0-2byte---障碍车X坐标，3byte---结束符\0
    // 4-6byte---障碍车Y坐标，7byte---结束符\0
    obs.x = atoi(&RxBuf1[0]);
    obs.y = atoi(&RxBuf1[4]);

    // *****************************************************************
}

/***********************************************************************
* 名  称: USART2_IRQHandler
* 功  能: USART2中断服务程序即接收串口发送的指令
* 参  数: 无
* 返  回: 无
************************************************************************/
void USART2_IRQHandler(void)
{
    uint16_t res = 0;

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)   // 发生串口接收
    {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);     // 清除串口接收中断标记

        res =  USART_ReceiveData(USART2);
        
        switch (res) {
        case Soft_command_beging:                   // 开始数据接收
            RxDataLen = 0;
            CLR_RX_BUFF;                            // 清空数据缓存
            break;
        case Soft_command_end:
            DataAnalysis();                         // 解析接收数据
            break;
        default:                                    // 保存接收数据
            RxBuf1[RxDataLen++] = res;
            break;
        }
    }
}
