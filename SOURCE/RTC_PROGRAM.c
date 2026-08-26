/*
 * RTC_PROGRAM.c
 *
 *  Created on: ??þ/??þ/????
 *      Author: ALRYADA
 */

/*
 * RTC_PROGRAM.c
 *
 * DS3231 RTC driver implementation
 */

#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"

#include "../INCLUDE/MCAL/I2C/I2C_INTERFACE.h"

#include "../INCLUDE/HAL/RTC/RTC_INTERFACE.h"
#include "../INCLUDE/HAL/RTC/RTC_PRIVATE.h"
#include "../INCLUDE/HAL/RTC/RTC_CFG.h"


/*================ Private Register Functions ================*/

static u8 RTC_u8DecimalToBCD(u8 A_u8DecimalValue);

static u8 RTC_u8BCDToDecimal(u8 A_u8BCDValue);

static u8 RTC_u8WriteRegister(u8 A_u8RegisterAddress,
                              u8 A_u8RegisterValue);

static u8 RTC_u8ReadRegister(u8 A_u8RegisterAddress,
                             u8 *A_pu8RegisterValue);

static u8 RTC_u8WriteRegisters(u8 A_u8StartRegister,
                               const u8 *A_pu8Data,
                               u8 A_u8DataLength);

static u8 RTC_u8ReadRegisters(u8 A_u8StartRegister,
                              u8 *A_pu8Data,
                              u8 A_u8DataLength);

static u8 RTC_u8EncodeHours(u8 A_u8Hours,
                            RTC_CLOCK_MODE A_enuClockMode,
                            RTC_PERIOD A_enuPeriod);

static void RTC_voidDecodeHours(u8 A_u8HoursRegister,
                                u8 *A_pu8Hours,
                                RTC_CLOCK_MODE *A_penuClockMode,
                                RTC_PERIOD *A_penuPeriod);


/*================ Initialization ================*/

void HRTC_voidInit(void)
{
    u8 Local_u8ControlRegister;

    MI2C_voidInit();

    if (RTC_u8ReadRegister(RTC_CONTROL_REG, &Local_u8ControlRegister) == 0) { return; }
    /*
     * EOSC = 0:
     * Start the DS3231 oscillator.
     */
    CLR_BIT(Local_u8ControlRegister,RTC_CONTROL_EOSC_BIT);

#if RTC_SQUARE_WAVE_MODE == RTC_SQUARE_WAVE_DISABLED
    /*
     * INTCN = 1:
     * INT/SQW pin is used for alarms.
     */
    SET_BIT(Local_u8ControlRegister,RTC_CONTROL_INTCN_BIT);
    CLR_BIT(Local_u8ControlRegister, RTC_CONTROL_RS1_BIT);
    CLR_BIT(Local_u8ControlRegister, RTC_CONTROL_RS2_BIT);
#elif RTC_SQUARE_WAVE_MODE == RTC_SQUARE_WAVE_1HZ
    CLR_BIT(Local_u8ControlRegister,RTC_CONTROL_INTCN_BIT);
    CLR_BIT(Local_u8ControlRegister, RTC_CONTROL_RS1_BIT);
    CLR_BIT(Local_u8ControlRegister, RTC_CONTROL_RS2_BIT);
#elif RTC_SQUARE_WAVE_MODE == RTC_SQUARE_WAVE_1024HZ
    CLR_BIT(Local_u8ControlRegister,RTC_CONTROL_INTCN_BIT);
    SET_BIT(Local_u8ControlRegister,RTC_CONTROL_RS1_BIT);
    CLR_BIT(Local_u8ControlRegister,RTC_CONTROL_RS2_BIT);
#elif RTC_SQUARE_WAVE_MODE == RTC_SQUARE_WAVE_4096HZ
    CLR_BIT(Local_u8ControlRegister,RTC_CONTROL_INTCN_BIT);
    CLR_BIT(Local_u8ControlRegister,RTC_CONTROL_RS1_BIT);
    SET_BIT(Local_u8ControlRegister,RTC_CONTROL_RS2_BIT);
#elif RTC_SQUARE_WAVE_MODE == RTC_SQUARE_WAVE_8192HZ
    CLR_BIT(Local_u8ControlRegister,RTC_CONTROL_INTCN_BIT);
    SET_BIT(Local_u8ControlRegister,RTC_CONTROL_RS1_BIT);
    SET_BIT(Local_u8ControlRegister,RTC_CONTROL_RS2_BIT);

#else
#error "Invalid RTC_SQUARE_WAVE_MODE configuration"
#endif

    RTC_u8WriteRegister(RTC_CONTROL_REG,Local_u8ControlRegister);
}


/*================ Time Functions ================*/

