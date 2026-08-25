/*
 * LCD_INTERFACE.h
 *
 *  Created on: Aug 20, 2026
 *      Author: Abdullah
 */

#ifndef INCLUDE_HAL_LCD_LCD_INTERFACE_H_
#define INCLUDE_HAL_LCD_LCD_INTERFACE_H_

#include "LCD_CFG.h"
#include "LCD_PRIVATE.h"

typedef enum{
	L1=0,
	L2,
	L3,
	L4
}LCD_ROWS;

typedef enum{
	C1=0,
	C2,
	C3,
	C4,
	C5,
	C6,
	C7,
	C8,
	C9,
	C10,
	C11,
	C12,
	C13,
	C14,
	C15,
	C16,
	C17,
	C18,
	C19,
	C20
}LCD_COLS;

typedef enum
{
	LCD_SUNDAY = 0,
	LCD_MONDAY,
	LCD_TUESDAY,
	LCD_WEDNESDAY,
	LCD_THURSDAY,
	LCD_FRIDAY,
	LCD_SATURDAY
} LCD_DAYS;

typedef enum
{
	LCD_MODE_12_HOUR = 0,
	LCD_MODE_24_HOUR
} LCD_CLOCK_MODE;

typedef enum
{
	LCD_AM_PERIOD = 0,
	LCD_PM_PERIOD
} LCD_CLOCK_PERIOD;

typedef enum
{
	LCD_TEMP_CELSIUS = 0,
	LCD_TEMP_FAHRENHEIT,
	LCD_TEMP_KELVIN
} LCD_TEMP_UNIT;

typedef enum
{
	LCD_POMODORO_OFF = 0,
	LCD_POMODORO_ON
} LCD_POMODORO_STATUS;
typedef enum
{
	LCD_STOPWATCH_OFF = 0,
	LCD_STOPWATCH_ON
} LCD_STOPWATCH_STATUS;

void HLCD_voidSendCommand(u8 A_u8Command);
void HLCD_voidSendData(u8 A_u8Command);
void HLCD_voidInit(void);
void HLCD_voidSendString(u8 *A_Pu8String);
void HLCD_voidClearDisplay(void);
//void HLCD_voidSendSpecialCharacter (u8 *A_pu8PatternArr, u8 A_u8PatternNumber,LCD_ROWS A_LcdRowNo,LCD_COLS A_LcdColNo );
void HLCD_voidDisplayNumber (u32 A_u32Number);
void HLCD_voidGoToPos (LCD_ROWS A_LcdRowNo,LCD_COLS A_LcdColNo);

/* //move to app layer
void HLCD_voidDisplayDateTime(u8 A_u8Hours,u8 A_u8Minutes,u8 A_u8Seconds,LCD_CLOCK_PERIOD A_enuPeriod,LCD_DAYS A_enuDay,u8 A_u8Date,u8 A_u8Month,u16 A_u16Year,LCD_CLOCK_MODE A_enuMode);
void HLCD_voidDisplayTempHumidity(f32 A_f32TemperatureCelsius,f32 A_f32Humidity,LCD_TEMP_UNIT A_TempUnit);
void HLCD_voidDisplayPomodoro(u8 A_u8WorkMinutes,u8 A_u8WorkSeconds,u8 A_u8BreakMinutes,u8 A_u8BreakSeconds,LCD_POMODORO_STATUS A_Status);
void HLCD_voidDisplayStopwatch(u8 A_u8Hours,u8 A_u8Minutes,u8 A_u8Seconds,LCD_STOPWATCH_STATUS A_Status);
*/
#endif /* INCLUDE_HAL_LCD_LCD_INTERFACE_H_ */
