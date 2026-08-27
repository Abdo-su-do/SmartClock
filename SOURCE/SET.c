
/*
 * SET.c
 *
 *  Created on: Aug 27, 2026
 *      Author: MALAH
 */


#include "../INCLUDE/INCLUDES.h"

// Requires your LCD_INTERFACE.h to be included

static u8 G_u8Digits[6] = {0, 0, 0, 0, 0, 0};

static const u8 G_u8Days[7][4] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
static s8 G_s8Daycounter = 0;

static u8 G_u8DigitsDay[6] = {0, 0, 0, 0, 0, 0};

static RTC_CLOCK_MODE G_enuSetClockMode = RTC_MODE_24_HOUR;
static RTC_PERIOD     G_enuSetPeriod    = RTC_AM;


void ASET_voidSetScreen1(void)
{
    // 1. Clear any leftover text from the previous screen
    //HLCD_voidClearDisplay();

    // 2. Line 1: Centered Title
    // "SETTINGS" is 8 chars. (20 - 8) / 2 = 6 spaces padding.
    // Start at C7 (which is the 7th column).
    HLCD_voidGoToPos(L1, C1);
    HLCD_voidSendString((u8 *)"      SETTINGS      ");

    // 3. Line 2: First Option
    HLCD_voidGoToPos(L2, C1);
    HLCD_voidSendString((u8 *)"1. 12/24 Format     ");

    // 4. Line 3: Second Option
    HLCD_voidGoToPos(L3, C1);
    HLCD_voidSendString((u8 *)"2. Set Time         ");

    // 5. Line 4: Third Option
    HLCD_voidGoToPos(L4, C1);
    HLCD_voidSendString((u8 *)"3. Set Date         ");
}

void ASET_voidSetScreen2(void)
{
    // 1. Clear any leftover text from the previous screen
    //HLCD_voidClearDisplay();

    // 2. Line 1: Centered Title
    // "SETTINGS" is 8 chars. (20 - 8) / 2 = 6 spaces padding.
    // Start at C7 (which is the 7th column).
    HLCD_voidGoToPos(L1, C1);
    HLCD_voidSendString((u8 *)"      SETTINGS      ");

    // 3. Line 2: First Option
    HLCD_voidGoToPos(L2, C1);
    HLCD_voidSendString((u8 *)"4. Temp Unit        ");

    // 4. Line 3: Second Option
    HLCD_voidGoToPos(L3, C1);
    HLCD_voidSendString((u8 *)"                     ");

    // 5. Line 4: Third Option
    HLCD_voidGoToPos(L4, C1);
    HLCD_voidSendString((u8 *)"                     ");
}

void ASET_voidSetScreenTimeMode(void)
{

    HLCD_voidGoToPos(L1, C1);
    HLCD_voidSendString((u8 *)"     Time Mode      ");

    HLCD_voidGoToPos(L2, C1);
    HLCD_voidSendString((u8 *)"1. 24-Hour Format   ");

    HLCD_voidGoToPos(L3, C1);
    HLCD_voidSendString((u8 *)"2. 12-Hour Format   ");

    HLCD_voidGoToPos(L4, C1);
    HLCD_voidSendString((u8 *)"                    ");
}