void HRTC_voidSetTime(u8 A_u8Hours, u8 A_u8Minutes,  u8 A_u8Seconds,  RTC_CLOCK_MODE A_enuClockMode, RTC_PERIOD A_enuPeriod)
{
    u8 Local_au8TimeData[3];

    if ((A_u8Seconds > 59) ||(A_u8Minutes > 59)){ return; }
    if (A_enuClockMode == RTC_MODE_24_HOUR)
    {
		 if (A_u8Hours > 23) { return;  }
    }
    else if (A_enuClockMode == RTC_MODE_12_HOUR)
    {
        if ((A_u8Hours < 1) || (A_u8Hours > 12)) {return; }
    }
    else
    {
        return;
    }

    Local_au8TimeData[0] = RTC_u8DecimalToBCD(A_u8Seconds);
    /*
     * Clear bit 7 because it is not part of seconds.
     */
    Local_au8TimeData[0] &= 0x7F;

    Local_au8TimeData[1] = RTC_u8DecimalToBCD(A_u8Minutes);
    Local_au8TimeData[1] &= 0x7F;

    Local_au8TimeData[2] =RTC_u8EncodeHours(A_u8Hours , A_enuClockMode, A_enuPeriod);

    RTC_u8WriteRegisters(RTC_SECONDS_REG,Local_au8TimeData,3);
}


void HRTC_voidGetTime(RTC_DATE_TIME *A_pstrDateTime)
{
    u8 Local_au8TimeData[3];
    if (A_pstrDateTime == NULL)
    { return; }
    if (RTC_u8ReadRegisters(RTC_SECONDS_REG, Local_au8TimeData, 3) == 0)
    {return; }

    A_pstrDateTime->Seconds =RTC_u8BCDToDecimal(Local_au8TimeData[0] & 0x7F);

    A_pstrDateTime->Minutes = RTC_u8BCDToDecimal(Local_au8TimeData[1] & 0x7F);
    RTC_voidDecodeHours(Local_au8TimeData[2],&A_pstrDateTime->Hours,&A_pstrDateTime->ClockMode,&A_pstrDateTime->Period);
}


/*================ Date Functions ================*/

void HRTC_voidSetDate(RTC_DAY A_enuDay,u8 A_u8Date,u8 A_u8Month,u16 A_u16Year){
	u8 Local_au8DateData[4];
    if ((A_enuDay < RTC_SUNDAY) ||
        (A_enuDay > RTC_SATURDAY) ||
        (A_u8Date < 1) ||
        (A_u8Date > 31) ||
        (A_u8Month < 1) ||
        (A_u8Month > 12) ||
        (A_u16Year < 2000) ||
        (A_u16Year > 2099))
    { return; }

    Local_au8DateData[0] =RTC_u8DecimalToBCD((u8)A_enuDay);
    Local_au8DateData[1] = RTC_u8DecimalToBCD(A_u8Date);
    Local_au8DateData[2] =RTC_u8DecimalToBCD(A_u8Month);
    /*
     * Century bit = 0 for years 2000 to 2099.
     */
    CLR_BIT(Local_au8DateData[2], RTC_CENTURY_BIT);
    Local_au8DateData[3] = RTC_u8DecimalToBCD((u8)(A_u16Year - 2000));
    RTC_u8WriteRegisters(RTC_DAY_REG,Local_au8DateData,4);
}


void HRTC_voidGetDate(RTC_DATE_TIME *A_pstrDateTime){
    u8 Local_au8DateData[4];
    if (A_pstrDateTime == NULL)
    { return; }
    if (RTC_u8ReadRegisters(RTC_DAY_REG, Local_au8DateData,4) == 0)
    {return; }

    A_pstrDateTime->Day = (RTC_DAY)RTC_u8BCDToDecimal(Local_au8DateData[0] & 0x07);
    A_pstrDateTime->Date = RTC_u8BCDToDecimal(Local_au8DateData[1] & 0x3F);
    A_pstrDateTime->Month =RTC_u8BCDToDecimal(Local_au8DateData[2] & 0x1F);
    A_pstrDateTime->Year =(u16)(2000 + RTC_u8BCDToDecimal(Local_au8DateData[3]));
}


/*================ Complete Date and Time ================*/

