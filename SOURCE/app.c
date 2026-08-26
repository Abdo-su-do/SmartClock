/*
 * app.c — Smart Clock Application
 *
 * Navigation:
 *   '/' = OK / Select / Confirm
 *   '*' = Back / Cancel
 *   Rotary = Scroll / Adjust
 *
 * State Structure:
 *   STATE_MENU       → top-level mode picker
 *   STATE_CLOCK      → clock/date/temp display (default)
 *   STATE_POMODORO   → pomodoro timer + tomato animation
 *   STATE_STOPWATCH  → stopwatch
 *   STATE_ALARM      → alarm set + view
 *   STATE_PASSWORD   → password gate (leads to Settings only)
 *   STATE_SETTINGS   → EEPROM-backed settings
 */

#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"
#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"
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

/* ================================================================
   STATES
================================================================ */
typedef enum {
    STATE_CLOCK = 0,
    STATE_MENU,
    STATE_POMODORO,
    STATE_STOPWATCH,
    STATE_ALARM,
    STATE_PASSWORD,
    STATE_SETTINGS
} APP_STATE;

static APP_STATE CurrentState  = STATE_CLOCK;
static APP_STATE PrevState     = STATE_MENU; /* where to return after Password */
static u8        RefreshScreen = 1;

/* ================================================================
   ROTARY (polled, no EXTI)
================================================================ */
static s16 RotaryValue = 0;

static void PollRotary(void) {
    static u8 prevA = 1;
    u8 a = MDIO_PinstatusGetPinValue(PORTD, PIN2);
    if (prevA == DIO_HIGH && a == DIO_LOW) {
        if (MDIO_PinstatusGetPinValue(PORTD, PIN3) == DIO_HIGH)
            RotaryValue++;
        else
            RotaryValue--;
    }
    prevA = a;
}

