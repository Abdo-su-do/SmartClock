/*
 * RTC_CFG.h
 *
 *  Created on: ??þ/??þ/????
 *      Author: ALRYADA
 */

#ifndef INCLUDE_HAL_RTC_RTC_CFG_H_
#define INCLUDE_HAL_RTC_RTC_CFG_H_




/*=========================================================
 * Clock Mode Configuration
 *=========================================================*/

#define RTC_CFG_24_HOUR_MODE         0
#define RTC_CFG_12_HOUR_MODE         1

/*
 * Select:
 *
 * RTC_CFG_24_HOUR_MODE
 * RTC_CFG_12_HOUR_MODE
 */
#define RTC_DEFAULT_CLOCK_MODE       RTC_CFG_24_HOUR_MODE


/*=========================================================
 * Square-Wave Output Configuration
 *=========================================================*/

#define RTC_SQUARE_WAVE_DISABLED     0
#define RTC_SQUARE_WAVE_1HZ          1
#define RTC_SQUARE_WAVE_1024HZ       2
#define RTC_SQUARE_WAVE_4096HZ       3
#define RTC_SQUARE_WAVE_8192HZ       4

/*
 * Select:
 *
 * RTC_SQUARE_WAVE_DISABLED
 * RTC_SQUARE_WAVE_1HZ
 * RTC_SQUARE_WAVE_1024HZ
 * RTC_SQUARE_WAVE_4096HZ
 * RTC_SQUARE_WAVE_8192HZ
 */
#define RTC_SQUARE_WAVE_MODE         RTC_SQUARE_WAVE_DISABLED

#endif /* INCLUDE_HAL_RTC_RTC_CFG_H_ */
