#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"

#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"
#include "../INCLUDE/MCAL/I2C/I2C_INTERFACE.h"
#include "../INCLUDE/HAL/LCD/LCD_INTERFACE.h"
#include "../INCLUDE/HAL/RTC/RTC_INTERFACE.h"
#include <util/delay.h>

/*
 * Standalone RTC & I2C test.
 * To use this, either rename main.c's main() to something else,
 * or exclude main.c from your build, and use this file instead.
 */

static void PrintTwoDigit(u8 n) {
    if (n < 10) HLCD_voidSendData('0');
    HLCD_voidDisplayNumber(n);
}

int mainAPP(void)
{
    /* 1. Initialize DIO for LCD and I2C */
    MDIO_voidInit();

    /* 2. Initialize LCD */
    HLCD_voidInit();

    /* 3. Initialize I2C and RTC */
    MI2C_voidInit();
    HRTC_voidInit();

    /* 4. Display Splash */
    HLCD_voidClearDisplay();
    HLCD_voidGoToPos(L1, C1);
    HLCD_voidSendString((u8*)" I2C & RTC Test ");
    _delay_ms(1000);

    /* 5. Set an initial mock time: 23:59:50 */
    RTC_DATE_TIME test_time;
    test_time.Hours = 23;
    test_time.Minutes = 59;
    test_time.Seconds = 50;
    test_time.ClockMode = RTC_MODE_24_HOUR;
    test_time.Period = RTC_AM; /* Ignored in 24h mode */
    test_time.Day = RTC_SUNDAY;
    test_time.Date = 31;
    test_time.Month = 12;
    test_time.Year = 2026;

    /* Write time to the RTC */
    HRTC_voidSetDateTime(&test_time);

    while(1)
    {
        /* Continuously read and display the time */
        RTC_DATE_TIME current_time;
        HRTC_voidGetDateTime(&current_time);

        HLCD_voidGoToPos(L2, C1);
        HLCD_voidSendString((u8*)"Time: ");
        PrintTwoDigit(current_time.Hours); HLCD_voidSendData(':');
        PrintTwoDigit(current_time.Minutes); HLCD_voidSendData(':');
        PrintTwoDigit(current_time.Seconds);
        HLCD_voidSendString((u8*)"   ");

        HLCD_voidGoToPos(L3, C1);
        HLCD_voidSendString((u8*)"Date: ");
        PrintTwoDigit(current_time.Date); HLCD_voidSendData('/');
        PrintTwoDigit(current_time.Month); HLCD_voidSendData('/');
        HLCD_voidDisplayNumber(current_time.Year);
        HLCD_voidSendString((u8*)"   ");

        _delay_ms(500);
    }
    
    return 0;
}