static s16 Clamp(s16 v, s16 lo, s16 hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

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
        PollRotary();
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

/* ================================================================
   MOCK DRIVERS (replace with real drivers when hardware arrives)
================================================================ */
static u8  MockH = 10, MockM = 30, MockS = 0;
static u8  MockDay = 26, MockMon = 8;
static u16 MockYear = 2026;

static void MockTick(void) {           /* called once per second */
    MockS++;
    if (MockS >= 60) { MockS = 0; MockM++; }
    if (MockM >= 60) { MockM = 0; MockH++; }
    if (MockH >= 24) { MockH = 0; }
}

/* ================================================================
   CUSTOM LCD CHARACTERS — Tomato (CGRAM char 0 & 1)
   Char 0: solid tomato
   Char 1: tomato with shine highlight
================================================================ */
static void InitTomatoChars(void) {
    /* Char 0: solid tomato */
    HLCD_voidSendCommand(0x40);        /* CGRAM addr for char 0 */
    HLCD_voidSendData(0x04);           /*   00100  stem */
    HLCD_voidSendData(0x0E);           /*   01110  top curve */
    HLCD_voidSendData(0x1F);           /*   11111  full */
    HLCD_voidSendData(0x1F);           /*   11111 */
    HLCD_voidSendData(0x1F);           /*   11111 */
    HLCD_voidSendData(0x1F);           /*   11111 */
    HLCD_voidSendData(0x0E);           /*   01110  bottom curve */
    HLCD_voidSendData(0x00);

    /* Char 1: tomato with shine */
    HLCD_voidSendCommand(0x48);        /* CGRAM addr for char 1 */
    HLCD_voidSendData(0x04);           /*   00100  stem */
    HLCD_voidSendData(0x0E);           /*   01110 */
    HLCD_voidSendData(0x1B);           /*   11011  shine spot */
    HLCD_voidSendData(0x1F);           /*   11111 */
    HLCD_voidSendData(0x1F);           /*   11111 */
    HLCD_voidSendData(0x1F);           /*   11111 */
    HLCD_voidSendData(0x0E);           /*   01110 */
    HLCD_voidSendData(0x00);

    /* Return to DDRAM mode */
    HLCD_voidGoToPos(L1, C1);
}

/* ================================================================
   SETTINGS & EEPROM
   All EEPROM calls are commented — replace when driver is available
================================================================ */
#define EE_TIME_FMT   0x10
#define EE_POMO_FLOW  0x11
#define EE_POMO_BRK   0x12
#define EE_POMO_SESS  0x13
#define EE_ALARM_H    0x14
#define EE_ALARM_M    0x15
#define EE_ALARM_EN   0x16

static u8 Fmt24       = 1;    /* 1 = 24h, 0 = 12h  */
static u8 PomoFlow    = 25;   /* minutes            */
static u8 PomoBreak   = 5;    /* minutes            */
static u8 PomoSess    = 4;    /* number of sessions */
static u8 AlarmHour   = 7;
static u8 AlarmMin    = 0;
static u8 AlarmEn     = 0;

static void LoadSettings(void) {
    /* Fmt24    = MEEPROM_u8ReadByte(EE_TIME_FMT);  */
    /* PomoFlow = MEEPROM_u8ReadByte(EE_POMO_FLOW); */
    /* PomoBreak= MEEPROM_u8ReadByte(EE_POMO_BRK);  */
    /* PomoSess = MEEPROM_u8ReadByte(EE_POMO_SESS); */
    /* AlarmHour= MEEPROM_u8ReadByte(EE_ALARM_H);   */
    /* AlarmMin = MEEPROM_u8ReadByte(EE_ALARM_M);   */
    /* AlarmEn  = MEEPROM_u8ReadByte(EE_ALARM_EN);  */
}

static void SaveSettings(void) {
    /* MEEPROM_voidWriteByte(EE_TIME_FMT,  Fmt24);    */
    /* MEEPROM_voidWriteByte(EE_POMO_FLOW, PomoFlow); */
    /* MEEPROM_voidWriteByte(EE_POMO_BRK,  PomoBreak);*/
    /* MEEPROM_voidWriteByte(EE_POMO_SESS, PomoSess); */
    /* MEEPROM_voidWriteByte(EE_ALARM_H,   AlarmHour);*/
    /* MEEPROM_voidWriteByte(EE_ALARM_M,   AlarmMin); */
    /* MEEPROM_voidWriteByte(EE_ALARM_EN,  AlarmEn);  */
}

/* ================================================================
   PASSWORD
   Uses keypad digit keys to enter each digit.
   '/' confirms each digit.   Default: 1 2 3 4
================================================================ */
static u8 Password[4]   = {1, 2, 3, 4};
static u8 PassEntered[4]= {0, 0, 0, 0};
static u8 PassIdx       = 0;
static u8 CurDigit      = 0;

static void HandlePassword(void) {
    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"Enter Settings PIN: ");
        HLCD_voidGoToPos(L3, C1); HLCD_voidSendString((u8*)"Type digit, /=OK    ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"*=Cancel            ");
        RefreshScreen = 0;
    }

    /* Number key → set current digit */
    if (LastKey >= '0' && LastKey <= '9') {
        CurDigit = LastKey - '0';
        LastKey  = KEY_NOT_PRESSED;
    }

    /* Render slots */
    HLCD_voidGoToPos(L2, C2);
    for (u8 i = 0; i < 4; i++) {
        if      (i < PassIdx)  HLCD_voidSendString((u8*)"*  ");
        else if (i == PassIdx) { HLCD_voidDisplayNumber(CurDigit); HLCD_voidSendString((u8*)"  "); }
        else                    HLCD_voidSendString((u8*)"-  ");
    }

    u8 k = ConsumeKey();
    if (k == KEY_OK) {
        PassEntered[PassIdx++] = CurDigit;
        CurDigit = 0;
        if (PassIdx >= 4) {
            u8 ok = 1;
            for (u8 i = 0; i < 4; i++) if (PassEntered[i] != Password[i]) { ok = 0; break; }
            if (ok) {
                CurrentState = STATE_SETTINGS;
            } else {
                HLCD_voidGoToPos(L3, C1); HLCD_voidSendString((u8*)"!! Wrong PIN !!     ");
                DelayAndPoll(1200);
                CurrentState = STATE_CLOCK;
            }
            RefreshScreen = 1;
        }
    }
    if (k == KEY_BACK) {
        CurrentState = STATE_CLOCK; RefreshScreen = 1;
    }
}

/* ================================================================
   STATE: CLOCK — main display
================================================================ */
static u8 PrevS = 255;