void HRTC_voidSetDateTime(const RTC_DATE_TIME *A_pstrDateTime)
{
    u8 Local_au8DateTimeData[7];
    if (A_pstrDateTime == NULL)
    { return;  }
    if ((A_pstrDateTime->Seconds > 59) ||
        (A_pstrDateTime->Minutes > 59) ||
        (A_pstrDateTime->Day < RTC_SUNDAY) ||
        (A_pstrDateTime->Day > RTC_SATURDAY) ||
        (A_pstrDateTime->Date < 1) ||
        (A_pstrDateTime->Date > 31) ||
        (A_pstrDateTime->Month < 1) ||
        (A_pstrDateTime->Month > 12) ||
        (A_pstrDateTime->Year < 2000) ||
        (A_pstrDateTime->Year > 2099))
    {  return;  }
    if (A_pstrDateTime->ClockMode == RTC_MODE_24_HOUR)
    {
        if (A_pstrDateTime->Hours > 23)
        { return; }
    }
    else if (A_pstrDateTime->ClockMode == RTC_MODE_12_HOUR)
    {
        if ((A_pstrDateTime->Hours < 1) ||
            (A_pstrDateTime->Hours > 12))
        { return;  }
    }
    else {return; }

    Local_au8DateTimeData[0] = RTC_u8DecimalToBCD(A_pstrDateTime->Seconds);
    Local_au8DateTimeData[0] &= 0x7F;
    Local_au8DateTimeData[1] =  RTC_u8DecimalToBCD(A_pstrDateTime->Minutes);
    Local_au8DateTimeData[1] &= 0x7F;
    Local_au8DateTimeData[2] = RTC_u8EncodeHours(A_pstrDateTime->Hours, A_pstrDateTime->ClockMode,  A_pstrDateTime->Period);
    Local_au8DateTimeData[3] = RTC_u8DecimalToBCD((u8)A_pstrDateTime->Day);
    Local_au8DateTimeData[4] = RTC_u8DecimalToBCD(A_pstrDateTime->Date);
    Local_au8DateTimeData[5] =RTC_u8DecimalToBCD(A_pstrDateTime->Month);
    CLR_BIT(Local_au8DateTimeData[5], RTC_CENTURY_BIT);
    Local_au8DateTimeData[6] = RTC_u8DecimalToBCD((u8)(A_pstrDateTime->Year - 2000));
    RTC_u8WriteRegisters(RTC_SECONDS_REG, Local_au8DateTimeData,7);
}


void HRTC_voidGetDateTime(RTC_DATE_TIME *A_pstrDateTime)
{
    u8 Local_au8DateTimeData[7];
    if (A_pstrDateTime == NULL)
    { return; }
    if (RTC_u8ReadRegisters(RTC_SECONDS_REG,Local_au8DateTimeData,7) == 0)
    { return; }
    A_pstrDateTime->Seconds =RTC_u8BCDToDecimal(Local_au8DateTimeData[0] & 0x7F);
    A_pstrDateTime->Minutes = RTC_u8BCDToDecimal( Local_au8DateTimeData[1] & 0x7F);
    RTC_voidDecodeHours(Local_au8DateTimeData[2], &A_pstrDateTime->Hours, &A_pstrDateTime->ClockMode,  &A_pstrDateTime->Period);
    A_pstrDateTime->Day =(RTC_DAY)RTC_u8BCDToDecimal( Local_au8DateTimeData[3] & 0x07);
    A_pstrDateTime->Date = RTC_u8BCDToDecimal( Local_au8DateTimeData[4] & 0x3F );
    A_pstrDateTime->Month = RTC_u8BCDToDecimal(  Local_au8DateTimeData[5] & 0x1F );
    A_pstrDateTime->Year =(u16)(2000 + RTC_u8BCDToDecimal(Local_au8DateTimeData[6]));
}


/*================ Alarm 1 Functions ================*/

