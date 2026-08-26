/*
 * app.c — Smart Clock Application (20x4 LCD version)
 *
 * Navigation:
 *   '+' / '-' = Next/Prev Screen OR Increment/Decrement value
 *   '/'       = OK / Select / Confirm
 *   '*'       = Back / Cancel
 *   '='       = Go to Settings (Password protected)
 */

#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"
#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"
#include "../INCLUDE/MCAL/EEPROM/EEPROM_INTERFACE.h"
#include "../INCLUDE/HAL/LCD/LCD_INTERFACE.h"
#include "../INCLUDE/HAL/KEYPAD/KEYPAD_INTERFACE.h"
#include "../INCLUDE/HAL/KEYPAD/KEYPAD_CFG.h"
#include "../INCLUDE/app.h"
#include <util/delay.h>

/* ================================================================
   KEYS
================================================================ */
#define KEY_OK     '/'
#define KEY_BACK   '*'
#define KEY_NEXT   '+'
#define KEY_PREV   '-'
#define KEY_SETT   '='

/* ================================================================
   STATES
================================================================ */
typedef enum {
    STATE_CLOCK = 0,
    STATE_POMODORO,
    STATE_STOPWATCH,
    STATE_ALARM,
    NUM_MAIN_SCREENS
} MAIN_SCREEN_T;

static MAIN_SCREEN_T CurrentScreen = STATE_CLOCK;

typedef enum {
    APP_MODE_MAIN,
    APP_MODE_PASSWORD,
    APP_MODE_SETTINGS
} APP_MODE_T;

static APP_MODE_T CurrentMode = APP_MODE_MAIN;
static u8 RefreshScreen = 1;

/* ================================================================
   KEYPAD (latch + consume)
================================================================ */
static u8 LastKey = KEY_NOT_PRESSED;

static void PollKeypad(void) {
    u8 k = HKEYPAD_u8GetPressedKey();
    if (k != KEY_NOT_PRESSED) LastKey = k;
}

static u8 ConsumeKey(void) {
    u8 k = LastKey;
    LastKey = KEY_NOT_PRESSED;
    return k;
}

/* ================================================================
   DELAY + POLL (200 ms chunks, polls every 10 ms)
================================================================ */
static void DelayAndPoll(u16 ms) {
    for (u16 i = 0; i < ms / 10u; i++) {
        PollKeypad();
        _delay_ms(10);
    }
}

/* ================================================================
   HELPERS
================================================================ */
static void PrintTwoDigit(u8 n) {
    if (n < 10) HLCD_voidSendData('0');
    HLCD_voidDisplayNumber(n);
}

