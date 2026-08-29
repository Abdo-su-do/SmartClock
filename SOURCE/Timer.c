/*
 * TIMER.c
 */
#include "../INCLUDE/INCLUDES.h"
    u8 L_u8DispHours=0;
    u8 L_u8DispMinutes=0;
    u8 L_u8DispSeconds=0;

// TIMER GLB VAR
static u8 G_u8Digits[6] = {0, 0, 0, 0, 0, 0};
static volatile LCD_TIMER_STATUS G_enuTimerState = LCD_TIMER_SETTING;
static volatile u32 G_u32TotalSeconds = 0;
static volatile u32 G_u32InitialSeconds = 0;

// STOPWATCH GLB VAR

static volatile LCD_STOPWATCH_STATUS G_enuStopwatchState = LCD_STOPWATCH_OFF;
static volatile u32 G_u32StopwatchSeconds = 0;

// DesiredTime > OVF time
#define TIMER0_OVERFLOWS_PER_SECOND   31
static volatile u16 G_u16ISRCount = 0;

// Timer0 ISR Callback function
static void TIMER_APP_voidTimer0Callback(void)
{
    G_u16ISRCount++;

    if (G_u16ISRCount >= TIMER0_OVERFLOWS_PER_SECOND)
    {
        G_u16ISRCount = 0;

        if (G_enuTimerState == LCD_TIMER_RUNNING)
        {
            if (G_u32TotalSeconds > 0)
            {
                G_u32TotalSeconds--;
            }
            /* Ringing! */
            if (G_u32TotalSeconds == 0)
            {
                G_enuTimerState = LCD_TIMER_RINGING;
                MDIO_voidSetPinValue(TIMER_BUZZER_PORT, TIMER_BUZZER_PIN, DIO_HIGH);
            }
        }

        if (G_enuStopwatchState == LCD_STOPWATCH_ON)
        {
            if (G_u32StopwatchSeconds < 359999UL) // Max 99:59:59
            {
                G_u32StopwatchSeconds++;
            }
        }

        /* Update Pomodoro countdown */
        POMODORRO_voidUpdate();
    }
}