void HRTC_voidSetAlarm1(
    const RTC_ALARM1_CONFIG *A_pstrAlarmConfig)
{
    u8 Local_au8AlarmData[4];

    if (A_pstrAlarmConfig == NULL)
    {
        return;
    }

    if ((A_pstrAlarmConfig->Seconds > 59) ||
        (A_pstrAlarmConfig->Minutes > 59))
    {
        return;
    }

    if (A_pstrAlarmConfig->ClockMode == RTC_MODE_24_HOUR)
    {
        if (A_pstrAlarmConfig->Hours > 23)
        {
            return;
        }
    }
    else if (A_pstrAlarmConfig->ClockMode ==
             RTC_MODE_12_HOUR)
    {
        if ((A_pstrAlarmConfig->Hours < 1) ||
            (A_pstrAlarmConfig->Hours > 12))
        {
            return;
        }
    }
    else
    {
        return;
    }

    if (A_pstrAlarmConfig->Mode >
        RTC_ALARM1_MATCH_DAY_TIME)
    {
        return;
    }

    Local_au8AlarmData[0] =
        RTC_u8DecimalToBCD(A_pstrAlarmConfig->Seconds);

    Local_au8AlarmData[1] =
        RTC_u8DecimalToBCD(A_pstrAlarmConfig->Minutes);

    Local_au8AlarmData[2] =
        RTC_u8EncodeHours(A_pstrAlarmConfig->Hours,
                          A_pstrAlarmConfig->ClockMode,
                          A_pstrAlarmConfig->Period);

    if (A_pstrAlarmConfig->Mode ==
        RTC_ALARM1_MATCH_DAY_TIME)
    {
        if ((A_pstrAlarmConfig->Day < RTC_SUNDAY) ||
            (A_pstrAlarmConfig->Day > RTC_SATURDAY))
        {
            return;
        }

        Local_au8AlarmData[3] =
            RTC_u8DecimalToBCD(
                (u8)A_pstrAlarmConfig->Day
            );

        /*
         * DY/DT = 1 means day-of-week mode.
         */
        SET_BIT(Local_au8AlarmData[3], 6);
    }
    else
    {
        if ((A_pstrAlarmConfig->Date < 1) ||
            (A_pstrAlarmConfig->Date > 31))
        {
            /*
             * Date is only important in date mode.
             * For the other modes, use a safe value.
             */
            Local_au8AlarmData[3] = 1;
        }
        else
        {
            Local_au8AlarmData[3] =
                RTC_u8DecimalToBCD(
                    A_pstrAlarmConfig->Date
                );
        }

        /*
         * DY/DT = 0 means date-of-month mode.
         */
        CLR_BIT(Local_au8AlarmData[3], 6);
    }

    switch (A_pstrAlarmConfig->Mode)
    {
        case RTC_ALARM1_EVERY_SECOND:

            SET_BIT(Local_au8AlarmData[0], 7);
            SET_BIT(Local_au8AlarmData[1], 7);
            SET_BIT(Local_au8AlarmData[2], 7);
            SET_BIT(Local_au8AlarmData[3], 7);

            break;

        case RTC_ALARM1_MATCH_SECONDS:

            CLR_BIT(Local_au8AlarmData[0], 7);
            SET_BIT(Local_au8AlarmData[1], 7);
            SET_BIT(Local_au8AlarmData[2], 7);
            SET_BIT(Local_au8AlarmData[3], 7);

            break;

        case RTC_ALARM1_MATCH_MINUTES_SECONDS:

            CLR_BIT(Local_au8AlarmData[0], 7);
            CLR_BIT(Local_au8AlarmData[1], 7);
            SET_BIT(Local_au8AlarmData[2], 7);
            SET_BIT(Local_au8AlarmData[3], 7);

            break;

        case RTC_ALARM1_MATCH_HOURS_MINUTES_SECONDS:

            CLR_BIT(Local_au8AlarmData[0], 7);
            CLR_BIT(Local_au8AlarmData[1], 7);
            CLR_BIT(Local_au8AlarmData[2], 7);
            SET_BIT(Local_au8AlarmData[3], 7);

            break;

        case RTC_ALARM1_MATCH_DATE_TIME:

            CLR_BIT(Local_au8AlarmData[0], 7);
            CLR_BIT(Local_au8AlarmData[1], 7);
            CLR_BIT(Local_au8AlarmData[2], 7);
            CLR_BIT(Local_au8AlarmData[3], 7);
            CLR_BIT(Local_au8AlarmData[3], 6);

            break;

        case RTC_ALARM1_MATCH_DAY_TIME:

            CLR_BIT(Local_au8AlarmData[0], 7);
            CLR_BIT(Local_au8AlarmData[1], 7);
            CLR_BIT(Local_au8AlarmData[2], 7);
            CLR_BIT(Local_au8AlarmData[3], 7);
            SET_BIT(Local_au8AlarmData[3], 6);

            break;

        default:
            return;
    }

    RTC_u8WriteRegisters(RTC_ALARM1_SECONDS_REG,
                         Local_au8AlarmData,
                         4);

    HRTC_voidClearAlarmFlag(RTC_ALARM_1);
}


void HRTC_voidGetAlarm1(
    RTC_ALARM1_CONFIG *A_pstrAlarmConfig)
{
    u8 Local_au8AlarmData[4];
    u8 Local_u8MaskBits;
    u8 Local_u8DayDateSelection;

    if (A_pstrAlarmConfig == NULL)
    {
        return;
    }

    if (RTC_u8ReadRegisters(RTC_ALARM1_SECONDS_REG,
                            Local_au8AlarmData,
                            4) == 0)
    {
        return;
    }

    A_pstrAlarmConfig->Seconds =
        RTC_u8BCDToDecimal(
            Local_au8AlarmData[0] & 0x7F
        );

    A_pstrAlarmConfig->Minutes =
        RTC_u8BCDToDecimal(
            Local_au8AlarmData[1] & 0x7F
        );

    RTC_voidDecodeHours(Local_au8AlarmData[2] & 0x7F,
                        &A_pstrAlarmConfig->Hours,
                        &A_pstrAlarmConfig->ClockMode,
                        &A_pstrAlarmConfig->Period);

    Local_u8DayDateSelection =
        GET_BIT(Local_au8AlarmData[3], 6);

    if (Local_u8DayDateSelection == 1)
    {
        A_pstrAlarmConfig->Day =
            (RTC_DAY)RTC_u8BCDToDecimal(
                Local_au8AlarmData[3] & 0x0F
            );

        A_pstrAlarmConfig->Date = 0;
    }
    else
    {
        A_pstrAlarmConfig->Date =
            RTC_u8BCDToDecimal(
                Local_au8AlarmData[3] & 0x3F
            );

        A_pstrAlarmConfig->Day = RTC_SUNDAY;
    }

    /*
     * Create a four-bit number:
     *
     * bit 0 = A1M1
     * bit 1 = A1M2
     * bit 2 = A1M3
     * bit 3 = A1M4
     */
    Local_u8MaskBits =
        (GET_BIT(Local_au8AlarmData[0], 7) << 0) |
        (GET_BIT(Local_au8AlarmData[1], 7) << 1) |
        (GET_BIT(Local_au8AlarmData[2], 7) << 2) |
        (GET_BIT(Local_au8AlarmData[3], 7) << 3);

    switch (Local_u8MaskBits)
    {
        case 0x0F:
            A_pstrAlarmConfig->Mode =
                RTC_ALARM1_EVERY_SECOND;
            break;

        case 0x0E:
            A_pstrAlarmConfig->Mode =
                RTC_ALARM1_MATCH_SECONDS;
            break;

        case 0x0C:
            A_pstrAlarmConfig->Mode =
                RTC_ALARM1_MATCH_MINUTES_SECONDS;
            break;

        case 0x08:
            A_pstrAlarmConfig->Mode =
                RTC_ALARM1_MATCH_HOURS_MINUTES_SECONDS;
            break;

        case 0x00:

            if (Local_u8DayDateSelection == 1)
            {
                A_pstrAlarmConfig->Mode =
                    RTC_ALARM1_MATCH_DAY_TIME;
            }
            else
            {
                A_pstrAlarmConfig->Mode =
                    RTC_ALARM1_MATCH_DATE_TIME;
            }

            break;

        default:
            A_pstrAlarmConfig->Mode =
                RTC_ALARM1_EVERY_SECOND;
            break;
    }
}


