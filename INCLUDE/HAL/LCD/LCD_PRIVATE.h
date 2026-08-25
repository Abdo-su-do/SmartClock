/*
 * LCD_PRIVATE.h
 *
 *  Created on: Aug 20, 2026
 *      Author: Abdullah
 */
#include "../../lib/BIT_MATH.h"

#ifndef INCLUDE_HAL_LCD_LCD_PRIVATE_H_
#define INCLUDE_HAL_LCD_LCD_PRIVATE_H_
//MACROS HERE


#include "LCD_CFG.h"

#if LCD_TYPE == LCD_TYPE_16X2

#define LCD_NUMBER_OF_ROWS       2
#define LCD_NUMBER_OF_COLUMNS    16

#define LCD_LINE1_ADDRESS        0x00
#define LCD_LINE2_ADDRESS        0x40

#elif LCD_TYPE == LCD_TYPE_16X4

#define LCD_NUMBER_OF_ROWS       4
#define LCD_NUMBER_OF_COLUMNS    16

#define LCD_LINE1_ADDRESS        0x00
#define LCD_LINE2_ADDRESS        0x40
#define LCD_LINE3_ADDRESS        0x10
#define LCD_LINE4_ADDRESS        0x50

#elif LCD_TYPE == LCD_TYPE_20X4

#define LCD_NUMBER_OF_ROWS       4
#define LCD_NUMBER_OF_COLUMNS    20

#define LCD_LINE1_ADDRESS        0x00
#define LCD_LINE2_ADDRESS        0x40
#define LCD_LINE3_ADDRESS        0x14
#define LCD_LINE4_ADDRESS        0x54

#else

#error "Unsupported LCD type"

#endif

#define CLEAR_DISPLAY             0X01
#define RETURN_CURSOR_HOME        0X02
#define SET_ENTRY_MODE           CONC_BIT(0,0,0,0,0,1,LCD_ID,LCD_SH)
#define DISPLAY_CONTROL          CONC_BIT(0,0,0,0,1,LCD_BACKLIGHT,LCD_CURSOR,LCD_CURSORBLINK)
#define SET_LCD                  CONC_BIT(0,0,1,LCD_MODE,LCD_NUM_LINES,LCD_CHARACTER_FONT,0,0)




#endif /* INCLUDE_HAL_LCD_LCD_PRIVATE_H_ */