static s16 Clamp(s16 v, s16 lo, s16 hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* ================================================================
   MOCK RTC DRIVER (replace with real driver later)
================================================================ */
static u8  MockH = 10, MockM = 30, MockS = 0;
static u8  MockDay = 26, MockMon = 8;
static u16 MockYear = 2026;

static void MockTick(void) {
    MockS++;
    if (MockS >= 60) { MockS = 0; MockM++; }
    if (MockM >= 60) { MockM = 0; MockH++; }
    if (MockH >= 24) { MockH = 0; }
}

/* ================================================================
   TOMATO ANIMATION
================================================================ */
static void InitTomatoChars(void) {
    HLCD_voidSendCommand(0x40); /* CGRAM char 0 */
    HLCD_voidSendData(0x04); HLCD_voidSendData(0x0E); HLCD_voidSendData(0x1F);
    HLCD_voidSendData(0x1F); HLCD_voidSendData(0x1F); HLCD_voidSendData(0x1F);
    HLCD_voidSendData(0x0E); HLCD_voidSendData(0x00);
    HLCD_voidSendCommand(0x48); /* CGRAM char 1 */
    HLCD_voidSendData(0x04); HLCD_voidSendData(0x0E); HLCD_voidSendData(0x1B);
    HLCD_voidSendData(0x1F); HLCD_voidSendData(0x1F); HLCD_voidSendData(0x1F);
    HLCD_voidSendData(0x0E); HLCD_voidSendData(0x00);
    HLCD_voidGoToPos(L1, C1);
}

/* ================================================================
   SETTINGS (EEPROM)
================================================================ */
static u8 Fmt24       = 1;
static u8 PomoFlow    = 25;
static u8 PomoBreak   = 5;
static u8 PomoSess    = 4;
static u8 AlarmHour   = 7;
static u8 AlarmMin    = 0;
static u8 AlarmEn     = 0;

static void LoadSettings(void) {
    /* MEEPROM_EEPROM_ErrorStatusReadByte(0x10, &Fmt24); */
}
static void SaveSettings(void) {
    /* MEEPROM_EEPROM_ErrorStatusWriteByte(0x10, Fmt24); */
}

/* ================================================================
   BACKGROUND TASKS (Stopwatch & Pomodoro)
================================================================ */
static u8  SW_Running = 0;
static u16 SW_Ticks = 0;

static u8  Pomo_Running = 0;   /* 0=Off, 1=Focus, 2=Break */
static u16 Pomo_TimerSec = 0;
static u8  Pomo_SessionsDone = 0;
static u8  Pomo_NotifyFlag = 0; /* 1=Focus done, 2=Break done, 3=All done */

static void BackgroundTick(void) {
    if (SW_Running) SW_Ticks++;

    static u8 prevS = 255;
    if (Pomo_Running && MockS != prevS) {
        prevS = MockS;
        if (Pomo_TimerSec > 0) {
            Pomo_TimerSec--;
        } else {
            if (Pomo_Running == 1) { 
                Pomo_SessionsDone++;
                if (Pomo_SessionsDone >= PomoSess) {
                    Pomo_Running = 0;
                    Pomo_NotifyFlag = 3; 
                } else {
                    Pomo_Running = 2; 
                    Pomo_TimerSec = (u16)PomoBreak * 60u;
                    Pomo_NotifyFlag = 1;
                }
            } else if (Pomo_Running == 2) { 
                Pomo_Running = 1; 
                Pomo_TimerSec = (u16)PomoFlow * 60u;
                Pomo_NotifyFlag = 2;
            }
        }
    }
}

static void HandleNotifications(void) {
    if (Pomo_NotifyFlag) {
        HLCD_voidClearDisplay();
        HLCD_voidGoToPos(L2, C1);
        if (Pomo_NotifyFlag == 3) HLCD_voidSendString((u8*)"  Pomodoro Finish!  ");
        if (Pomo_NotifyFlag == 1) HLCD_voidSendString((u8*)"  Time for Break!   ");
        if (Pomo_NotifyFlag == 2) HLCD_voidSendString((u8*)"  Back to Work!     ");
        
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"    Press / to OK   ");
        while(1) {
            DelayAndPoll(100);
            if (ConsumeKey() == KEY_OK) break;
        }
        Pomo_NotifyFlag = 0;
        RefreshScreen = 1;
    }
    
    if (AlarmEn && MockH == AlarmHour && MockM == AlarmMin && MockS == 0) {
        HLCD_voidClearDisplay();
        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"  ** ALARM !! **    ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"   Press / to Stop  ");
        while(1) {
            DelayAndPoll(100);
            if (ConsumeKey() == KEY_OK) break;
        }
        RefreshScreen = 1;
    }
}

/* ================================================================
   CLOCK SCREEN (20x4)
================================================================ */
static void Screen_Clock(u8 k) {
    static u8 prevS = 255;
    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)" (+/-) Nav (=) Setg");
        RefreshScreen = 0; prevS = 255;
    }
    if (MockS != prevS) {
        prevS = MockS;
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"Time: ");
        PrintTwoDigit(MockH); HLCD_voidSendData(':');
        PrintTwoDigit(MockM); HLCD_voidSendData(':');
        PrintTwoDigit(MockS); 
        
        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"Date: ");
        PrintTwoDigit(MockDay); HLCD_voidSendData('/');
        PrintTwoDigit(MockMon); HLCD_voidSendData('/');
        HLCD_voidDisplayNumber(MockYear);

        HLCD_voidGoToPos(L3, C1); HLCD_voidSendString((u8*)"Temp: 25.5C Hum: 45%");
    }
    
    if (k == KEY_NEXT) { CurrentScreen = STATE_POMODORO; RefreshScreen = 1; }
    if (k == KEY_PREV) { CurrentScreen = STATE_ALARM; RefreshScreen = 1; }
}