/*================ Alarm 2 Functions ================*/

void HRTC_voidSetAlarm2(
    const RTC_ALARM2_CONFIG *A_pstrAlarmConfig)
{
    u8 Local_au8AlarmData[3];

    if (A_pstrAlarmConfig == NULL)
    {
        return;
    }

    if (A_pstrAlarmConfig->Minutes > 59)
    {
        return;
    }

    if (A_pstrAlarmConfig->ClockMode == RTC_MODE_24_HOUR)
    {
        if (A_pstrAlarmConfig->Hours > 23)
        {
            return;
        }
    }
    else if (A_pstrAlarmConfig->ClockMode ==
             RTC_MODE_12_HOUR)
    {
        if ((A_pstrAlarmConfig->Hours < 1) ||
            (A_pstrAlarmConfig->Hours > 12))
        {
            return;
        }
    }
    else
    {
        return;
    }

    if (A_pstrAlarmConfig->Mode >
        RTC_ALARM2_MATCH_DAY_TIME)
    {
        return;
    }

    Local_au8AlarmData[0] =
        RTC_u8DecimalToBCD(A_pstrAlarmConfig->Minutes);

    Local_au8AlarmData[1] =
        RTC_u8EncodeHours(A_pstrAlarmConfig->Hours,
                          A_pstrAlarmConfig->ClockMode,
                          A_pstrAlarmConfig->Period);

    if (A_pstrAlarmConfig->Mode ==
        RTC_ALARM2_MATCH_DAY_TIME)
    {
        if ((A_pstrAlarmConfig->Day < RTC_SUNDAY) ||
            (A_pstrAlarmConfig->Day > RTC_SATURDAY))
        {
            return;
        }

        Local_au8AlarmData[2] =
            RTC_u8DecimalToBCD(
                (u8)A_pstrAlarmConfig->Day
            );

        SET_BIT(Local_au8AlarmData[2], 6);
    }
    else
    {
        if ((A_pstrAlarmConfig->Date < 1) ||
            (A_pstrAlarmConfig->Date > 31))
        {
            Local_au8AlarmData[2] = 1;
        }
        else
        {
            Local_au8AlarmData[2] =
                RTC_u8DecimalToBCD(
                    A_pstrAlarmConfig->Date
                );
        }

        CLR_BIT(Local_au8AlarmData[2], 6);
    }

    switch (A_pstrAlarmConfig->Mode)
    {
        case RTC_ALARM2_EVERY_MINUTE:

            SET_BIT(Local_au8AlarmData[0], 7);
            SET_BIT(Local_au8AlarmData[1], 7);
            SET_BIT(Local_au8AlarmData[2], 7);

            break;

        case RTC_ALARM2_MATCH_MINUTES:

            CLR_BIT(Local_au8AlarmData[0], 7);
            SET_BIT(Local_au8AlarmData[1], 7);
            SET_BIT(Local_au8AlarmData[2], 7);

            break;

        case RTC_ALARM2_MATCH_HOURS_MINUTES:

            CLR_BIT(Local_au8AlarmData[0], 7);
            CLR_BIT(Local_au8AlarmData[1], 7);
            SET_BIT(Local_au8AlarmData[2], 7);

            break;

        case RTC_ALARM2_MATCH_DATE_TIME:

            CLR_BIT(Local_au8AlarmData[0], 7);
            CLR_BIT(Local_au8AlarmData[1], 7);
            CLR_BIT(Local_au8AlarmData[2], 7);
            CLR_BIT(Local_au8AlarmData[2], 6);

            break;

        case RTC_ALARM2_MATCH_DAY_TIME:

            CLR_BIT(Local_au8AlarmData[0], 7);
            CLR_BIT(Local_au8AlarmData[1], 7);
            CLR_BIT(Local_au8AlarmData[2], 7);
            SET_BIT(Local_au8AlarmData[2], 6);

            break;

        default:
            return;
    }

    RTC_u8WriteRegisters(RTC_ALARM2_MINUTES_REG,
                         Local_au8AlarmData,
                         3);

    HRTC_voidClearAlarmFlag(RTC_ALARM_2);
}


