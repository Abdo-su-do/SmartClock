/*
 * RTC_INTERFACE.h
 *
 * DS3231 RTC public interface
 */

#ifndef RTC_INTERFACE_H_
#define RTC_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"


/*================ Clock Mode ================*/

typedef enum
{
    RTC_MODE_24_HOUR = 0,
    RTC_MODE_12_HOUR

} RTC_CLOCK_MODE;


/*================ AM / PM ================*/

typedef enum
{
    RTC_AM = 0,
    RTC_PM

} RTC_PERIOD;


/*================ Days of Week ================*/

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


/*================ Alarm Selection ================*/

typedef enum
{
    RTC_ALARM_1 = 0,
    RTC_ALARM_2

} RTC_ALARM;


/*================ Alarm State ================*/

typedef enum
{
    RTC_ALARM_DISABLED = 0,
    RTC_ALARM_ENABLED

} RTC_ALARM_STATE;


/*================ Alarm 1 Modes ================*/

/*
 * Alarm 1 can compare:
 * Seconds, minutes, hours, and day/date.
 */
typedef enum
{
    /* Alarm once every second */
    RTC_ALARM1_EVERY_SECOND = 0,

    /* Alarm when seconds match */
    RTC_ALARM1_MATCH_SECONDS,

    /* Alarm when minutes and seconds match */
    RTC_ALARM1_MATCH_MINUTES_SECONDS,

    /* Alarm when hours, minutes, and seconds match */
    RTC_ALARM1_MATCH_HOURS_MINUTES_SECONDS,

    /* Alarm when date, hours, minutes, and seconds match */
    RTC_ALARM1_MATCH_DATE_TIME,

    /* Alarm when day, hours, minutes, and seconds match */
    RTC_ALARM1_MATCH_DAY_TIME

} RTC_ALARM1_MODE;


/*================ Alarm 2 Modes ================*/

/*
 * Alarm 2 does not contain a seconds register.
 * It operates when seconds become 00.
 */
typedef enum
{
    /* Alarm once every minute at second 00 */
    RTC_ALARM2_EVERY_MINUTE = 0,

    /* Alarm when minutes match and seconds become 00 */
    RTC_ALARM2_MATCH_MINUTES,

    /* Alarm when hours and minutes match */
    RTC_ALARM2_MATCH_HOURS_MINUTES,

    /* Alarm when date, hours, and minutes match */
    RTC_ALARM2_MATCH_DATE_TIME,

    /* Alarm when day, hours, and minutes match */
    RTC_ALARM2_MATCH_DAY_TIME

} RTC_ALARM2_MODE;


/*================ RTC Date-Time Structure ================*/

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


/*================ Alarm 1 Structure ================*/

typedef struct
{
    u8 Seconds;
    u8 Minutes;
    u8 Hours;

    RTC_PERIOD Period;
    RTC_CLOCK_MODE ClockMode;

    /*
     * Use Day with RTC_ALARM1_MATCH_DAY_TIME.
     * Use Date with RTC_ALARM1_MATCH_DATE_TIME.
     */
    RTC_DAY Day;
    u8 Date;

    RTC_ALARM1_MODE Mode;

} RTC_ALARM1_CONFIG;


/*================ Alarm 2 Structure ================*/

typedef struct
{
    u8 Minutes;
    u8 Hours;

    RTC_PERIOD Period;
    RTC_CLOCK_MODE ClockMode;

    /*
     * Use Day with RTC_ALARM2_MATCH_DAY_TIME.
     * Use Date with RTC_ALARM2_MATCH_DATE_TIME.
     */
    RTC_DAY Day;
    u8 Date;

    RTC_ALARM2_MODE Mode;

} RTC_ALARM2_CONFIG;


/*================ Initialization ================*/

/*
 * Initializes the I2C peripheral and configures the DS3231.
 */
void HRTC_voidInit(void);


/*================ Time Functions ================*/

/*
 * Set time in the DS3231.
 *
 * 24-hour mode:
 * Hours = 0 to 23
 *
 * 12-hour mode:
 * Hours = 1 to 12
 */
void HRTC_voidSetTime(u8 A_u8Hours,
                     u8 A_u8Minutes,
                     u8 A_u8Seconds,
                     RTC_CLOCK_MODE A_enuClockMode,
                     RTC_PERIOD A_enuPeriod);


/*
 * Read the complete time from the DS3231.
 */
void HRTC_voidGetTime(RTC_DATE_TIME *A_pstrDateTime);


/*================ Date Functions ================*/

/*
 * Set the date in the DS3231.
 *
 * Day   = RTC_SUNDAY to RTC_SATURDAY
 * Date  = 1 to 31
 * Month = 1 to 12
 * Year  = 2000 to 2099
 */
void HRTC_voidSetDate(RTC_DAY A_enuDay,
                     u8 A_u8Date,
                     u8 A_u8Month,
                     u16 A_u16Year);


/*
 * Read the complete date from the DS3231.
 */
void HRTC_voidGetDate(RTC_DATE_TIME *A_pstrDateTime);


/*================ Complete Date and Time ================*/

/*
 * Set both date and time.
 */
void HRTC_voidSetDateTime(const RTC_DATE_TIME *A_pstrDateTime);


/*
 * Read both date and time.
 */
void HRTC_voidGetDateTime(RTC_DATE_TIME *A_pstrDateTime);


/*================ Alarm 1 Functions ================*/

/*
 * Configure Alarm 1.
 *
 * Seconds = 0 to 59
 * Minutes = 0 to 59
 *
 * Hours in 24-hour mode = 0 to 23
 * Hours in 12-hour mode = 1 to 12
 *
 * Date = 1 to 31
 */
void HRTC_voidSetAlarm1(const RTC_ALARM1_CONFIG *A_pstrAlarmConfig);


/*
 * Read the current Alarm 1 configuration.
 */
void HRTC_voidGetAlarm1(RTC_ALARM1_CONFIG *A_pstrAlarmConfig);


/*================ Alarm 2 Functions ================*/

/*
 * Configure Alarm 2.
 *
 * Alarm 2 operates when seconds become 00.
 *
 * Minutes = 0 to 59
 *
 * Hours in 24-hour mode = 0 to 23
 * Hours in 12-hour mode = 1 to 12
 *
 * Date = 1 to 31
 */
void HRTC_voidSetAlarm2(const RTC_ALARM2_CONFIG *A_pstrAlarmConfig);


/*
 * Read the current Alarm 2 configuration.
 */
void HRTC_voidGetAlarm2(RTC_ALARM2_CONFIG *A_pstrAlarmConfig);


/*================ Alarm Control Functions ================*/

/*
 * Enable or disable the selected alarm interrupt.
 *
 * The DS3231 INT/SQW pin becomes active LOW when an enabled
 * alarm occurs.
 */
void HRTC_voidSetAlarmState(RTC_ALARM A_enuAlarm,
                            RTC_ALARM_STATE A_enuState);


/*
 * Check whether the selected alarm has occurred.
 *
 * Return:
 * 1 -> Alarm occurred.
 * 0 -> Alarm did not occur.
 */
u8 HRTC_u8IsAlarmTriggered(RTC_ALARM A_enuAlarm);


/*
 * Clear the selected alarm flag.
 *
 * The alarm flag must be cleared after handling the alarm
 * to release the INT/SQW pin.
 */
void HRTC_voidClearAlarmFlag(RTC_ALARM A_enuAlarm);


/*
 * Disable both alarms and clear both alarm flags.
 */
void HRTC_voidDisableAllAlarms(void);


#endif /* RTC_INTERFACE_H_ */