/* ================================================================
   POMODORO SCREEN (20x4)
================================================================ */
static void Screen_Pomodoro(u8 k) {
    static u8 setupStep = 0; /* 0=view, 1=set flow, 2=set break, 3=set sess */
    static u8 editVal = 0;
    static u8 animFrame = 0;

    if (RefreshScreen) { HLCD_voidClearDisplay(); RefreshScreen = 0; }

    if (Pomo_Running) {
        animFrame ^= 1;
        u8 rem_m = (u8)(Pomo_TimerSec / 60);
        u8 rem_s = (u8)(Pomo_TimerSec % 60);
        
        HLCD_voidGoToPos(L1, C1);
        if (Pomo_Running == 1) HLCD_voidSendString((u8*)"FOCUS TIMER         ");
        else                   HLCD_voidSendString((u8*)"BREAK TIMER         ");
        
        HLCD_voidGoToPos(L2, C1);
        HLCD_voidSendString((u8*)"Time Left:  ");
        PrintTwoDigit(rem_m); HLCD_voidSendData(':'); PrintTwoDigit(rem_s);
        HLCD_voidSendString((u8*)" ");
        
        HLCD_voidGoToPos(L3, C1);
        HLCD_voidSendString((u8*)"Session:    "); HLCD_voidDisplayNumber(Pomo_SessionsDone+1);
        HLCD_voidSendData('/'); HLCD_voidDisplayNumber(PomoSess);
        HLCD_voidSendString((u8*)"   ");
        if (Pomo_Running == 1) {
            HLCD_voidSendData(animFrame); HLCD_voidSendData(animFrame); HLCD_voidSendData(animFrame);
        } else {
            HLCD_voidSendString((u8*)"   ");
        }
        
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"/=Stop Timer  (+/-) ");
        
        if (k == KEY_OK) { Pomo_Running = 0; RefreshScreen = 1; }
        if (k == KEY_NEXT) { CurrentScreen = STATE_STOPWATCH; RefreshScreen = 1; }
        if (k == KEY_PREV) { CurrentScreen = STATE_CLOCK; RefreshScreen = 1; }
        return;
    }

    if (setupStep == 0) {
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"   Pomodoro Timer   ");
        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"Focus: "); PrintTwoDigit(PomoFlow); HLCD_voidSendString((u8*)"m Brk: "); PrintTwoDigit(PomoBreak); HLCD_voidSendData('m');
        HLCD_voidGoToPos(L3, C1); HLCD_voidSendString((u8*)"Total Sessions: "); PrintTwoDigit(PomoSess);
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"/=Start *=Set  (+/-)");
        
        if (k == KEY_OK) {
            Pomo_Running = 1; Pomo_SessionsDone = 0;
            Pomo_TimerSec = (u16)PomoFlow * 60u;
            RefreshScreen = 1;
        }
        if (k == KEY_BACK) { setupStep = 1; editVal = PomoFlow; RefreshScreen = 1; }
        if (k == KEY_NEXT) { CurrentScreen = STATE_STOPWATCH; RefreshScreen = 1; }
        if (k == KEY_PREV) { CurrentScreen = STATE_CLOCK; RefreshScreen = 1; }
    } else {
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"  Pomodoro Setup    ");
        
        HLCD_voidGoToPos(L2, C1);
        if      (setupStep == 1) HLCD_voidSendString((u8*)"Set Focus (min):    ");
        else if (setupStep == 2) HLCD_voidSendString((u8*)"Set Break (min):    ");
        else if (setupStep == 3) HLCD_voidSendString((u8*)"Set Sessions:       ");
        
        HLCD_voidGoToPos(L3, C3);
        HLCD_voidSendString((u8*)"> "); PrintTwoDigit(editVal); HLCD_voidSendString((u8*)"     ");
        
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)" (+/-) Adj   /=Next ");
        
        if (k == KEY_NEXT) editVal++;
        if (k == KEY_PREV) editVal--;
        if (k == KEY_BACK) { setupStep = 0; RefreshScreen = 1; }
        
        if (k == KEY_OK) {
            if      (setupStep == 1) { PomoFlow = editVal;  setupStep = 2; editVal = PomoBreak; }
            else if (setupStep == 2) { PomoBreak = editVal; setupStep = 3; editVal = PomoSess; }
            else if (setupStep == 3) { PomoSess = editVal;  setupStep = 0; }
            RefreshScreen = 1;
        }
    }
}