void TIMER_APP_voidInit(void)
{
    //callback & start Timer0
    MTIMER0_voidSetOVFCallback(TIMER_APP_voidTimer0Callback);
    MTIMER0_voidInit();
    MTIMER0_voidStartTimer();
    //Buzzer
    MDIO_voidSetPinDirection(TIMER_BUZZER_PORT, TIMER_BUZZER_PIN, DIO_OUTPUT);
    MDIO_voidSetPinValue(TIMER_BUZZER_PORT, TIMER_BUZZER_PIN, DIO_LOW);
}
void TIMER_APP_voidUpdate(u8 A_u8PressedKey)
{
    switch (G_enuTimerState)
    {
        case LCD_TIMER_SETTING:
            if (A_u8PressedKey >= '0' && A_u8PressedKey <= '9')
            {
                G_u8Digits[0] = G_u8Digits[1];
                G_u8Digits[1] = G_u8Digits[2];
                G_u8Digits[2] = G_u8Digits[3];
                G_u8Digits[3] = G_u8Digits[4];
                G_u8Digits[4] = G_u8Digits[5];
                G_u8Digits[5] = A_u8PressedKey - '0';
            }
            else if (A_u8PressedKey == 'C')
            {
                for (u8 Local_u8Index = 0; Local_u8Index < 6; Local_u8Index++)
                {
                    G_u8Digits[Local_u8Index] = 0;
                }
            }
            else if (A_u8PressedKey == '/')
            {
                u32 Local_u32H = (u32)G_u8Digits[0] * 10 + G_u8Digits[1];
                u32 Local_u32M = (u32)G_u8Digits[2] * 10 + G_u8Digits[3];
                u32 Local_u32S = (u32)G_u8Digits[4] * 10 + G_u8Digits[5];
                G_u32TotalSeconds = Local_u32H * 3600UL + Local_u32M * 60UL + Local_u32S;
                if (G_u32TotalSeconds > 0)
                {
                    G_u32InitialSeconds = G_u32TotalSeconds;
                    G_u16ISRCount = 0;
                    G_enuTimerState = LCD_TIMER_RUNNING;
                }
            }
            break;
        case LCD_TIMER_RUNNING:
            if (A_u8PressedKey == '/')
            {
                G_enuTimerState = LCD_TIMER_PAUSED;
            }
            else if (A_u8PressedKey == '=')
            {
                G_u32TotalSeconds = 0;
                for (u8 Local_u8Index = 0; Local_u8Index < 6; Local_u8Index++)
                {
                    G_u8Digits[Local_u8Index] = 0;
                }
                G_enuTimerState = LCD_TIMER_SETTING;
            }
            break;
        case LCD_TIMER_PAUSED:
            if (A_u8PressedKey == '/')
            {
                G_enuTimerState = LCD_TIMER_RUNNING;
            }
            else if (A_u8PressedKey == '=')
            {
                G_u32TotalSeconds = 0;
                for (u8 Local_u8Index = 0; Local_u8Index < 6; Local_u8Index++)
                {
                    G_u8Digits[Local_u8Index] = 0;
                }
                G_enuTimerState = LCD_TIMER_SETTING;
            }
            break;
        case LCD_TIMER_RINGING:
            MDIO_voidSetPinDirection(TIMER_BUZZER_PORT, TIMER_BUZZER_PIN, DIO_OUTPUT);
            MDIO_voidSetPinValue(TIMER_BUZZER_PORT, TIMER_BUZZER_PIN, DIO_HIGH);

            if (A_u8PressedKey != 'N')
            {
                MDIO_voidSetPinValue(TIMER_BUZZER_PORT, TIMER_BUZZER_PIN, DIO_LOW);

                for (u8 Local_u8Index = 0; Local_u8Index < 6; Local_u8Index++)
                {
                    G_u8Digits[Local_u8Index] = 0;
                }
                G_u32TotalSeconds = 0;
                G_enuTimerState = LCD_TIMER_SETTING;
            }
            break;
    }
    if (G_enuTimerState == LCD_TIMER_SETTING)
    {
        L_u8DispHours   = G_u8Digits[0] * 10 + G_u8Digits[1];
        L_u8DispMinutes = G_u8Digits[2] * 10 + G_u8Digits[3];
        L_u8DispSeconds = G_u8Digits[4] * 10 + G_u8Digits[5];
    }
    else
    {
        L_u8DispHours   = (u8)(G_u32TotalSeconds / 3600UL);
        L_u8DispMinutes = (u8)((G_u32TotalSeconds % 3600UL) / 60UL);
        L_u8DispSeconds = (u8)(G_u32TotalSeconds % 60UL);
    }
    // Update LCD
    HLCD_voidDisplayTimer(L_u8DispHours, L_u8DispMinutes, L_u8DispSeconds, G_enuTimerState);
}
/*=========================================*/

// Start of Stopwatch 
void STOPWATCH_APP_voidInit(void)
{
    G_enuStopwatchState = LCD_STOPWATCH_OFF;
    G_u32StopwatchSeconds = 0;
}

void STOPWATCH_APP_voidUpdate(u8 A_u8PressedKey)
{
    switch (G_enuStopwatchState)
    {
        case LCD_STOPWATCH_OFF:
            if (A_u8PressedKey == '/')
            {
                G_enuStopwatchState = LCD_STOPWATCH_ON;
            }
            else if (A_u8PressedKey == '=')
            {
                G_u32StopwatchSeconds = 0;
            }
            break;

        case LCD_STOPWATCH_ON:
            if (A_u8PressedKey == '/')
            {
                G_enuStopwatchState = LCD_STOPWATCH_PAUSED;
            }
            else if (A_u8PressedKey == '=')
            {
                G_enuStopwatchState = LCD_STOPWATCH_OFF;
                G_u32StopwatchSeconds = 0;
            }
            break;

        case LCD_STOPWATCH_PAUSED:
            if (A_u8PressedKey == '/')
            {
                G_enuStopwatchState = LCD_STOPWATCH_ON;
            }
            else if (A_u8PressedKey == '=')
            {
                G_enuStopwatchState = LCD_STOPWATCH_OFF;
                G_u32StopwatchSeconds = 0;
            }
            break;
    }

    u8 L_u8Hours   = (u8)(G_u32StopwatchSeconds / 3600UL);
    u8 L_u8Minutes = (u8)((G_u32StopwatchSeconds % 3600UL) / 60UL);
    u8 L_u8Seconds = (u8)(G_u32StopwatchSeconds % 60UL);

    HLCD_voidDisplayStopwatch(L_u8Hours, L_u8Minutes, L_u8Seconds, G_enuStopwatchState);
}