/*
 * RTC_INTERFACE.h
 *
 *  Created on: ??�/??�/????
 *      Author: ALRYADA
 */

#ifndef INCLUDE_HAL_RTC_RTC_INTERFACE_H_
#define INCLUDE_HAL_RTC_RTC_INTERFACE_H_


#include "../../lib/STD_TYPES.h"



/*=========================================================
 * Clock Mode
 *=========================================================*/

typedef enum
{
    RTC_MODE_24_HOUR = 0,
    RTC_MODE_12_HOUR

} RTC_CLOCK_MODE;


/*=========================================================
 * AM / PM
 *=========================================================*/

typedef enum
{
    RTC_AM = 0,
    RTC_PM

} RTC_PERIOD;


/*=========================================================
 * Days of Week
 *=========================================================*/

typedef enum
{
    RTC_SUNDAY = 1,
    RTC_MONDAY,
    RTC_TUESDAY,
    RTC_WEDNESDAY,
    RTC_THURSDAY,
    RTC_FRIDAY,
    RTC_SATURDAY

} RTC_DAY;


/*=========================================================
 * Alarm Selection
 *=========================================================*/

typedef enum
{
    RTC_ALARM_1 = 0,
    RTC_ALARM_2

} RTC_ALARM;


/*=========================================================
 * Alarm State
 *=========================================================*/

typedef enum
{
    RTC_ALARM_DISABLED = 0,
    RTC_ALARM_ENABLED

} RTC_ALARM_STATE;


/*=========================================================
 * Alarm 1 Modes
 *=========================================================*/

typedef enum
{
    /*
     * A1M1 = 1
     * A1M2 = 1
     * A1M3 = 1
     * A1M4 = 1
     *
     * Trigger once every second.
     */
    RTC_ALARM1_EVERY_SECOND = 0,

    /*
     * Trigger when seconds match.
     */
    RTC_ALARM1_MATCH_SECONDS,

    /*
     * Trigger when minutes and seconds match.
     */
    RTC_ALARM1_MATCH_MINUTES_SECONDS,

    /*
     * Trigger when hours, minutes and seconds match.
     */
    RTC_ALARM1_MATCH_HOURS_MINUTES_SECONDS,

    /*
     * Trigger when date, hours, minutes and seconds match.
     */
    RTC_ALARM1_MATCH_DATE_TIME,

    /*
     * Trigger when day, hours, minutes and seconds match.
     */
    RTC_ALARM1_MATCH_DAY_TIME

} RTC_ALARM1_MODE;


/*=========================================================
 * Alarm 2 Modes
 *=========================================================*/

typedef enum
{
    /*
     * Trigger once every minute when seconds become 00.
     */
    RTC_ALARM2_EVERY_MINUTE = 0,

    /*
     * Trigger when minutes match and seconds become 00.
     */
    RTC_ALARM2_MATCH_MINUTES,

    /*
     * Trigger when hours and minutes match.
     */
    RTC_ALARM2_MATCH_HOURS_MINUTES,

    /*
     * Trigger when date, hours and minutes match.
     */
    RTC_ALARM2_MATCH_DATE_TIME,

    /*
     * Trigger when day, hours and minutes match.
     */
    RTC_ALARM2_MATCH_DAY_TIME

} RTC_ALARM2_MODE;


/*=========================================================
 * RTC Date-Time Structure
 *=========================================================*/

typedef struct
{
    u8 Seconds;
    u8 Minutes;
    u8 Hours;

    RTC_PERIOD Period;
    RTC_CLOCK_MODE ClockMode;

    RTC_DAY Day;

    u8 Date;
    u8 Month;
    u16 Year;

} RTC_DATE_TIME;


/*=========================================================
 * Alarm 1 Configuration Structure
 *=========================================================*/

typedef struct
{
    u8 Seconds;
    u8 Minutes;
    u8 Hours;

    RTC_PERIOD Period;
    RTC_CLOCK_MODE ClockMode;

    /*
     * Day is used with:
     * RTC_ALARM1_MATCH_DAY_TIME
     */
    RTC_DAY Day;

    /*
     * Date is used with:
     * RTC_ALARM1_MATCH_DATE_TIME
     */
    u8 Date;

    RTC_ALARM1_MODE Mode;

} RTC_ALARM1_CONFIG;


