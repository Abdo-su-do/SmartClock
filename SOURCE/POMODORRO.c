#include "../INCLUDE/INCLUDES.h"

#include "../INCLUDE/POMODORRO.h"


static u8 G_u8CurrentMode = POMO_MODE_WORK;
static LCD_POMODORO_STATUS G_enuStatus = LCD_POMODORO_OFF;

static u8 G_u8CfgWorkMins = POMO_WORK_MINUTES;
static u8 G_u8CfgBreakMins = POMO_BREAK_MINUTES;
static u8 G_u8CfgLongBreakMins = POMO_LONG_BREAK_MINUTES;

static u8 G_u8WorkMins = POMO_WORK_MINUTES;
static u8 G_u8WorkSecs = POMO_WORK_SECONDS;
static u8 G_u8BreakMins = POMO_BREAK_MINUTES;
static u8 G_u8BreakSecs = POMO_BREAK_SECONDS;

static u8 G_u8CompletedSessions = 0;
static u8 G_u8BuzzerTimer = 0;

static u8 G_u8SettingEditIndex = 0; // 0=none, 1=work, 2=break, 3=lbreak
static u8 G_u8SettingBuffer[2] = {0, 0};
static u8 G_u8SettingCursor = 0;

void POMODORRO_voidInit(void)
{
    POMODORRO_voidReset();
}

void POMODORRO_voidUpdate(void)
{
    if (G_u8BuzzerTimer > 0)
    {
        G_u8BuzzerTimer--;
        if (G_u8BuzzerTimer == 0)
        {
            MDIO_voidSetPinValue(PORTC, PIN2, DIO_LOW);
        }
    }

    if (G_enuStatus == LCD_POMODORO_ON)
    {
        if (G_u8CurrentMode == POMO_MODE_WORK)
        {
            if (G_u8WorkSecs == 0)
            {
                if (G_u8WorkMins == 0)
                {
                    G_u8CompletedSessions++;

                    G_u8BuzzerTimer = 2;
                    MDIO_voidSetPinDirection(PORTC, PIN2, DIO_OUTPUT);
                    MDIO_voidSetPinValue(PORTC, PIN2, DIO_HIGH);

                    if (G_u8CompletedSessions >= POMO_MAX_SESSIONS)
                    {
                        G_u8CurrentMode = POMO_MODE_LONG_BREAK;
                        G_u8BreakMins = G_u8CfgLongBreakMins;
                        G_u8BreakSecs = POMO_LONG_BREAK_SECONDS;
                        G_u8CompletedSessions = 0;
                    }
                    else
                    {
                        G_u8CurrentMode = POMO_MODE_SHORT_BREAK;
                        G_u8BreakMins = G_u8CfgBreakMins;
                        G_u8BreakSecs = POMO_BREAK_SECONDS;
                    }
                    G_enuStatus = LCD_POMODORO_ON;
                }
                else
                {
                    G_u8WorkMins--;
                    G_u8WorkSecs = 59;
                }
            }
            else
            {
                G_u8WorkSecs--;
            }
        }
        else // break mode
        {
            if (G_u8BreakSecs == 0)
            {
                if (G_u8BreakMins == 0)
                {
                    G_u8CurrentMode = POMO_MODE_WORK;
                    G_u8WorkMins = G_u8CfgWorkMins;
                    G_u8WorkSecs = POMO_WORK_SECONDS;

                    G_u8BuzzerTimer = 2;
                    MDIO_voidSetPinDirection(PORTC, PIN2, DIO_OUTPUT);
                    MDIO_voidSetPinValue(PORTC, PIN2, DIO_HIGH);
                    
                    G_enuStatus = LCD_POMODORO_OFF;
                }
                else
                {
                    G_u8BreakMins--;
                    G_u8BreakSecs = 59;
                }
            }
            else
            {
                G_u8BreakSecs--;
            }
        }
    }
}

void POMODORRO_voidStartPause(void)
{
    if (G_enuStatus == LCD_POMODORO_OFF)
    {
        G_enuStatus = LCD_POMODORO_ON;
    }
    else
    {
        G_enuStatus = LCD_POMODORO_OFF;
    }
}

void POMODORRO_voidReset(void)
{
    G_enuStatus = LCD_POMODORO_OFF;
    G_u8CurrentMode = POMO_MODE_WORK;
    G_u8CompletedSessions = 0;

    G_u8WorkMins = G_u8CfgWorkMins;
    G_u8WorkSecs = POMO_WORK_SECONDS;
    G_u8BreakMins = G_u8CfgBreakMins;
    G_u8BreakSecs = POMO_BREAK_SECONDS;
}