void ASET_voidDisplayTimeValue(void)
{
    /* Clear line 1 */
    HLCD_voidGoToPos(L1, C1);
    HLCD_voidSendString((u8 *)"      SET TIME      ");

    /* Clear line 2 */
    HLCD_voidGoToPos(L2, C1);
    HLCD_voidSendString((u8 *)"Time: ");

    HLCD_voidSendData(G_u8Digits[0] + '0');
    HLCD_voidSendData(G_u8Digits[1] + '0');
    HLCD_voidSendData(':');

    HLCD_voidSendData(G_u8Digits[2] + '0');
    HLCD_voidSendData(G_u8Digits[3] + '0');
    HLCD_voidSendData(':');

    HLCD_voidSendData(G_u8Digits[4] + '0');
    HLCD_voidSendData(G_u8Digits[5] + '0');

    if (G_enuSetClockMode == RTC_MODE_12_HOUR)
    {
        if (G_enuSetPeriod == RTC_AM)
        {
            HLCD_voidSendString((u8 *)" AM   ");
        }
        else
        {
            HLCD_voidSendString((u8 *)" PM   ");
        }
    }else {
        HLCD_voidSendString((u8 *)"      ");
    }



    if (G_enuSetClockMode == RTC_MODE_12_HOUR)
    {
        HLCD_voidGoToPos(L3, C1);
        HLCD_voidSendString((u8 *)"+ AM      - PM      ");
    }
    else{
        HLCD_voidGoToPos(L3, C1);
        HLCD_voidSendString((u8 *)"                    ");
    }

    /* Clear line 4 */
    HLCD_voidGoToPos(L4, C1);
    HLCD_voidSendString((u8 *)"/ Save     C Clear  ");
}

void ASET_voidSetScreenTimeValue(u8 A_u8PressedKey,s8 *A_u8SetScreenCounter , RTC_CLOCK_MODE A_enuSetClockMode )
{
    G_enuSetClockMode = A_enuSetClockMode;
    u8 Local_u8Hours;
    u8 Local_u8Minutes;
    u8 Local_u8Seconds;
    u8 Local_u8ValidTime = 1;
    u8 Local_u8Counter;

    if (A_u8SetScreenCounter == NULL)
    {
        return;
    }

    /*--------------------------------------
     * Enter a digit
     *--------------------------------------*/
    if ((A_u8PressedKey >= '0') &&
        (A_u8PressedKey <= '9'))
    {
        G_u8Digits[0] = G_u8Digits[1];
        G_u8Digits[1] = G_u8Digits[2];
        G_u8Digits[2] = G_u8Digits[3];
        G_u8Digits[3] = G_u8Digits[4];
        G_u8Digits[4] = G_u8Digits[5];
        G_u8Digits[5] = A_u8PressedKey - '0';
    }

    /*--------------------------------------
     * Clear the entered time
     *--------------------------------------*/
    else if (A_u8PressedKey == 'C')
    {
        for (Local_u8Counter = 0;
             Local_u8Counter < 6;
             Local_u8Counter++)
        {
            G_u8Digits[Local_u8Counter] = 0;
        }
    }

    else if (A_u8PressedKey == '=')
    {
        *A_u8SetScreenCounter = SET_SCREEN1;
        for (Local_u8Counter = 0;
             Local_u8Counter < 6;
             Local_u8Counter++)
            {
                G_u8Digits[Local_u8Counter] = 0;
            }
    }
    /*--------------------------------------
     * Select AM using +
     *--------------------------------------*/
    else if ((A_u8PressedKey == '+') &&
             (G_enuSetClockMode == RTC_MODE_12_HOUR))
    {
        G_enuSetPeriod = RTC_AM;
    }

    /*--------------------------------------
     * Select PM using -
     *--------------------------------------*/
    else if ((A_u8PressedKey == '-') &&
             (G_enuSetClockMode == RTC_MODE_12_HOUR))
    {
        G_enuSetPeriod = RTC_PM;
    }

    /*--------------------------------------
     * Confirm using /
     *--------------------------------------*/
    else if (A_u8PressedKey == '/')
    {
        Local_u8Hours = (G_u8Digits[0] * 10U) + G_u8Digits[1];

        Local_u8Minutes = (G_u8Digits[2] * 10U) + G_u8Digits[3];

        Local_u8Seconds =  (G_u8Digits[4] * 10U) + G_u8Digits[5];

        if ((Local_u8Minutes > 59U) ||
            (Local_u8Seconds > 59U))
        {
            Local_u8ValidTime = 0;
        }

        if (G_enuSetClockMode == RTC_MODE_24_HOUR)
        {
            if (Local_u8Hours > 23U)
            {
                Local_u8ValidTime = 0;
            }
        }
        else
        {
            if ((Local_u8Hours < 1U) ||
                (Local_u8Hours > 12U))
            {
                Local_u8ValidTime = 0;
            }
        }

        /*
         * Clear all old characters on the status line.
         */


        if (Local_u8ValidTime == 1U)
        {
            HRTC_voidSetTime(
                Local_u8Hours,
                Local_u8Minutes,
                Local_u8Seconds,
                G_enuSetClockMode,
                G_enuSetPeriod
            );

            HLCD_voidGoToPos(L4, C1);
            HLCD_voidSendString((u8 *)"Time saved          ");
            _delay_ms(500);
            *A_u8SetScreenCounter = SET_SCREEN1;
            for (Local_u8Counter = 0;
             Local_u8Counter < 6;
             Local_u8Counter++)
            {
                G_u8Digits[Local_u8Counter] = 0;
            }
        }
        else
        {
            HLCD_voidGoToPos(L4, C1);
            HLCD_voidSendString((u8 *)"Invalid time        ");
            _delay_ms(500);
            for (Local_u8Counter = 0;
             Local_u8Counter < 6;
             Local_u8Counter++)
            {
                G_u8Digits[Local_u8Counter] = 0;
            }
        }
    }

     ASET_voidDisplayTimeValue();
}

