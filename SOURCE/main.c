#include "../INCLUDE/INCLUDES.h"

// Global Var
u8 G_u8ScreenCounter = 0;
u8 G_u8TempScreenCounter=0;
u8 G_u8UserPresseKey = 'N';
u16 G_u16TempRaw=0;
u16 G_u16HumidRaw=0;

#define KEY_OK '/'
#define KEY_BACK '='
#define KEY_NEXT '+'
#define KEY_PREV '-'
#define KEY_SETT '*'

#define TIME_DATE_SCREEN   0
#define TEMP_HUMID_SCREEN  1
#define ALARM_SCREEN       2
#define TIMER_SCREEN       3
#define STOPWATCH_SCREEN   4
#define POMODORRO_SCREEN   5
#define SETTINGS_SCREEN    6


#define MAX_SCREENS 1

int main(void)
{
    MDIO_voidInit();     
    HLCD_voidInit();     
    MI2C_voidInit();     
    HRTC_voidInit();     
    HKEPAD_voidInit();   
    MADC_voidInit();

    RTC_DATE_TIME set_time;
    set_time.Hours = 23;
    set_time.Minutes = 59;
    set_time.Seconds = 50;
    set_time.ClockMode = RTC_MODE_24_HOUR;
    set_time.Period = RTC_AM; /* Ignored in 24h mode */
    set_time.Day = RTC_SUNDAY;
    set_time.Date = 31;
    set_time.Month = 12;
    set_time.Year = 2026;

    

    /* Write time to the RTC */
    HRTC_voidSetDateTime(&set_time);

    HLCD_voidClearDisplay();
    HLCD_voidGoToPos(L1, C1);
    HLCD_voidSendString((u8*)" {v1.0} ");
    _delay_ms(1000);
    HLCD_voidClearDisplay();

    while (1)
    {
        // reading time from rtc.
        RTC_DATE_TIME current_time;
        HRTC_voidGetDateTime(&current_time);

        G_u8UserPresseKey = HKEYPAD_u8GetPressedKey();
        switch (G_u8UserPresseKey)
        {
        case KEY_NEXT:
            G_u8ScreenCounter++;
            if (G_u8ScreenCounter > MAX_SCREENS)
            {
                G_u8ScreenCounter = 0;
            }

            break;
        case KEY_PREV:
            G_u8ScreenCounter--;
            if (G_u8ScreenCounter < 0)
            {
                G_u8ScreenCounter = MAX_SCREENS;
            }
            break;
        case KEY_SETT:
            G_u8TempScreenCounter=G_u8ScreenCounter;
            G_u8ScreenCounter=SETTINGS_SCREEN;

            //callSettings(oldscreenCounter)
            break;
        default:
            break;
        }

        switch (G_u8ScreenCounter)
        {
        case TIME_DATE_SCREEN:
            HLCD_voidDisplayDateTime(current_time.Hours ,current_time.Minutes,
                current_time.Seconds,current_time.Period,current_time.Day,
                current_time.Date,current_time.Month,current_time.Year,
                current_time.ClockMode);
            break;
        
        case TEMP_HUMID_SCREEN:

            G_u16TempRaw=MADC_u16GetDigitalValue(ADC0);
            G_u16HumidRaw=MADC_u16GetDigitalValue(ADC1);
            f32 L_f32Temp=((f32)G_u16TempRaw * 500.0f) / 1024.0f;
            f32 L_f32Humid=((f32)G_u16HumidRaw * 100.0f) / 1023.0f;  
            //edit this so esttings struct edit the temp unit.
            HLCD_voidDisplayTempHumidity(L_f32Temp,L_f32Humid,LCD_TEMP_CELSIUS);

            break;

        case ALARM_SCREEN:
            //CALL THE FUNC
            break;
        case TIMER_SCREEN:
            //CALL THE FUNC
            break;
        case STOPWATCH_SCREEN:
            //CALL THE FUNC
            break;
        case POMODORRO_SCREEN:
            //CALL THE FUNC
            break;
        default:
            break;
        }


    }
}