/* ================================================================
   STOPWATCH SCREEN (20x4)
================================================================ */
static void Screen_Stopwatch(u8 k) {
    static u16 prevTicks = 9999;
    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"     STOPWATCH      ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"/=Play/Pause *=Rst  ");
        prevTicks = 9999; RefreshScreen = 0;
    }
    
    if (SW_Ticks != prevTicks) {
        prevTicks = SW_Ticks;
        u16 tot = SW_Ticks / 5;
        HLCD_voidGoToPos(L2, C6);
        PrintTwoDigit((u8)(tot/3600)); HLCD_voidSendData(':');
        PrintTwoDigit((u8)((tot%3600)/60)); HLCD_voidSendData(':');
        PrintTwoDigit((u8)(tot%60));
        
        HLCD_voidGoToPos(L3, C1);
        if(SW_Running) HLCD_voidSendString((u8*)"   >> RUNNING <<    ");
        else           HLCD_voidSendString((u8*)"   || PAUSED  ||    ");
    }
    
    if (k == KEY_OK)   SW_Running = !SW_Running;
    if (k == KEY_BACK) { SW_Running = 0; SW_Ticks = 0; RefreshScreen = 1; }
    if (k == KEY_NEXT) { CurrentScreen = STATE_ALARM; RefreshScreen = 1; }
    if (k == KEY_PREV) { CurrentScreen = STATE_POMODORO; RefreshScreen = 1; }
}

/* ================================================================
   ALARM SCREEN (20x4)
================================================================ */
static void Screen_Alarm(u8 k) {
    static u8 as = 0;
    static u8 editVal = 0;
    
    if (RefreshScreen) { HLCD_voidClearDisplay(); RefreshScreen = 0; }
    
    if (as == 0) {
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"   Daily Alarm      ");
        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"Set Time:  ");
        PrintTwoDigit(AlarmHour); HLCD_voidSendData(':'); PrintTwoDigit(AlarmMin);
        
        HLCD_voidGoToPos(L3, C1);
        if (AlarmEn) HLCD_voidSendString((u8*)"Status:    [ ON ]   ");
        else         HLCD_voidSendString((u8*)"Status:    [ OFF ]  ");
        
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"/=Tgl *=Edit  (+/-) ");
        
        if (k == KEY_OK)   { AlarmEn = !AlarmEn; RefreshScreen = 1; }
        if (k == KEY_BACK) { as = 1; editVal = AlarmHour; RefreshScreen = 1; }
        if (k == KEY_NEXT) { CurrentScreen = STATE_CLOCK; RefreshScreen = 1; }
        if (k == KEY_PREV) { CurrentScreen = STATE_STOPWATCH; RefreshScreen = 1; }
    } else {
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"  Alarm Setup       ");
        
        HLCD_voidGoToPos(L2, C1);
        if (as == 1) HLCD_voidSendString((u8*)"Set Hour (0-23):    ");
        if (as == 2) HLCD_voidSendString((u8*)"Set Minute (0-59):  ");
        
        HLCD_voidGoToPos(L3, C5);
        HLCD_voidSendString((u8*)"> "); PrintTwoDigit(editVal); HLCD_voidSendString((u8*)"   ");
        
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)" (+/-) Adj   /=Next ");
        
        if (k == KEY_NEXT) editVal++;
        if (k == KEY_PREV) editVal--;
        if (as == 1) editVal %= 24;
        if (as == 2) editVal %= 60;
        if (k == KEY_BACK) { as = 0; RefreshScreen = 1; }
        
        if (k == KEY_OK) {
            if (as == 1) { AlarmHour = editVal; as = 2; editVal = AlarmMin; }
            else         { AlarmMin = editVal; as = 0; }
            RefreshScreen = 1;
        }
    }
}

