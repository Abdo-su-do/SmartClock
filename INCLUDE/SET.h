/*
 * SET.h
 *
 *  Created on: Aug 27, 2026
 *      Author: elmal
 */

#ifndef SOURCE_SETTINGS_SET_H_
#define SOURCE_SETTINGS_SET_H_

#include "INCLUDES.h"

#define SET_SCREEN1 0
#define SET_SCREEN2 1
#define SET_SCREEN3 2
#define SET_TIME_MODE 3


#define SET_TIME_VALUE 4

#define SET_DATE_VALUE 5

#define SET_TEMP_UNIT 6

#define LCD_EMPTY_LINE    "                    "

typedef struct{
	RTC_DATE_TIME time_setting;

	LCD_TEMP_UNIT temp_setting;

}SETTING;

void ASET_voidSetScreen1(void);
void ASET_voidSetScreen2(void);
void ASET_voidSetScreenTimeMode(void);
void ASET_voidSetScreenTimeValue(u8 A_u8PressedKey,s8 *A_u8SetScreenCounter , RTC_CLOCK_MODE A_enuSetClockMode );
void ASET_voidSetScreenDateValue(u8 A_u8PressedKey,s8 *A_u8SetScreenCounter );
void ASET_voidSetScreenTempUnit(void);

#endif /* SOURCE_SETTINGS_SET_H_ */