static void HandleClock(void) {
    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        HLCD_voidGoToPos(L3, C1); HLCD_voidSendString((u8*)"Temp: 25.5C  Hum:45%");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"/=Menu              ");
        RefreshScreen = 0; PrevS = 255;
    }
    if (MockS != PrevS) {
        PrevS = MockS;
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"Time: ");
        PrintTwoDigit(MockH); HLCD_voidSendData(':');
        PrintTwoDigit(MockM); HLCD_voidSendData(':');
        PrintTwoDigit(MockS); HLCD_voidSendString((u8*)"   ");
        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"Date: ");
        PrintTwoDigit(MockDay); HLCD_voidSendData('/');
        PrintTwoDigit(MockMon); HLCD_voidSendData('/');
        HLCD_voidDisplayNumber(MockYear); HLCD_voidSendString((u8*)"  ");
    }
    if (AlarmEn && MockH == AlarmHour && MockM == AlarmMin && MockS == 0) {
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"!! ALARM !!  /=stop ");
    }
    if (ConsumeKey() == KEY_OK) {
        RotaryValue = 0; CurrentState = STATE_MENU; RefreshScreen = 1;
    }
}

/* ================================================================
   STATE: MENU — top-level mode picker
================================================================ */
static const u8* MenuLabels[] = {
    (u8*)"Clock Display   ",
    (u8*)"Pomodoro Timer  ",
    (u8*)"Stopwatch       ",
    (u8*)"Alarm           ",
    (u8*)"Settings (PIN)  "
};
#define MENU_COUNT 5
static u8 PrevMenuIdx = 255;

static void HandleMenu(void) {
    RotaryValue = Clamp(RotaryValue, 0, MENU_COUNT - 1);
    u8 idx = (u8)RotaryValue;

    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"=====  Main Menu  ==");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"Rotate+/=OK  *=Clock");
        PrevMenuIdx = 255; RefreshScreen = 0;
    }
    if (idx != PrevMenuIdx) {
        PrevMenuIdx = idx;
        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"  > ");
        HLCD_voidSendString((u8*)MenuLabels[idx]);
        HLCD_voidGoToPos(L3, C1);
        HLCD_voidSendString((u8*)"    "); HLCD_voidDisplayNumber(idx + 1);
        HLCD_voidSendString((u8*)" of "); HLCD_voidDisplayNumber(MENU_COUNT);
        HLCD_voidSendString((u8*)"              ");
    }

    u8 k = ConsumeKey();
    if (k == KEY_OK) {
        RotaryValue = 0; RefreshScreen = 1;
        switch (idx) {
            case 0: CurrentState = STATE_CLOCK;     break;
            case 1: CurrentState = STATE_POMODORO;  break;
            case 2: CurrentState = STATE_STOPWATCH; break;
            case 3: CurrentState = STATE_ALARM;     break;
            case 4:
                PassIdx = 0; CurDigit = 0;
                CurrentState = STATE_PASSWORD;
                break;
        }
    }
    if (k == KEY_BACK) { CurrentState = STATE_CLOCK; RefreshScreen = 1; }
}

