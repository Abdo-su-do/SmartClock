/*
 * TIMER.h
 */
#ifndef INCLUDE_TIMER_H_
#define INCLUDE_TIMER_H_

#include "lib/STD_TYPES.h"
#include "HAL/LCD/LCD_INTERFACE.h"

#define TIMER_BUZZER_PORT    PORTC
#define TIMER_BUZZER_PIN     PIN2

void TIMER_APP_voidInit(void);
void TIMER_APP_voidUpdate(u8 A_u8PressedKey);

/* Stopwatch Functions */
void STOPWATCH_APP_voidInit(void);
void STOPWATCH_APP_voidUpdate(u8 A_u8PressedKey);

#endif /* INCLUDE_TIMER_H_ */