/* ================================================================
   PASSWORD SCREEN (20x4)
================================================================ */
static void Mode_Password(u8 k) {
    static u8 pwd[4] = {1,2,3,4};
    static u8 ent[4] = {0,0,0,0};
    static u8 idx = 0;
    static u8 cur = 0;
    
    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)" Restricted Access  ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"/=OK *=Exit  Type 0-9");
        RefreshScreen = 0;
    }
    
    if (k >= '0' && k <= '9') cur = k - '0';
    
    HLCD_voidGoToPos(L2, C5);
    for(u8 i=0; i<4; i++) {
        if(i < idx) HLCD_voidSendString((u8*)"*  ");
        else if (i == idx) { HLCD_voidDisplayNumber(cur); HLCD_voidSendString((u8*)"  "); }
        else HLCD_voidSendString((u8*)"-  ");
    }
    
    if (k == KEY_OK) {
        ent[idx++] = cur; cur = 0;
        if (idx == 4) {
            u8 match = 1;
            for(u8 i=0; i<4; i++) if (ent[i] != pwd[i]) match = 0;
            if (match) CurrentMode = APP_MODE_SETTINGS;
            else {
                HLCD_voidGoToPos(L3, C3); HLCD_voidSendString((u8*)"!! Wrong PIN !! ");
                DelayAndPoll(1000);
                CurrentMode = APP_MODE_MAIN;
            }
            idx = 0; RefreshScreen = 1;
        }
    }
    if (k == KEY_BACK) { idx = 0; cur = 0; CurrentMode = APP_MODE_MAIN; RefreshScreen = 1; }
}

/* ================================================================
   SETTINGS SCREEN (20x4)
================================================================ */
static void Mode_Settings(u8 k) {
    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"  System Settings   ");
        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"1. Time Format: ");
        HLCD_voidSendString(Fmt24 ? (u8*)"24h" : (u8*)"12h");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"Press 1 to Tgl *=Exit");
        RefreshScreen = 0;
    }
    
    if (k == '1') {
        Fmt24 = !Fmt24; SaveSettings();
        HLCD_voidGoToPos(L3, C1);
        if(Fmt24) HLCD_voidSendString((u8*)"   Saved: 24-hour   ");
        else      HLCD_voidSendString((u8*)"   Saved: 12-hour   ");
        DelayAndPoll(1000); RefreshScreen = 1;
    }
    if (k == KEY_BACK) { CurrentMode = APP_MODE_MAIN; RefreshScreen = 1; }
}

/* ================================================================
   PUBLIC API
================================================================ */
void APP_voidInit(void) {
    LoadSettings();
    InitTomatoChars();
    CurrentMode = APP_MODE_MAIN;
    CurrentScreen = STATE_CLOCK;
    RefreshScreen = 1;
}

void APP_voidUpdate(void) {
    static u8 ticks = 0;
    ticks++;
    if (ticks >= 5) { ticks = 0; MockTick(); }
    
    BackgroundTick();
    HandleNotifications();
    
    u8 k = ConsumeKey();
    
    if (CurrentMode == APP_MODE_MAIN) {
        if (k == KEY_SETT) { CurrentMode = APP_MODE_PASSWORD; RefreshScreen = 1; k = KEY_NOT_PRESSED; }
    }
    
    switch (CurrentMode) {
        case APP_MODE_MAIN:
            switch (CurrentScreen) {
                case STATE_CLOCK:     Screen_Clock(k);     break;
                case STATE_POMODORO:  Screen_Pomodoro(k);  break;
                case STATE_STOPWATCH: Screen_Stopwatch(k); break;
                case STATE_ALARM:     Screen_Alarm(k);     break;
                default: CurrentScreen = STATE_CLOCK; break;
            }
            break;
            
        case APP_MODE_PASSWORD: Mode_Password(k); break;
        case APP_MODE_SETTINGS: Mode_Settings(k); break;
    }
    
    DelayAndPoll(200);
}
