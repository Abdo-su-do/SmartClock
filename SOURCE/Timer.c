/*
 * TIMER.c
 */
#include "../INCLUDE/INCLUDES.h"
#include "../INCLUDE/TIMER.h"

    u8 L_u8DispHours;
    u8 L_u8DispMinutes;
    u8 L_u8DispSeconds;

static u8 G_u8Digits[6] = {0, 0, 0, 0, 0, 0};
static volatile LCD_TIMER_STATUS G_enuTimerState = LCD_TIMER_SETTING;
static volatile u32 G_u32TotalSeconds = 0;
static volatile u32 G_u32InitialSeconds = 0;

#define TIMER0_OVERFLOWS_PER_SECOND   31
static volatile u16 G_u16ISRCount = 0;

static void TIMER_APP_voidTimer0Callback(void)
{
    if (G_enuTimerState == LCD_TIMER_RUNNING)
    {
        G_u16ISRCount++;
        if (G_u16ISRCount >= TIMER0_OVERFLOWS_PER_SECOND)
        {
            G_u16ISRCount = 0;
            if (G_u32TotalSeconds > 0)
            {
                G_u32TotalSeconds--;
            }
            /* Ringing! */
            if (G_u32TotalSeconds == 0)
            {
                G_enuTimerState = LCD_TIMER_RINGING;
                MTIMER0_voidStopTimer();
                MDIO_voidSetPinValue(TIMER_BUZZER_PORT, TIMER_BUZZER_PIN, DIO_HIGH);
            }
        }
    }
}
void TIMER_APP_voidInit(void)
{
    MTIMER0_voidSetOVFCallback(TIMER_APP_voidTimer0Callback);
    MTIMER0_voidInit();
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
                for (u8 i = 0; i < 6; i++)
                {
                    G_u8Digits[i] = 0;
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
                    MTIMER0_voidStartTimer();
                }
            }
            break;
        case LCD_TIMER_RUNNING:
            if (A_u8PressedKey == '/')
            {
                G_enuTimerState = LCD_TIMER_PAUSED;
                MTIMER0_voidStopTimer();
            }
            else if (A_u8PressedKey == '=')
            {
                MTIMER0_voidStopTimer();
                G_u32TotalSeconds = 0;
                for (u8 i = 0; i < 6; i++) 
                {
                    G_u8Digits[i] = 0;
                }
                G_enuTimerState = LCD_TIMER_SETTING;
            }
            break;
        case LCD_TIMER_PAUSED:
            if (A_u8PressedKey == '/')
            {
                G_enuTimerState = LCD_TIMER_RUNNING;
                MTIMER0_voidStartTimer();
            }
            else if (A_u8PressedKey == '=')
            {
                MTIMER0_voidStopTimer();
                G_u32TotalSeconds = 0;
                for (u8 i = 0; i < 6; i++) 
                {
                    G_u8Digits[i] = 0;
                }
                G_enuTimerState = LCD_TIMER_SETTING;
            }
            break;
        case LCD_TIMER_RINGING:
            if (A_u8PressedKey != 'N')
            {
                MDIO_voidSetPinValue(TIMER_BUZZER_PORT, TIMER_BUZZER_PIN, DIO_LOW);
                for (u8 i = 0; i < 6; i++) 
                {
                    G_u8Digits[i] = 0;
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
    HLCD_voidDisplayTimer(L_u8DispHours, L_u8DispMinutes, L_u8DispSeconds, G_enuTimerState);
}