/* ================================================================
   STATE: POMODORO
   Sub-states: 0=set flow, 1=set break, 2=set sessions, 3=running focus,
               4=running break
================================================================ */
static void HandlePomodoro(void) {
    static u8  ps           = 0;  /* sub-state */
    static u16 TimerSec     = 0;  /* countdown in seconds */
    static u8  SessionsDone = 0;
    static u8  animFrame    = 0;
    static u8  prevTimerS   = 255;

    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        RefreshScreen = 0;
        prevTimerS = 255;
    }

    u8 k = ConsumeKey();

    /* --- Sub-state 0: Set Flow Time --- */
    if (ps == 0) {
        RotaryValue = Clamp(RotaryValue, 1, 90);
        PomoFlow = (u8)RotaryValue;
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"  Pomodoro Setup    ");
        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"Focus time (min):   ");
        HLCD_voidGoToPos(L3, C3); HLCD_voidDisplayNumber(PomoFlow);
        HLCD_voidSendString((u8*)" min          ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"Rotate + /=OK *=Back");
        if (k == KEY_OK)   { ps = 1; RotaryValue = PomoBreak; RefreshScreen = 1; }
        if (k == KEY_BACK) { ps = 0; CurrentState = STATE_MENU; RefreshScreen = 1; }
    }
    /* --- Sub-state 1: Set Break Time --- */
    else if (ps == 1) {
        RotaryValue = Clamp(RotaryValue, 1, 30);
        PomoBreak = (u8)RotaryValue;
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"  Pomodoro Setup    ");
        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"Break time (min):   ");
        HLCD_voidGoToPos(L3, C3); HLCD_voidDisplayNumber(PomoBreak);
        HLCD_voidSendString((u8*)" min          ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"Rotate + /=OK *=Back");
        if (k == KEY_OK)   { ps = 2; RotaryValue = PomoSess;  RefreshScreen = 1; }
        if (k == KEY_BACK) { ps = 0; RotaryValue = PomoFlow;  RefreshScreen = 1; }
    }
    /* --- Sub-state 2: Set Sessions --- */
    else if (ps == 2) {
        RotaryValue = Clamp(RotaryValue, 1, 10);
        PomoSess = (u8)RotaryValue;
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"  Pomodoro Setup    ");
        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"Sessions:           ");
        HLCD_voidGoToPos(L3, C3); HLCD_voidDisplayNumber(PomoSess);
        HLCD_voidSendString((u8*)" sessions     ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"Rotate + /=Start    ");
        if (k == KEY_OK) {
            ps = 3;
            SessionsDone = 0;
            TimerSec = (u16)PomoFlow * 60u;
            animFrame = 0; prevTimerS = 255;
            RefreshScreen = 1;
        }
        if (k == KEY_BACK) { ps = 1; RotaryValue = PomoBreak; RefreshScreen = 1; }
    }
    /* --- Sub-state 3: FOCUS running --- */
    else if (ps == 3) {
        /* Animate tomato every call */
        animFrame ^= 1;

        u8 rem_m = (u8)(TimerSec / 60);
        u8 rem_s = (u8)(TimerSec % 60);

        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"FOCUS ");
        PrintTwoDigit(rem_m); HLCD_voidSendData(':'); PrintTwoDigit(rem_s);
        HLCD_voidSendString((u8*)"  Sess:");
        HLCD_voidDisplayNumber(SessionsDone + 1);
        HLCD_voidSendData('/');
        HLCD_voidDisplayNumber(PomoSess);

        /* Line 2 & 3: Tomato animation (5 tomato chars in a row) */
        HLCD_voidGoToPos(L2, C8);
        for (u8 t = 0; t < 5; t++) HLCD_voidSendData(animFrame); /* custom char 0 or 1 */

        HLCD_voidGoToPos(L3, C1); HLCD_voidSendString((u8*)"Next: break ");
        HLCD_voidDisplayNumber(PomoBreak); HLCD_voidSendString((u8*)" min  ");

        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"/=Pause      *=Exit ");

        if (k == KEY_OK)   { ps = 3; /* toggle pause — extend to add pause state if needed */ }
        if (k == KEY_BACK) { ps = 0; CurrentState = STATE_MENU; RotaryValue = 0; RefreshScreen = 1; }

        /* Timer handled in APP_voidUpdate tick */
    }
    /* --- Sub-state 4: BREAK running --- */
    else if (ps == 4) {
        u8 rem_m = (u8)(TimerSec / 60);
        u8 rem_s = (u8)(TimerSec % 60);

        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"BREAK ");
        PrintTwoDigit(rem_m); HLCD_voidSendData(':'); PrintTwoDigit(rem_s);
        HLCD_voidSendString((u8*)"          ");
        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"Session ");
        HLCD_voidDisplayNumber(SessionsDone); HLCD_voidSendString((u8*)" of ");
        HLCD_voidDisplayNumber(PomoSess); HLCD_voidSendString((u8*)"  Done! ");
        HLCD_voidGoToPos(L3, C1); HLCD_voidSendString((u8*)"Relax!              ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"/=Skip break *=Exit ");

        if (k == KEY_OK)   { /* skip break → next session */
            TimerSec = 0;
        }
        if (k == KEY_BACK) { ps = 0; CurrentState = STATE_MENU; RotaryValue = 0; RefreshScreen = 1; }
    }

    /* -------- Tick the timer (driven by APP_voidUpdate seconds tick) -------- */
    if ((ps == 3 || ps == 4) && MockS != prevTimerS) {
        prevTimerS = MockS;
        if (TimerSec > 0) {
            TimerSec--;
        } else {
            /* Transition */
            if (ps == 3) {          /* focus done → break */
                SessionsDone++;
                if (SessionsDone >= PomoSess) {
                    /* All sessions done! */
                    HLCD_voidClearDisplay();
                    HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"All sessions done!  ");
                    HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"Great work!         ");
                    DelayAndPoll(2000);
                    ps = 0; CurrentState = STATE_MENU; RotaryValue = 0; RefreshScreen = 1;
                } else {
                    ps = 4; TimerSec = (u16)PomoBreak * 60u; RefreshScreen = 1;
                }
            } else {                /* break done → next focus */
                ps = 3; TimerSec = (u16)PomoFlow * 60u; RefreshScreen = 1;
            }
        }
    }
}