void ASET_voidDisplayDateValue(void)
{
    /* Clear line 1 */
    HLCD_voidGoToPos(L1, C1);
    HLCD_voidSendString((u8 *)"      SET DATE      ");

    /* Clear line 2 */
    HLCD_voidGoToPos(L2, C1);
    HLCD_voidSendString((u8 *)"Date: ");
    HLCD_voidSendData('2');
    HLCD_voidSendData('0');
    HLCD_voidSendData(G_u8DigitsDay[0] + '0');
    HLCD_voidSendData(G_u8DigitsDay[1] + '0');
    HLCD_voidSendData('/');

    HLCD_voidSendData(G_u8DigitsDay[2] + '0');
    HLCD_voidSendData(G_u8DigitsDay[3] + '0');
    HLCD_voidSendData('/');

    HLCD_voidSendData(G_u8DigitsDay[4] + '0');
    HLCD_voidSendData(G_u8DigitsDay[5] + '0');
    HLCD_voidSendString((u8 *)"    ");

    HLCD_voidGoToPos(L3, C1);
    HLCD_voidSendString((u8 *)"(-) <-");
    HLCD_voidSendString((u8 *)"  ");
    HLCD_voidSendString((u8 *)G_u8Days[G_s8Daycounter]);
    HLCD_voidSendString((u8 *)"   ");
    HLCD_voidSendString((u8 *)"-> (+)");

    /* Clear line 4 */
    HLCD_voidGoToPos(L4, C1);
    HLCD_voidSendString((u8 *)"/ Save     C Clear  ");
}

