#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f10x.h"

#define BUZZER_RCC      RCC_APB2Periph_GPIOC
#define BUZZER_PORT     GPIOC
#define BUZZER_PIN      GPIO_Pin_13

#define BUZZER_ON       GPIO_SetBits(BUZZER_PORT, BUZZER_PIN)         // ¿ªÆôÀ®°È
#define BUZZER_OFF      GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN)       // ¹Ø±ÕÀ®°È

void buzzer_init(void);
void buzzer_beep(void);       // ·äÃùÆ÷ÃùÏì n ´Î

#endif