/*=========================================================
 * Alarm 2 Configuration Structure
 *=========================================================*/

typedef struct
{
    u8 Minutes;
    u8 Hours;

    RTC_PERIOD Period;
    RTC_CLOCK_MODE ClockMode;

    /*
     * Day is used with:
     * RTC_ALARM2_MATCH_DAY_TIME
     */
    RTC_DAY Day;

    /*
     * Date is used with:
     * RTC_ALARM2_MATCH_DATE_TIME
     */
    u8 Date;

    RTC_ALARM2_MODE Mode;

} RTC_ALARM2_CONFIG;


/*=========================================================
 * Initialization
 *=========================================================*/

/*
 * Initialize the RTC driver.
 *
 * I2C must be initialized before using the RTC unless
 * this function initializes I2C internally.
 */
void HRTC_voidInit(void);


/*=========================================================
 * Time Functions
 *=========================================================*/

/*
 * Set RTC time.
 *
 * 24-hour mode:
 * Hours  = 0 to 23
 * Period = Ignored
 *
 * 12-hour mode:
 * Hours  = 1 to 12
 * Period = RTC_AM or RTC_PM
 */
void HRTC_voidSetTime(
    u8 A_u8Hours,
    u8 A_u8Minutes,
    u8 A_u8Seconds,
    RTC_CLOCK_MODE A_enuClockMode,
    RTC_PERIOD A_enuPeriod
);


/*
 * Read RTC time.
 */
void HRTC_voidGetTime(
    RTC_DATE_TIME *A_pstrDateTime
);


/*=========================================================
 * Date Functions
 *=========================================================*/

/*
 * Set RTC date.
 *
 * Day   = RTC_SUNDAY to RTC_SATURDAY
 * Date  = 1 to 31
 * Month = 1 to 12
 * Year  = 2000 to 2099
 */
void HRTC_voidSetDate(
    RTC_DAY A_enuDay,
    u8 A_u8Date,
    u8 A_u8Month,
    u16 A_u16Year
);


/*
 * Read RTC date.
 */
void HRTC_voidGetDate(
    RTC_DATE_TIME *A_pstrDateTime
);


/*=========================================================
 * Complete Date and Time Functions
 *=========================================================*/

/*
 * Write the complete date and time.
 */
void HRTC_voidSetDateTime(
    const RTC_DATE_TIME *A_pstrDateTime
);


/*
 * Read the complete date and time.
 */
void HRTC_voidGetDateTime(
    RTC_DATE_TIME *A_pstrDateTime
);


/*=========================================================
 * Alarm 1 Functions
 *=========================================================*/

/*
 * Configure Alarm 1.
 */
void HRTC_voidSetAlarm1(
    const RTC_ALARM1_CONFIG *A_pstrAlarmConfig
);


/*
 * Read Alarm 1 configuration.
 */
void HRTC_voidGetAlarm1(
    RTC_ALARM1_CONFIG *A_pstrAlarmConfig
);


/*=========================================================
 * Alarm 2 Functions
 *=========================================================*/

/*
 * Configure Alarm 2.
 */
void HRTC_voidSetAlarm2(
    const RTC_ALARM2_CONFIG *A_pstrAlarmConfig
);


/*
 * Read Alarm 2 configuration.
 */
void HRTC_voidGetAlarm2(
    RTC_ALARM2_CONFIG *A_pstrAlarmConfig
);


/*=========================================================
 * Alarm Control Functions
 *=========================================================*/

/*
 * Enable or disable the selected alarm.
 */
void HRTC_voidSetAlarmState(
    RTC_ALARM A_enuAlarm,
    RTC_ALARM_STATE A_enuState
);


/*
 * Return:
 *
 * 1 -> Selected alarm was triggered.
 * 0 -> Selected alarm was not triggered.
 */
u8 HRTC_u8IsAlarmTriggered(
    RTC_ALARM A_enuAlarm
);


/*
 * Clear the selected alarm flag.
 */
void HRTC_voidClearAlarmFlag(
    RTC_ALARM A_enuAlarm
);


/*
 * Disable both alarms and clear their flags.
 */
void HRTC_voidDisableAllAlarms(void);


#endif /* INCLUDE_HAL_RTC_RTC_INTERFACE_H_ */
