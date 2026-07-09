/************************************************************************
*   ����: uart.c
*   ����: 2024-6-6
*   ����: wtltom
*   ������
*       ����ͨѶ���򣬴��������շ���
*************************************************************************/
#include "stm32f10x.h"
#include "uart.h"
#include "command.h"
#include "string.h"
#include "motor.h"
#include <stdlib.h>

#define CLR_RX_BUFF     memset(RxBuf1, 0, UART2_RX_BUF_SIZE)
#define TELEMETRY_DIVIDER       10
#define TELEMETRY_VREFINT_MV    1200
/*********************** ���崮�� UART FIFO ������ **********************/
char RxBuf1[UART2_RX_BUF_SIZE];              // �������ݻ�����
char TxBuf1[UART2_TX_BUF_SIZE];              // �������ݻ�����
uint8_t RxDataLen = 0;                       // �������ݻ�������С
uint8_t uart1_status = 0;                    // 0 idle, 1 busy

static uint16_t telemetryHeartbeat = 0;
static uint16_t uartRxFrameCount = 0;
static uint16_t uartTxFrameCount = 0;
static uint8_t telemetryDivider = 0;

static void telemetry_adc_init(void);
static uint16_t telemetry_read_vdda_mv(void);
static uint16_t estimate_motor_rpm(uint8_t motorRunning, uint8_t speed);
/************************************************************************
* ��  ��: fgetc
* ��  ��: �ض���c�⺯��scanf��USART2
* ��  ��: �ļ�ָ��
* ��  ��: ����
*************************************************************************/
int fgetc(FILE *f)
{
    /* �ȴ�������� */
    while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(USART2);
}

/***********************************************************************
* ��  ��: ConfigUartNVIC
* ��  ��: �ض���c�⺯��printf��USART2
* ��  ��: �ļ�ָ��
* ��  ��: ����
************************************************************************/
int fputc(int ch, FILE *f)
{
    /* ����һ���ֽ����ݵ�USART2 */
    USART_SendData(USART2, (uint8_t) ch);

    /* �ȴ�������� */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);

    return (ch);
}

/***********************************************************************
* ��  ��: uart_init
* ��  ��: ���ڳ�ʼ��
* ��  ��: ������bound
* ��  ��: ��
************************************************************************/
void uart_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;

   // RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);      // ʹ��USART2ʱ��
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);       // ʹ��GPIOAʱ��

    // USART2_TX     GPIOA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                   // PA.2 TX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;             // �����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                      // ��ʼ��GPIOA.2

    // USART2_RX     GPIOA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;                   // PA.3 RX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       // ��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                      // ��ʼ��GPIOA.3

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USART2 Interrupt */
    memset(&NVIC_InitStructure, 0, sizeof(NVIC_InitStructure));
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable USART2 */
    USART_InitStructure.USART_BaudRate = bound;                                         // ���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                         // �ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                              // һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                                 // ����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     // ��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                     // �շ�ģʽ
    USART_Init(USART2, &USART_InitStructure);                                           // ��ʼ������1
    USART_Cmd(USART2, ENABLE);                                                          // ʹ�ܴ���1

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                                      // enable UART RX interrupt

    telemetry_adc_init();
}

/***********************************************************************
* ��  ��: uart_SendByte
* ��  ��: ���ڷ���
* ��  ��: Byte�����ֽڷ���
* ��  ��: ��
************************************************************************/

static void telemetry_adc_init(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_TempSensorVrefintCmd(ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

static uint16_t telemetry_read_vdda_mv(void)
{
    uint16_t raw = 0;
    uint32_t vdda = 0;

    ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    raw = ADC_GetConversionValue(ADC1);
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

    if(raw == 0)
    {
        return 0;
    }

    vdda = (uint32_t)TELEMETRY_VREFINT_MV * 4095U / raw;
    return (uint16_t)vdda;
}

static uint16_t estimate_motor_rpm(uint8_t motorRunning, uint8_t speed)
{
    if(!motorRunning)
    {
        return 0;
    }

    return (uint16_t)(300U + (uint16_t)speed * 90U);
}
void uart_SendByte(uint16_t txData)
{
    USART_SendData(USART2, txData);

    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

/***********************************************************************
* ��  ��: uart_sendCarInfo
* ��  ��: ���ڷ���
* ��  ��: Byte�����ֽڷ���
* ��  ��: ��
************************************************************************/
void uart_sendCarInfo(__CAR *pCar)
{
    uartTxFrameCount++;
    telemetryDivider++;

    if(telemetryDivider < TELEMETRY_DIVIDER)
    {
        printf("%c%3d%c%3d%c", Soft_command_beging, pCar->x, Soft_command_return, pCar->y, Soft_command_end);
        return;
    }

    telemetryDivider = 0;
    telemetryHeartbeat++;

    printf("%cX=%d,Y=%d,HB=%u,RX=%u,TX=%u,RL=%u,RR=%u,V=%u,ST=%u%c",
           Soft_command_beging,
           pCar->x,
           pCar->y,
           telemetryHeartbeat,
           uartRxFrameCount,
           uartTxFrameCount,
           estimate_motor_rpm(motor_leftRunning(), pCar->speed),
           estimate_motor_rpm(motor_rightRunning(), pCar->speed),
           telemetry_read_vdda_mv(),
           1,
           Soft_command_end);
}
/***********************************************************************
* ��  ��: DataAnalysis
* ��  ��: �������ڽ�������
* ��  ��: ��
* ��  ��: ��
************************************************************************/
void DataAnalysis(void)
{
    // ********************TODO:�붨���Լ������ݸ�ʽ********************
    // *************************���벹����������************************
    // *************************��***��***֮�䣬�������****************
    // ���ڽ���֡���ã�
    // 0-2byte---�ϰ���X���꣬3byte---������\0
    // 4-6byte---�ϰ���Y���꣬7byte---������\0
    obs.x = atoi(&RxBuf1[0]);
    obs.y = atoi(&RxBuf1[4]);

    // *****************************************************************
}

/***********************************************************************
* ��  ��: USART2_IRQHandler
* ��  ��: USART2�жϷ�����򼴽��մ��ڷ��͵�ָ��
* ��  ��: ��
* ��  ��: ��
************************************************************************/
void USART2_IRQHandler(void)
{
    uint16_t res = 0;

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)   // �������ڽ���
    {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);     // ������ڽ����жϱ��

        res =  USART_ReceiveData(USART2);
        
        switch (res) {
        case Soft_command_beging:                   // ��ʼ���ݽ���
            RxDataLen = 0;
            CLR_RX_BUFF;                            // ������ݻ���
            break;
        case Soft_command_end:
            RxBuf1[RxDataLen] = 0;
            DataAnalysis();                         // parse received frame
            uartRxFrameCount++;
            break;
        default:                                    // receive frame payload
            if(RxDataLen < UART2_RX_BUF_SIZE - 1)
            {
                RxBuf1[RxDataLen++] = res;
            }
            break;
        }
    }
}