void HRTC_voidGetAlarm2(
    RTC_ALARM2_CONFIG *A_pstrAlarmConfig)
{
    u8 Local_au8AlarmData[3];
    u8 Local_u8MaskBits;
    u8 Local_u8DayDateSelection;

    if (A_pstrAlarmConfig == NULL)
    {
        return;
    }

    if (RTC_u8ReadRegisters(RTC_ALARM2_MINUTES_REG,
                            Local_au8AlarmData,
                            3) == 0)
    {
        return;
    }

    A_pstrAlarmConfig->Minutes =
        RTC_u8BCDToDecimal(
            Local_au8AlarmData[0] & 0x7F
        );

    RTC_voidDecodeHours(Local_au8AlarmData[1] & 0x7F,
                        &A_pstrAlarmConfig->Hours,
                        &A_pstrAlarmConfig->ClockMode,
                        &A_pstrAlarmConfig->Period);

    Local_u8DayDateSelection =
        GET_BIT(Local_au8AlarmData[2], 6);

    if (Local_u8DayDateSelection == 1)
    {
        A_pstrAlarmConfig->Day =
            (RTC_DAY)RTC_u8BCDToDecimal(
                Local_au8AlarmData[2] & 0x0F
            );

        A_pstrAlarmConfig->Date = 0;
    }
    else
    {
        A_pstrAlarmConfig->Date =
            RTC_u8BCDToDecimal(
                Local_au8AlarmData[2] & 0x3F
            );

        A_pstrAlarmConfig->Day = RTC_SUNDAY;
    }

    /*
     * bit 0 = A2M2
     * bit 1 = A2M3
     * bit 2 = A2M4
     */
    Local_u8MaskBits =
        (GET_BIT(Local_au8AlarmData[0], 7) << 0) |
        (GET_BIT(Local_au8AlarmData[1], 7) << 1) |
        (GET_BIT(Local_au8AlarmData[2], 7) << 2);

    switch (Local_u8MaskBits)
    {
        case 0x07:
            A_pstrAlarmConfig->Mode =
                RTC_ALARM2_EVERY_MINUTE;
            break;

        case 0x06:
            A_pstrAlarmConfig->Mode =
                RTC_ALARM2_MATCH_MINUTES;
            break;

        case 0x04:
            A_pstrAlarmConfig->Mode =
                RTC_ALARM2_MATCH_HOURS_MINUTES;
            break;

        case 0x00:

            if (Local_u8DayDateSelection == 1)
            {
                A_pstrAlarmConfig->Mode =
                    RTC_ALARM2_MATCH_DAY_TIME;
            }
            else
            {
                A_pstrAlarmConfig->Mode =
                    RTC_ALARM2_MATCH_DATE_TIME;
            }

            break;

        default:
            A_pstrAlarmConfig->Mode =
                RTC_ALARM2_EVERY_MINUTE;
            break;
    }
}


/*================ Alarm Control Functions ================*/

void HRTC_voidSetAlarmState(RTC_ALARM A_enuAlarm,
                            RTC_ALARM_STATE A_enuState)
{
    u8 Local_u8ControlRegister;

    if ((A_enuAlarm != RTC_ALARM_1) &&
        (A_enuAlarm != RTC_ALARM_2))
    {
        return;
    }

    if ((A_enuState != RTC_ALARM_DISABLED) &&
        (A_enuState != RTC_ALARM_ENABLED))
    {
        return;
    }

    if (RTC_u8ReadRegister(RTC_CONTROL_REG,
                           &Local_u8ControlRegister) == 0)
    {
        return;
    }

    if (A_enuState == RTC_ALARM_ENABLED)
    {
        /*
         * INTCN must equal 1 to use INT/SQW
         * as an alarm interrupt output.
         */
        SET_BIT(Local_u8ControlRegister,
                RTC_CONTROL_INTCN_BIT);

        if (A_enuAlarm == RTC_ALARM_1)
        {
            SET_BIT(Local_u8ControlRegister,
                    RTC_CONTROL_A1IE_BIT);
        }
        else
        {
            SET_BIT(Local_u8ControlRegister,
                    RTC_CONTROL_A2IE_BIT);
        }
    }
    else
    {
        if (A_enuAlarm == RTC_ALARM_1)
        {
            CLR_BIT(Local_u8ControlRegister,
                    RTC_CONTROL_A1IE_BIT);
        }
        else
        {
            CLR_BIT(Local_u8ControlRegister,
                    RTC_CONTROL_A2IE_BIT);
        }
    }

    RTC_u8WriteRegister(RTC_CONTROL_REG,
                        Local_u8ControlRegister);
}