void ASET_voidSetScreenDateValue(u8 A_u8PressedKey,s8 *A_u8SetScreenCounter )
{
    u8 Local_u8Year;
    u8 Local_u8Months;
    u8 Local_u8Date;
    u8 Local_u8ValidTime = 1;
    u8 Local_u8Counter;

    if (A_u8SetScreenCounter == NULL)
    {
        return;
    }

    /*--------------------------------------
     * Enter a digit
     *--------------------------------------*/
    if ((A_u8PressedKey >= '0') &&
        (A_u8PressedKey <= '9'))
    {
        G_u8DigitsDay[0] = G_u8DigitsDay[1];
        G_u8DigitsDay[1] = G_u8DigitsDay[2];
        G_u8DigitsDay[2] = G_u8DigitsDay[3];
        G_u8DigitsDay[3] = G_u8DigitsDay[4];
        G_u8DigitsDay[4] = G_u8DigitsDay[5];
        G_u8DigitsDay[5] = A_u8PressedKey - '0';
    }

    /*--------------------------------------
     * Clear the entered time
     *--------------------------------------*/
    else if (A_u8PressedKey == 'C')
    {
        for (Local_u8Counter = 0;
             Local_u8Counter < 6;
             Local_u8Counter++)
        {
            G_u8DigitsDay[Local_u8Counter] = 0;
        }
    }

    else if (A_u8PressedKey == '=')
    {
        *A_u8SetScreenCounter = SET_SCREEN1;
        for (Local_u8Counter = 0;
             Local_u8Counter < 6;
             Local_u8Counter++)
            {
                G_u8DigitsDay[Local_u8Counter] = 0;
            }
    }
    /*--------------------------------------
     * Select AM using +
     *--------------------------------------*/
    else if (A_u8PressedKey == '+')
    {
        G_s8Daycounter++;
        if (G_s8Daycounter > 6){
            G_s8Daycounter=0;
        }
    }

    /*--------------------------------------
     * Select PM using -
     *--------------------------------------*/
    else if ( A_u8PressedKey == '-' )
    {
        G_s8Daycounter--;
        if (G_s8Daycounter < 0){
            G_s8Daycounter=6;
        }
    }

    /*--------------------------------------
     * Confirm using /
     *--------------------------------------*/
    else if (A_u8PressedKey == '/')
    {
        Local_u8Year = (G_u8DigitsDay[0] * 10U) + G_u8DigitsDay[1];

        Local_u8Months = (G_u8DigitsDay[2] * 10U) + G_u8DigitsDay[3];

        Local_u8Date =  (G_u8DigitsDay[4] * 10U) + G_u8DigitsDay[5];

        if (Local_u8Date > 31U)
        {
            Local_u8ValidTime = 0;
        }
        if ((Local_u8Months < 1U) ||
                (Local_u8Months > 12U))
        {
            Local_u8ValidTime = 0;
        }
        if (Local_u8Year > 99U)
        {
            Local_u8ValidTime = 0;
        }
        if ((G_s8Daycounter < 0U) ||
                (G_s8Daycounter > 6U))
        {
            Local_u8ValidTime = 0;
        }



        if (Local_u8ValidTime == 1U)
        {
             HRTC_voidSetDate(
                    G_s8Daycounter+1,
                    Local_u8Date,
                    Local_u8Months,
                    Local_u8Year + 2000
                );

            HLCD_voidGoToPos(L4, C1);
            HLCD_voidSendString((u8 *)"Date saved          ");
            _delay_ms(500);
            *A_u8SetScreenCounter = SET_SCREEN1;
            for (Local_u8Counter = 0;
             Local_u8Counter < 6;
             Local_u8Counter++)
            {
                G_u8DigitsDay[Local_u8Counter] = 0;
            }
        }
        else
        {
            HLCD_voidGoToPos(L4, C1);
            HLCD_voidSendString((u8 *)"Invalid Date        ");
            _delay_ms(500);
            for (Local_u8Counter = 0;
             Local_u8Counter < 6;
             Local_u8Counter++)
            {
                G_u8DigitsDay[Local_u8Counter] = 0;
            }
        }
    }

     ASET_voidDisplayDateValue();
}

void ASET_voidSetScreenTempUnit(void)
{

    HLCD_voidGoToPos(L1, C1);
    HLCD_voidSendString((u8 *)"     Temp Unit      ");

    HLCD_voidGoToPos(L2, C1);
    HLCD_voidSendString((u8 *)"1. CELSIUS          ");

    HLCD_voidGoToPos(L3, C1);
    HLCD_voidSendString((u8 *)"2. FAHRENHEIT       ");

    HLCD_voidGoToPos(L4, C1);
    HLCD_voidSendString((u8 *)"3. KELVIN           ");
}

