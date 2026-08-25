/*
 * RTC_CFG.h
 *
 * DS3231 RTC configuration file
 */

#ifndef RTC_CFG_H_
#define RTC_CFG_H_


/*================ Clock Mode Configuration ================*/

#define RTC_CLOCK_MODE_24_HOUR       0
#define RTC_CLOCK_MODE_12_HOUR       1

/*
 * Select:
 * RTC_CLOCK_MODE_24_HOUR
 * RTC_CLOCK_MODE_12_HOUR
 */
#define RTC_CLOCK_MODE               RTC_CLOCK_MODE_24_HOUR


/*================ Square-Wave Output Configuration ================*/

#define RTC_SQUARE_WAVE_DISABLED     0
#define RTC_SQUARE_WAVE_1HZ          1
#define RTC_SQUARE_WAVE_1024HZ       2
#define RTC_SQUARE_WAVE_4096HZ       3
#define RTC_SQUARE_WAVE_8192HZ       4

/*
 * Select the SQW output frequency.
 */
#define RTC_SQUARE_WAVE_MODE         RTC_SQUARE_WAVE_DISABLED


#endif /* RTC_CFG_H_ */