u8 HRTC_u8IsAlarmTriggered(RTC_ALARM A_enuAlarm)
{
    u8 Local_u8StatusRegister;

    if (RTC_u8ReadRegister(RTC_STATUS_REG,
                           &Local_u8StatusRegister) == 0)
    {
        return 0;
    }

    if (A_enuAlarm == RTC_ALARM_1)
    {
        return GET_BIT(Local_u8StatusRegister,
                       RTC_STATUS_A1F_BIT);
    }
    else if (A_enuAlarm == RTC_ALARM_2)
    {
        return GET_BIT(Local_u8StatusRegister,
                       RTC_STATUS_A2F_BIT);
    }

    return 0;
}


void HRTC_voidClearAlarmFlag(RTC_ALARM A_enuAlarm)
{
    u8 Local_u8StatusRegister;

    if ((A_enuAlarm != RTC_ALARM_1) &&
        (A_enuAlarm != RTC_ALARM_2))
    {
        return;
    }

    if (RTC_u8ReadRegister(RTC_STATUS_REG,
                           &Local_u8StatusRegister) == 0)
    {
        return;
    }

    if (A_enuAlarm == RTC_ALARM_1)
    {
        CLR_BIT(Local_u8StatusRegister,
                RTC_STATUS_A1F_BIT);
    }
    else
    {
        CLR_BIT(Local_u8StatusRegister,
                RTC_STATUS_A2F_BIT);
    }

    RTC_u8WriteRegister(RTC_STATUS_REG,
                        Local_u8StatusRegister);
}


void HRTC_voidDisableAllAlarms(void)
{
    u8 Local_u8ControlRegister;
    u8 Local_u8StatusRegister;

    if (RTC_u8ReadRegister(RTC_CONTROL_REG,
                           &Local_u8ControlRegister) == 1)
    {
        CLR_BIT(Local_u8ControlRegister,
                RTC_CONTROL_A1IE_BIT);

        CLR_BIT(Local_u8ControlRegister,
                RTC_CONTROL_A2IE_BIT);

        RTC_u8WriteRegister(RTC_CONTROL_REG,
                            Local_u8ControlRegister);
    }

    if (RTC_u8ReadRegister(RTC_STATUS_REG,
                           &Local_u8StatusRegister) == 1)
    {
        CLR_BIT(Local_u8StatusRegister,
                RTC_STATUS_A1F_BIT);

        CLR_BIT(Local_u8StatusRegister,
                RTC_STATUS_A2F_BIT);

        RTC_u8WriteRegister(RTC_STATUS_REG,
                            Local_u8StatusRegister);
    }
}


/*================ Private Conversion Functions ================*/

static u8 RTC_u8DecimalToBCD(u8 A_u8DecimalValue)
{
    return (u8)(((A_u8DecimalValue / 10) << 4) |
                 (A_u8DecimalValue % 10));
}


static u8 RTC_u8BCDToDecimal(u8 A_u8BCDValue)
{
    return (u8)(((A_u8BCDValue >> 4) * 10) +
                 (A_u8BCDValue & 0x0F));
}


/*================ Private Hour Functions ================*/

static u8 RTC_u8EncodeHours(u8 A_u8Hours,
                            RTC_CLOCK_MODE A_enuClockMode,
                            RTC_PERIOD A_enuPeriod)
{
    u8 Local_u8HoursRegister;

    Local_u8HoursRegister =
        RTC_u8DecimalToBCD(A_u8Hours);

    if (A_enuClockMode == RTC_MODE_12_HOUR)
    {
        SET_BIT(Local_u8HoursRegister,
                RTC_HOUR_MODE_BIT);

        if (A_enuPeriod == RTC_PM)
        {
            SET_BIT(Local_u8HoursRegister,
                    RTC_AM_PM_BIT);
        }
        else
        {
            CLR_BIT(Local_u8HoursRegister,
                    RTC_AM_PM_BIT);
        }
    }
    else
    {
        CLR_BIT(Local_u8HoursRegister,
                RTC_HOUR_MODE_BIT);
    }

    return Local_u8HoursRegister;
}