/* ================================================================
   STATE: STOPWATCH
================================================================ */
static void HandleStopwatch(void) {
    static u8  running   = 0;
    static u16 ticks     = 0;     /* incremented every 200ms when running */
    static u16 prevTicks = 9999;

    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"     STOPWATCH      ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"/=Start/Pause *=Back");
        ticks = 0; running = 0; prevTicks = 9999; RefreshScreen = 0;
    }

    if (running) ticks++;

    if (ticks != prevTicks) {
        prevTicks = ticks;
        u16 tot  = ticks / 5;               /* 5 × 200ms = 1 sec */
        u8  sw_h = (u8)(tot / 3600);
        u8  sw_m = (u8)((tot % 3600) / 60);
        u8  sw_s = (u8)(tot % 60);

        HLCD_voidGoToPos(L2, C4);
        PrintTwoDigit(sw_h); HLCD_voidSendData(':');
        PrintTwoDigit(sw_m); HLCD_voidSendData(':');
        PrintTwoDigit(sw_s); HLCD_voidSendString((u8*)"    ");

        HLCD_voidGoToPos(L3, C1);
        if (running) HLCD_voidSendString((u8*)"  >> RUNNING <<     ");
        else         HLCD_voidSendString((u8*)"  || PAUSED  ||     ");
    }

    u8 k = ConsumeKey();
    if (k == KEY_OK)   running = !running;
    if (k == KEY_BACK) { CurrentState = STATE_MENU; RotaryValue = 2; RefreshScreen = 1; }
}

/* ================================================================
   STATE: ALARM
   Sub-states: 0=view, 1=set hour, 2=set min
================================================================ */
static void HandleAlarm(void) {
    static u8 as = 0; /* sub-state */

    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        RefreshScreen = 0;
    }

    if (as == 0) { /* View / toggle */
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"      ALARM         ");
        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"Set: ");
        PrintTwoDigit(AlarmHour); HLCD_voidSendData(':'); PrintTwoDigit(AlarmMin);
        HLCD_voidSendString((u8*)"           ");
        HLCD_voidGoToPos(L3, C1);
        if (AlarmEn) HLCD_voidSendString((u8*)"Status: ON          ");
        else         HLCD_voidSendString((u8*)"Status: OFF         ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"/=Edit  7=ON/OFF  * ");

        u8 k = ConsumeKey();
        if (k == KEY_OK)   { as = 1; RotaryValue = AlarmHour; RefreshScreen = 1; }
        if (k == '7')      { AlarmEn = !AlarmEn; }
        if (k == KEY_BACK) { CurrentState = STATE_MENU; RotaryValue = 3; RefreshScreen = 1; }
    }
    else if (as == 1) { /* Set Hour */
        RotaryValue = Clamp(RotaryValue, 0, 23);
        AlarmHour = (u8)RotaryValue;
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"Set Alarm Hour:     ");
        HLCD_voidGoToPos(L2, C3); PrintTwoDigit(AlarmHour); HLCD_voidSendString((u8*)"  h             ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"Rotate + /=OK *=Back");
        u8 k = ConsumeKey();
        if (k == KEY_OK)   { as = 2; RotaryValue = AlarmMin; RefreshScreen = 1; }
        if (k == KEY_BACK) { as = 0; RefreshScreen = 1; }
    }
    else if (as == 2) { /* Set Minute */
        RotaryValue = Clamp(RotaryValue, 0, 59);
        AlarmMin = (u8)RotaryValue;
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"Set Alarm Minute:   ");
        HLCD_voidGoToPos(L2, C3); PrintTwoDigit(AlarmMin); HLCD_voidSendString((u8*)"  min           ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"Rotate + /=OK *=Back");
        u8 k = ConsumeKey();
        if (k == KEY_OK)   { /* SaveSettings(); */ as = 0; RefreshScreen = 1; }
        if (k == KEY_BACK) { as = 1; RotaryValue = AlarmHour; RefreshScreen = 1; }
    }
}

/* ================================================================
   STATE: SETTINGS (PIN-protected)
================================================================ */
static u8 PrevSettIdx = 255;

