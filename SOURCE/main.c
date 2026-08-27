#include "../INCLUDE/INCLUDES.h"

// Global Var
s8 G_u8ScreenCounter = 0;
s8 G_u8PrevScreenCounter = -1;
s8 G_u8TempScreenCounter = 0;
u8 G_u8UserPresseKey = 'N';
u16 G_u16TempRaw = 0;
u16 G_u16HumidRaw = 0;

u8 G_u8Alarm1_Hours = 0;
u8 G_u8Alarm1_Minutes = 0;
u8 G_u8Alarm1_Seconds = 0;
u8 G_u8Alarm1_Status = 0;

f32 G_f32Temp = 0;
f32 G_f32Humid = 0;

#define KEY_OK '/'
#define KEY_BACK '='
#define KEY_NEXT '+'
#define KEY_PREV '-'
#define KEY_SETT '*'

#define TIME_DATE_SCREEN 0
#define TEMP_HUMID_SCREEN 1
#define ALARM_SCREEN 2
#define TIMER_SCREEN 3
#define STOPWATCH_SCREEN 4
#define POMODORRO_SCREEN 5
#define SETTINGS_SCREEN 6
#define RINGING_SCREEN 9

#define MAX_SCREENS 5

int main(void)
{
    MDIO_voidInit();
    HLCD_voidInit();
    MI2C_voidInit();
    HRTC_voidInit();
    HKEPAD_voidInit();
    MADC_voidInit();
    POMODORRO_voidInit();

    TIMER_APP_voidInit();
    STOPWATCH_APP_voidInit();
    MGI_voidEnable();

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
    HLCD_voidSendString((u8 *)" {v1.0} ");
    _delay_ms(1000);
    HLCD_voidClearDisplay();

    while (1)
    {
        // reading time from rtc.
        RTC_DATE_TIME current_time;
        HRTC_voidGetDateTime(&current_time);

        if (G_u8Alarm1_Status == 1 &&
            current_time.Hours == G_u8Alarm1_Hours &&
            current_time.Minutes == G_u8Alarm1_Minutes &&
            current_time.Seconds == G_u8Alarm1_Seconds &&
            G_u8ScreenCounter != RINGING_SCREEN) // Prevent re-triggering every millisecond
        {
            G_u8TempScreenCounter = G_u8ScreenCounter; // Save where the user was
            G_u8ScreenCounter = RINGING_SCREEN;        // Force screen change

            // Turn ON Buzzer on PC2
            MDIO_voidSetPinDirection(PORTC, PIN2, DIO_HIGH);
            MDIO_voidSetPinValue(PORTC, PIN2, DIO_HIGH);
        }

        G_u8UserPresseKey = HKEYPAD_u8GetPressedKey();
        if (G_u8ScreenCounter != RINGING_SCREEN && G_u8ScreenCounter <= SETTINGS_SCREEN)
        {
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
                if (G_u8ScreenCounter != POMODORRO_SCREEN)
                {
                    G_u8TempScreenCounter = G_u8ScreenCounter;
                    G_u8ScreenCounter = SETTINGS_SCREEN;
                }
                break;
            default:
                break;
            }
        }

        switch (G_u8ScreenCounter)
        {
        case TIME_DATE_SCREEN:
            HLCD_voidDisplayDateTime(current_time.Hours, current_time.Minutes,
                                     current_time.Seconds, current_time.Period, current_time.Day,
                                     current_time.Date, current_time.Month, current_time.Year,
                                     current_time.ClockMode);
            break;

        case TEMP_HUMID_SCREEN:

            G_u16TempRaw = MADC_u16GetDigitalValue(ADC0);
            G_u16HumidRaw = MADC_u16GetDigitalValue(ADC1);
            G_f32Temp = ((f32)G_u16TempRaw * 500.0f) / 1024.0f;
            G_f32Humid = ((f32)G_u16HumidRaw * 100.0f) / 1023.0f;
            // edit this so esttings struct edit the temp unit.
            HLCD_voidDisplayTempHumidity(G_f32Temp, G_f32Humid, LCD_TEMP_CELSIUS);

            break;
        case ALARM_SCREEN:
        {
            // 0=Menu, 1=Alarm1 View, 2=Alarm2 View, 3=Alarm1 Edit
            static u8 Local_AlarmState = 0;
            static u8 Local_EditBuffer[6] = {0, 0, 0, 0, 0, 0};
            static u8 Local_EditCursor = 0;

            // --- INPUT LOGIC ---
            if (Local_AlarmState == 0) // Main Menu
            {
                if (G_u8UserPresseKey == '1')
                    Local_AlarmState = 1;
                else if (G_u8UserPresseKey == '2')
                    Local_AlarmState = 2;
            }
            else if (Local_AlarmState == 1) // Alarm 1 View
            {
                if (G_u8UserPresseKey == KEY_BACK)
                {
                    Local_AlarmState = 0;
                }
                else if (G_u8UserPresseKey == KEY_OK) // Changed to '/' to avoid Settings collision
                {
                    Local_AlarmState = 3; // Enter Edit Mode
                    Local_EditCursor = 0; // Reset typing cursor
                }
                else if (G_u8UserPresseKey == 'c' || G_u8UserPresseKey == 'C')
                {
                    G_u8Alarm1_Status = !G_u8Alarm1_Status; // Toggle ON/OFF
                }
            }
            else if (Local_AlarmState == 3) // Alarm 1 Edit Mode
            {
                if (G_u8UserPresseKey == KEY_BACK)
                {
                    Local_AlarmState = 1; // Cancel edit
                }
                else if (G_u8UserPresseKey >= '0' && G_u8UserPresseKey <= '9')
                {
                    Local_EditBuffer[Local_EditCursor] = G_u8UserPresseKey - '0';
                    Local_EditCursor++;

                    // Once 6 digits are typed, save and exit edit mode
                    if (Local_EditCursor >= 6)
                    {
                        G_u8Alarm1_Hours = (Local_EditBuffer[0] * 10) + Local_EditBuffer[1];
                        G_u8Alarm1_Minutes = (Local_EditBuffer[2] * 10) + Local_EditBuffer[3];
                        G_u8Alarm1_Seconds = (Local_EditBuffer[4] * 10) + Local_EditBuffer[5];
                        Local_AlarmState = 1;
                    }
                }
            }
            else if (Local_AlarmState == 2) // Alarm 2 View
            {
                if (G_u8UserPresseKey == KEY_BACK)
                    Local_AlarmState = 0;
            }

            // --- DRAWING LOGIC ---
            if (Local_AlarmState == 0)
            {
                HLCD_voidGoToPos(L1, C1);
                HLCD_voidSendString((u8 *)"     Alarms         ");
                HLCD_voidGoToPos(L2, C1);
                HLCD_voidSendString((u8 *)"1.Alarm One         ");
                HLCD_voidGoToPos(L3, C1);
                HLCD_voidSendString((u8 *)"2.Alarm Two         ");
                HLCD_voidGoToPos(L4, C1);
                HLCD_voidSendString((u8 *)"                    ");
            }
            else if (Local_AlarmState == 1)
            {
                HLCD_voidGoToPos(L1, C1);
                HLCD_voidSendString((u8 *)"    Alarm One       ");

                // Dynamically print the saved time
                HLCD_voidGoToPos(L2, C1);
                HLCD_voidSendString((u8 *)"Time: ");
                HLCD_voidSendData((G_u8Alarm1_Hours / 10) + '0');
                HLCD_voidSendData((G_u8Alarm1_Hours % 10) + '0');
                HLCD_voidSendData(':');
                HLCD_voidSendData((G_u8Alarm1_Minutes / 10) + '0');
                HLCD_voidSendData((G_u8Alarm1_Minutes % 10) + '0');
                HLCD_voidSendData(':');
                HLCD_voidSendData((G_u8Alarm1_Seconds / 10) + '0');
                HLCD_voidSendData((G_u8Alarm1_Seconds % 10) + '0');
                HLCD_voidSendString((u8 *)"      ");

                // Dynamically print the status
                HLCD_voidGoToPos(L3, C1);
                if (G_u8Alarm1_Status == 1)
                    HLCD_voidSendString((u8 *)"/.Edit     [ ON ]   "); // Changed display text
                else
                    HLCD_voidSendString((u8 *)"/.Edit     [ OFF]   "); // Changed display text

                HLCD_voidGoToPos(L4, C1);
                HLCD_voidSendString((u8 *)"                    ");
            }
            else if (Local_AlarmState == 3)
            {
                HLCD_voidGoToPos(L1, C1);
                HLCD_voidSendString((u8 *)"  Enter Time (24h)  ");

                // Dynamically draw the typing buffer with hyphens
                HLCD_voidGoToPos(L2, C1);
                HLCD_voidSendString((u8 *)"      ");
                for (u8 i = 0; i < 6; i++)
                {
                    if (i < Local_EditCursor)
                        HLCD_voidSendData(Local_EditBuffer[i] + '0');
                    else
                        HLCD_voidSendData('-');

                    if (i == 1 || i == 3)
                        HLCD_voidSendData(':');
                }
                HLCD_voidSendString((u8 *)"      ");

                HLCD_voidGoToPos(L3, C1);
                HLCD_voidSendString((u8 *)"                    ");
                HLCD_voidGoToPos(L4, C1);
                HLCD_voidSendString((u8 *)"                    ");
            }
            else if (Local_AlarmState == 2)
            {
                HLCD_voidGoToPos(L1, C1);
                HLCD_voidSendString((u8 *)"    Alarm Two       ");
                HLCD_voidGoToPos(L2, C1);
                HLCD_voidSendString((u8 *)"Time: 00:00:00      ");
                HLCD_voidGoToPos(L3, C1);
                HLCD_voidSendString((u8 *)"/.Edit   c.ON/OFF   "); // Changed display text
                HLCD_voidGoToPos(L4, C1);
                HLCD_voidSendString((u8 *)"                    ");
            }

            break;
        }

        case RINGING_SCREEN:
        {
            static s8 Local_RingTracker = -1;

            // --- ENTRY & DRAW LOGIC ---
            if (Local_RingTracker != G_u8ScreenCounter)
            {
                // We just entered the ringing screen! Draw everything immediately.
                HLCD_voidClearDisplay();
                HLCD_voidGoToPos(L1, C1);
                HLCD_voidSendString((u8 *)" *** ALARM 1 ***    ");
                HLCD_voidGoToPos(L2, C1);
                HLCD_voidSendString((u8 *)"     IS RINGING!    ");
                HLCD_voidGoToPos(L3, C1);
                HLCD_voidSendString((u8 *)"1. Snooze (+5 Min)  ");
                HLCD_voidGoToPos(L4, C1);
                HLCD_voidSendString((u8 *)"2. Turn Off         ");

                // Lock the tracker so it doesn't flicker while ringing
                Local_RingTracker = G_u8ScreenCounter;
            }

            // --- INPUT LOGIC ---
            if (G_u8UserPresseKey == '1') // Snooze
            {
                // 1. Math to add 5 minutes safely
                G_u8Alarm1_Minutes += 1;
                if (G_u8Alarm1_Minutes >= 60)
                {
                    G_u8Alarm1_Minutes -= 60;
                    G_u8Alarm1_Hours++;
                    if (G_u8Alarm1_Hours >= 24)
                        G_u8Alarm1_Hours = 0;
                }

                // 2. Turn off the buzzer!
                MDIO_voidSetPinValue(PORTC, PIN2, DIO_LOW);

                // 3. Go back to where we were
                G_u8ScreenCounter = G_u8TempScreenCounter;

                // 4. CRITICAL FIX: Reset the tracker so it draws next time!
                Local_RingTracker = -1;
            }
            else if (G_u8UserPresseKey == '2') // Turn Off
            {
                // 1. Disable the alarm
                G_u8Alarm1_Status = 0;

                // 2. Turn off the buzzer!
                MDIO_voidSetPinValue(PORTC, PIN2, DIO_LOW);

                // 3. Go back to where we were
                G_u8ScreenCounter = G_u8TempScreenCounter;

                // 4. CRITICAL FIX: Reset the tracker so it draws next time!
                Local_RingTracker = -1;
            }

            break;
        }

        case TIMER_SCREEN:
            TIMER_APP_voidUpdate(G_u8UserPresseKey);
            break;
        case STOPWATCH_SCREEN:
            STOPWATCH_APP_voidUpdate(G_u8UserPresseKey);
            break;
        case POMODORRO_SCREEN:
        {
            if (G_u8UserPresseKey != 'N')
            {
                POMODORRO_voidHandleKeypadInput(G_u8UserPresseKey);
            }

        	        POMODORRO_voidRender();

        	        break;
        }

        default:
            break;
        }
    }
}