void POMODORRO_voidHandleKeypadInput(u8 a_u8Key)
{
    if (a_u8Key == '*')
    {
        if (G_u8CurrentMode != POMO_MODE_SETTING)
        {
            G_u8CurrentMode = POMO_MODE_SETTING;
            G_u8SettingEditIndex = 1;
            G_u8SettingCursor = 0;
            G_enuStatus = LCD_POMODORO_OFF;
        }
        else
        {
            G_u8SettingEditIndex++;
            if (G_u8SettingEditIndex > 3)
            {
                G_u8CurrentMode = POMO_MODE_WORK;
                G_u8SettingEditIndex = 0;
                POMODORRO_voidReset();
            }
            G_u8SettingCursor = 0;
        }
        return;
    }

    if (G_u8CurrentMode == POMO_MODE_SETTING)
    {
        if (a_u8Key >= '0' && a_u8Key <= '9')
        {
            G_u8SettingBuffer[G_u8SettingCursor] = a_u8Key - '0';
            G_u8SettingCursor++;
            if (G_u8SettingCursor >= 2)
            {
                u8 val = G_u8SettingBuffer[0] * 10 + G_u8SettingBuffer[1];
                if (G_u8SettingEditIndex == 1) G_u8CfgWorkMins = val;
                else if (G_u8SettingEditIndex == 2) G_u8CfgBreakMins = val;
                else if (G_u8SettingEditIndex == 3) G_u8CfgLongBreakMins = val;
                
                G_u8SettingCursor = 0;
                G_u8SettingEditIndex++;
                if (G_u8SettingEditIndex > 3)
                {
                    G_u8CurrentMode = POMO_MODE_WORK;
                    G_u8SettingEditIndex = 0;
                    POMODORRO_voidReset();
                }
            }
        }
        else if (a_u8Key == '=')
        {
            G_u8CurrentMode = POMO_MODE_WORK;
            G_u8SettingEditIndex = 0;
            POMODORRO_voidReset();
        }
    }
    else
    {
        if (a_u8Key == '/')
        {
            POMODORRO_voidStartPause();
        }
        else if (a_u8Key == '=')
        {
            POMODORRO_voidReset();
        }
    }
}

void POMODORRO_voidRender(void)
{
    if (G_u8CurrentMode == POMO_MODE_SETTING)
    {
        HLCD_voidGoToPos(L1, C1);
        HLCD_voidSendString((u8 *)" Pomodoro Settings  ");

        HLCD_voidGoToPos(L2, C1);
        HLCD_voidSendString((u8 *)"1.Work : ");
        if (G_u8SettingEditIndex == 1 && G_u8SettingCursor == 0) HLCD_voidSendString((u8 *)"--");
        else if (G_u8SettingEditIndex == 1 && G_u8SettingCursor == 1) { HLCD_voidSendData(G_u8SettingBuffer[0]+'0'); HLCD_voidSendData('-'); }
        else { HLCD_voidSendData(G_u8CfgWorkMins/10+'0'); HLCD_voidSendData(G_u8CfgWorkMins%10+'0'); }
        HLCD_voidSendString((u8 *)"         ");

        HLCD_voidGoToPos(L3, C1);
        HLCD_voidSendString((u8 *)"2.Break: ");
        if (G_u8SettingEditIndex == 2 && G_u8SettingCursor == 0) HLCD_voidSendString((u8 *)"--");
        else if (G_u8SettingEditIndex == 2 && G_u8SettingCursor == 1) { HLCD_voidSendData(G_u8SettingBuffer[0]+'0'); HLCD_voidSendData('-'); }
        else { HLCD_voidSendData(G_u8CfgBreakMins/10+'0'); HLCD_voidSendData(G_u8CfgBreakMins%10+'0'); }
        HLCD_voidSendString((u8 *)"         ");

        HLCD_voidGoToPos(L4, C1);
        HLCD_voidSendString((u8 *)"3.L.Brk: ");
        if (G_u8SettingEditIndex == 3 && G_u8SettingCursor == 0) HLCD_voidSendString((u8 *)"--");
        else if (G_u8SettingEditIndex == 3 && G_u8SettingCursor == 1) { HLCD_voidSendData(G_u8SettingBuffer[0]+'0'); HLCD_voidSendData('-'); }
        else { HLCD_voidSendData(G_u8CfgLongBreakMins/10+'0'); HLCD_voidSendData(G_u8CfgLongBreakMins%10+'0'); }
        HLCD_voidSendString((u8 *)"         ");

        return;
    }

    u8 displayBreakMins = G_u8BreakMins;
    u8 displayBreakSecs = G_u8BreakSecs;

    if (G_u8CurrentMode == POMO_MODE_LONG_BREAK && G_enuStatus == LCD_POMODORO_OFF && G_u8BreakMins == G_u8CfgBreakMins)
    {
        displayBreakMins = G_u8CfgLongBreakMins;
        displayBreakSecs = POMO_LONG_BREAK_SECONDS;
    }

    HLCD_voidDisplayPomodoro(G_u8WorkMins, G_u8WorkSecs, displayBreakMins, displayBreakSecs, G_enuStatus);
}