static void RTC_voidDecodeHours(
    u8 A_u8HoursRegister,
    u8 *A_pu8Hours,
    RTC_CLOCK_MODE *A_penuClockMode,
    RTC_PERIOD *A_penuPeriod)
{
    if ((A_pu8Hours == NULL) ||
        (A_penuClockMode == NULL) ||
        (A_penuPeriod == NULL))
    {
        return;
    }

    if (GET_BIT(A_u8HoursRegister,
                RTC_HOUR_MODE_BIT) == 1)
    {
        *A_penuClockMode = RTC_MODE_12_HOUR;

        if (GET_BIT(A_u8HoursRegister,
                    RTC_AM_PM_BIT) == 1)
        {
            *A_penuPeriod = RTC_PM;
        }
        else
        {
            *A_penuPeriod = RTC_AM;
        }

        *A_pu8Hours =
            RTC_u8BCDToDecimal(
                A_u8HoursRegister & 0x1F
            );
    }
    else
    {
        *A_penuClockMode = RTC_MODE_24_HOUR;
        *A_penuPeriod = RTC_AM;

        *A_pu8Hours =
            RTC_u8BCDToDecimal(
                A_u8HoursRegister & 0x3F
            );
    }
}


/*================ Private I2C Functions ================*/

static u8 RTC_u8WriteRegister(u8 A_u8RegisterAddress,
                              u8 A_u8RegisterValue)
{
    I2C_ERROR_STATUS Local_enuErrorStatus;

    Local_enuErrorStatus =
        MI2C_enuSendStartCondition();

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuSendSlaveAddressWithWrite(
            RTC_SLAVE_ADDRESS
        );

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuMasterWriteDataByte(
            A_u8RegisterAddress
        );

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuMasterWriteDataByte(
            A_u8RegisterValue
        );

    MI2C_voidSendStopCondition();

    if (Local_enuErrorStatus != I2C_OK)
    {
        return 0;
    }

    return 1;
}


static u8 RTC_u8ReadRegister(u8 A_u8RegisterAddress,
                             u8 *A_pu8RegisterValue)
{
    I2C_ERROR_STATUS Local_enuErrorStatus;

    if (A_pu8RegisterValue == NULL)
    {
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuSendStartCondition();

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuSendSlaveAddressWithWrite(
            RTC_SLAVE_ADDRESS
        );

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuMasterWriteDataByte(
            A_u8RegisterAddress
        );

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuSendRepeatedStartCondition();

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuSendSlaveAddressWithRead(
            RTC_SLAVE_ADDRESS
        );

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuMasterReadDataByteWithNACK(
            A_pu8RegisterValue
        );

    MI2C_voidSendStopCondition();

    if (Local_enuErrorStatus != I2C_OK)
    {
        return 0;
    }

    return 1;
}


static u8 RTC_u8WriteRegisters(u8 A_u8StartRegister,
                               const u8 *A_pu8Data,
                               u8 A_u8DataLength)
{
    u8 Local_u8Counter;
    I2C_ERROR_STATUS Local_enuErrorStatus;

    if ((A_pu8Data == NULL) ||
        (A_u8DataLength == 0))
    {
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuSendStartCondition();

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuSendSlaveAddressWithWrite(
            RTC_SLAVE_ADDRESS
        );

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuMasterWriteDataByte(
            A_u8StartRegister
        );

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    for (Local_u8Counter = 0;
         Local_u8Counter < A_u8DataLength;
         Local_u8Counter++)
    {
        Local_enuErrorStatus =
            MI2C_enuMasterWriteDataByte(
                A_pu8Data[Local_u8Counter]
            );

        if (Local_enuErrorStatus != I2C_OK)
        {
            MI2C_voidSendStopCondition();
            return 0;
        }
    }

    MI2C_voidSendStopCondition();

    return 1;
}


static u8 RTC_u8ReadRegisters(u8 A_u8StartRegister,u8 *A_pu8Data,u8 A_u8DataLength)
{
    u8 Local_u8Counter;
    I2C_ERROR_STATUS Local_enuErrorStatus;

    if ((A_pu8Data == NULL) ||
        (A_u8DataLength == 0))
    {
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuSendStartCondition();

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuSendSlaveAddressWithWrite(
            RTC_SLAVE_ADDRESS
        );

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuMasterWriteDataByte(
            A_u8StartRegister
        );

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuSendRepeatedStartCondition();

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    Local_enuErrorStatus =
        MI2C_enuSendSlaveAddressWithRead(
            RTC_SLAVE_ADDRESS
        );

    if (Local_enuErrorStatus != I2C_OK)
    {
        MI2C_voidSendStopCondition();
        return 0;
    }

    for (Local_u8Counter = 0;
         Local_u8Counter < A_u8DataLength;
         Local_u8Counter++)
    {
        if (Local_u8Counter <
            (A_u8DataLength - 1))
        {
            Local_enuErrorStatus =
                MI2C_enuMasterReadDataByteWithACK(
                    &A_pu8Data[Local_u8Counter]
                );
        }
        else
        {
            Local_enuErrorStatus =
                MI2C_enuMasterReadDataByteWithNACK(
                    &A_pu8Data[Local_u8Counter]
                );
        }

        if (Local_enuErrorStatus != I2C_OK)
        {
            MI2C_voidSendStopCondition();
            return 0;
        }
    }

    MI2C_voidSendStopCondition();

    return 1;

}