static void HandleSettings(void) {
    static u8 ss = 0; /* sub-state: 0=menu, 1=time format, 2=pomo flow, 3=pomo break, 4=pomo sess */

    if (ss == 0) {
        RotaryValue = Clamp(RotaryValue, 0, 4);
        u8 idx = (u8)RotaryValue;
        const u8* items[] = {
            (u8*)"Time Format     ",
            (u8*)"Pomo Flow (min) ",
            (u8*)"Pomo Break (min)",
            (u8*)"Pomo Sessions   ",
            (u8*)"Exit Settings   "
        };
        if (RefreshScreen) {
            HLCD_voidClearDisplay();
            HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"====   Settings  ===");
            HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"Rotate+/=OK  *=Exit ");
            PrevSettIdx = 255; RefreshScreen = 0;
        }
        if (idx != PrevSettIdx) {
            PrevSettIdx = idx;
            HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"> "); HLCD_voidSendString((u8*)items[idx]);
            HLCD_voidGoToPos(L3, C1);
            if      (idx == 0) { HLCD_voidSendString((u8*)"Now: "); HLCD_voidSendString(Fmt24 ? (u8*)"24h" : (u8*)"12h"); HLCD_voidSendString((u8*)"             "); }
            else if (idx == 1) { HLCD_voidSendString((u8*)"Now: "); HLCD_voidDisplayNumber(PomoFlow);  HLCD_voidSendString((u8*)" min          "); }
            else if (idx == 2) { HLCD_voidSendString((u8*)"Now: "); HLCD_voidDisplayNumber(PomoBreak); HLCD_voidSendString((u8*)" min          "); }
            else if (idx == 3) { HLCD_voidSendString((u8*)"Now: "); HLCD_voidDisplayNumber(PomoSess);  HLCD_voidSendString((u8*)" sessions     "); }
            else                 HLCD_voidSendString((u8*)"                    ");
        }
        u8 k = ConsumeKey();
        if (k == KEY_OK) {
            if      (idx == 0) { Fmt24 = !Fmt24; SaveSettings(); PrevSettIdx = 255; }
            else if (idx == 1) { ss = 1; RotaryValue = PomoFlow;  RefreshScreen = 1; }
            else if (idx == 2) { ss = 2; RotaryValue = PomoBreak; RefreshScreen = 1; }
            else if (idx == 3) { ss = 3; RotaryValue = PomoSess;  RefreshScreen = 1; }
            else { ss = 0; CurrentState = STATE_CLOCK; RefreshScreen = 1; }
        }
        if (k == KEY_BACK) { ss = 0; CurrentState = STATE_CLOCK; RefreshScreen = 1; }
    }
    else {
        const u8* label = (ss==1) ? (u8*)"Pomo Flow (min):" :
                          (ss==2) ? (u8*)"Pomo Break (min):" :
                                    (u8*)"Sessions:";
        s16 lo = (ss==3) ? 1 : 1, hi = (ss==1) ? 90 : (ss==2) ? 30 : 10;
        RotaryValue = Clamp(RotaryValue, lo, hi);
        if (RefreshScreen) { HLCD_voidClearDisplay(); RefreshScreen = 0; }
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)label);
        HLCD_voidGoToPos(L2, C3); HLCD_voidDisplayNumber((u8)RotaryValue); HLCD_voidSendString((u8*)"               ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"Rotate+/=Save *=Back");
        u8 k = ConsumeKey();
        if (k == KEY_OK) {
            if (ss==1) PomoFlow  = (u8)RotaryValue;
            if (ss==2) PomoBreak = (u8)RotaryValue;
            if (ss==3) PomoSess  = (u8)RotaryValue;
            SaveSettings();
            ss = 0; RotaryValue = 0; RefreshScreen = 1;
        }
        if (k == KEY_BACK) { ss = 0; RotaryValue = 0; RefreshScreen = 1; }
    }
}

/* ================================================================
   PUBLIC API
================================================================ */
void APP_voidInit(void) {
    LoadSettings();
    InitTomatoChars();
    CurrentState  = STATE_CLOCK;
    RefreshScreen = 1;
}

void APP_voidUpdate(void) {
    /* Tick the mock RTC at real 1-second rate (5 × 200ms loops) */
    static u8 ticks = 0;
    ticks++;
    if (ticks >= 5) { ticks = 0; MockTick(); }

    switch (CurrentState) {
        case STATE_CLOCK:     HandleClock();     break;
        case STATE_MENU:      HandleMenu();      break;
        case STATE_POMODORO:  HandlePomodoro();  break;
        case STATE_STOPWATCH: HandleStopwatch(); break;
        case STATE_ALARM:     HandleAlarm();     break;
        case STATE_PASSWORD:  HandlePassword();  break;
        case STATE_SETTINGS:  HandleSettings();  break;
        default: CurrentState = STATE_CLOCK; RefreshScreen = 1; break;
    }

    DelayAndPoll(200);
}
