#include "dashboard.h"
#include "oled.h"
#include "uart.h"
#include "command.h"
#include <stdio.h>

#define DASHBOARD_REFRESH_TICKS  25
#define DASHBOARD_RETRY_TICKS   200

static uint8_t dashboardReady = 0;
static uint16_t dashboardTick = 0;

static int dashboard_clamp_coordinate(int value)
{
    if(value < 0)
    {
        return 0;
    }
    if(value > 999)
    {
        return 999;
    }
    return value;
}

static void dashboard_draw_boot(void)
{
    oled_clear();
    oled_draw_rect(0, 0, OLED_WIDTH, OLED_HEIGHT, 1);
    oled_draw_line(8, 9, 119, 9, 1);
    oled_draw_line(8, 53, 119, 53, 1);
    oled_draw_text(37, 14, "MINICAR", 1);
    oled_draw_text(28, 27, "SYSTEM BOOT", 1);
    oled_draw_text(34, 40, "OLED I2C", 1);
    oled_fill_rect(3, 3, 3, 3, 1);
    oled_fill_rect(122, 3, 3, 3, 1);
    oled_fill_rect(3, 58, 3, 3, 1);
    oled_fill_rect(122, 58, 3, 3, 1);
    oled_update();
}

static void dashboard_draw_header(const __CAR *pCar, const UartTelemetrySnapshot *status)
{
    if(pCar->mode == CAR_AUTO)
    {
        oled_draw_rect(0, 0, 29, 9, 1);
        oled_draw_text(3, 1, "AUTO", 1);
    }
    else
    {
        oled_draw_rect(0, 0, 23, 9, 1);
        oled_draw_text(3, 1, "MAN", 1);
    }

    oled_draw_char(35, 1, 'L', 1);
    oled_draw_char(41, 1, (char)('0' + (pCar->lane <= LANE_2 ? pCar->lane : 0)), 1);

    oled_fill_rect(81, 3, 3, 3, status->pcOnline ? 1 : 0);
    oled_draw_text(87, 1, status->pcOnline ? "PC ON" : "PC OFF", 1);
    oled_draw_line(0, 9, 127, 9, 1);
}

static void dashboard_draw_voltage(uint16_t vddaMv)
{
    uint8_t whole;
    uint8_t tenth;
    uint8_t hundredth;

    oled_draw_text(2, 19, "VDDA", 1);
    oled_draw_line(2, 28, 23, 28, 1);

    if(vddaMv < 1800 || vddaMv > 5000)
    {
        oled_draw_text(38, 18, "NO DATA", 1);
        return;
    }

    whole = (uint8_t)((vddaMv / 1000U) % 10U);
    tenth = (uint8_t)((vddaMv / 100U) % 10U);
    hundredth = (uint8_t)((vddaMv / 10U) % 10U);

    oled_draw_big_digit(29, 12, whole, 1);
    oled_fill_rect(44, 31, 3, 3, 1);
    oled_draw_big_digit(50, 12, tenth, 1);
    oled_draw_big_digit(65, 12, hundredth, 1);
    oled_draw_text(82, 27, "V", 1);

    oled_draw_text(104, 13, "MCU", 1);
    oled_draw_rect(103, 22, 24, 13, 1);
    oled_draw_line(107, 19, 107, 22, 1);
    oled_draw_line(113, 19, 113, 22, 1);
    oled_draw_line(119, 19, 119, 22, 1);
    oled_draw_line(107, 35, 107, 38, 1);
    oled_draw_line(113, 35, 113, 38, 1);
    oled_draw_line(119, 35, 119, 38, 1);
}

static void dashboard_draw_serial(const __CAR *pCar, const UartTelemetrySnapshot *status)
{
    char line[22];
    int carX = dashboard_clamp_coordinate(pCar->x);
    int carY = dashboard_clamp_coordinate(pCar->y);

    oled_draw_line(0, 39, 127, 39, 1);
    oled_draw_text(0, 41, "UART2 9600 8N1", 1);

    sprintf(line, "TX:%04u X%03d Y%03d",
            status->txFrameCount % 10000U,
            carX,
            carY);
    oled_draw_text(0, 49, line, 1);

    if(status->lastRxType == UART_RX_MODE)
    {
        sprintf(line, "RX:%04u MODE:%s",
                status->rxFrameCount % 10000U,
                status->lastRxMode == CAR_AUTO ? "AUTO" : "MAN");
    }
    else if(status->lastRxType == UART_RX_COORDINATE)
    {
        sprintf(line, "RX:%04u X%03d Y%03d",
                status->rxFrameCount % 10000U,
                dashboard_clamp_coordinate(status->lastRxX),
                dashboard_clamp_coordinate(status->lastRxY));
    }
    else
    {
        sprintf(line, "RX:%04u NO DATA", status->rxFrameCount % 10000U);
    }

    oled_draw_text(0, 57, line, 1);
}

void dashboard_init(void)
{
    dashboardReady = oled_init();
    dashboardTick = 0;

    if(dashboardReady)
    {
        dashboard_draw_boot();
    }
}

void dashboard_task(const __CAR *pCar)
{
    UartTelemetrySnapshot status;

    dashboardTick++;
    if(!dashboardReady)
    {
        if(dashboardTick >= DASHBOARD_RETRY_TICKS)
        {
            dashboardTick = 0;
            dashboardReady = oled_init();
        }
        return;
    }

    if(dashboardTick < DASHBOARD_REFRESH_TICKS)
    {
        return;
    }
    dashboardTick = 0;

    uart_getTelemetrySnapshot(&status);
    oled_clear();
    dashboard_draw_header(pCar, &status);
    dashboard_draw_voltage(status.vddaMv);
    dashboard_draw_serial(pCar, &status);

    if(!oled_update())
    {
        dashboardReady = 0;
    }
}

uint8_t dashboard_is_ready(void)
{
    return dashboardReady;
}
