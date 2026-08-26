/*
 * LCD_CFG.h
 *
 *  Created on: Aug 20, 2026
 *      Author: fares
 */

#ifndef INCLUDE_HAL_LCD_LCD_CFG_H_
#define INCLUDE_HAL_LCD_LCD_CFG_H_

// Library Directives
#include "../../lib/STD_TYPES.h"
#include "../../lib/BIT_MATH.h"
#include "../../MCAL/DIO/DIO_INTERFACE.h"

// LCD Control Pins (Mapped to PORTA)
#define LCD_RS_PIN   PIN2
#define LCD_RS_PORT  PORTA

#define LCD_RW_PIN   PIN3
#define LCD_RW_PORT  PORTA

#define LCD_E_PIN    PIN4
#define LCD_E_PORT   PORTA

// LCD Data Port (Mapped to PORTD entirely to avoid I2C conflict on PORTC)
#define LCD_DATA_PORT PORTD

// Settings
#define LCD_MODE_8BIT        1
#define LCD_MODE_4BIT        0
#define LCD_MODE           LCD_MODE_8BIT

#define LCD_NUM_L1        0
#define LCD_NUM_L2        1
#define LCD_NUM_LINES           LCD_NUM_L2

#define LCD_CHARACTER_FONT_5_8         0
#define LCD_CHARACTER_FONT_5_10        1
#define LCD_CHARACTER_FONT           LCD_CHARACTER_FONT_5_8

#define LCD_BACKLIGHT_ON     1
#define LCD_BACKLIGHT_OFF    0
#define LCD_BACKLIGHT    LCD_BACKLIGHT_ON

#define LCD_CURSOR_ON     1
#define LCD_CURSOR_OFF    0
#define LCD_CURSOR    LCD_CURSOR_OFF

#define LCD_CURSORBLINK_ON     1
#define LCD_CURSORBLINK_OFF    0
#define LCD_CURSORBLINK    LCD_CURSORBLINK_OFF

#define LCD_ID_INCREMENT   1
#define LCD_ID_DECREMENT   0
#define LCD_ID    LCD_ID_INCREMENT

#define LCD_SHIFT_ON  1
#define LCD_SHIFT_OFF   0
#define LCD_SH    LCD_SHIFT_OFF

/* LCD types */
#define LCD_TYPE_16X2    0
#define LCD_TYPE_16X4    1
#define LCD_TYPE_20X4    2

/* Select your LCD type */
#define LCD_TYPE         LCD_TYPE_20X4

#endif /* INCLUDE_HAL_LCD_LCD_CFG_H_ */
