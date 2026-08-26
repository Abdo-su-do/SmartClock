/*
 * app.c - Smart Clock Application Logic
 * No interrupts used. Everything is polled.
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
   APPLICATION STATES
================================================================ */
typedef enum {
    STATE_MAIN,
    STATE_PASSWORD,
    STATE_SETTINGS,
    STATE_POMODORO,
    STATE_STOPWATCH
} APP_STATE;

static APP_STATE CurrentState = STATE_MAIN;
static u8 RefreshScreen    = 1;

/* ================================================================
   ROTARY (Polled — no EXTI interrupt needed)
================================================================ */
static volatile s16 RotaryValue = 0;

static void PollRotary(void) {
    static u8 prevA = 1;
    u8 a = MDIO_PinstatusGetPinValue(PORTD, PIN2);
    if (prevA == DIO_HIGH && a == DIO_LOW) { /* falling edge */
        u8 b = MDIO_PinstatusGetPinValue(PORTD, PIN3);
        if (b == DIO_HIGH) { RotaryValue++; }
        else               { RotaryValue--; }
    }
    prevA = a;
}

static s16 Constrain(s16 v, s16 lo, s16 hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* ================================================================
   KEYPAD (Last-key latch, cleared on read)
================================================================ */
static u8 LastKey = KEY_NOT_PRESSED;

static void PollKeypad(void) {
    u8 k = HKEYPAD_u8GetPressedKey();
    if (k != KEY_NOT_PRESSED) {
        LastKey = k;
    }
}

static u8 ConsumeKey(void) {
    u8 k = LastKey;
    LastKey = KEY_NOT_PRESSED;
    return k;
}

/* ================================================================
   COMBINED DELAY (polls both keypad and rotary every 10 ms)
================================================================ */
static void DelayAndPoll(u16 ms) {
    u16 ticks = ms / 10u;
    for (u16 i = 0; i < ticks; i++) {
        PollKeypad();
        PollRotary();
        _delay_ms(10);
    }
}

/* ================================================================
   MOCK DRIVERS
================================================================ */
static u8  MockHour = 10, MockMin = 30, MockSec = 0;
static u8  MockDay  = 26, MockMonth = 8;
static u16 MockYear = 2026;

/* Called once per second — controlled by a counter in APP_voidUpdate */
static void MockRTC_Tick(void) {
    MockSec++;
    if (MockSec >= 60) { MockSec = 0; MockMin++; }
    if (MockMin >= 60) { MockMin = 0; MockHour++; }
    if (MockHour >= 24) { MockHour = 0; }
}

/* Cheap 2-digit display helper */
static void LCD_PrintTwoDigit(u8 n) {
    if (n < 10) HLCD_voidSendData('0');
    HLCD_voidDisplayNumber(n);
}

/* ================================================================
   SETTINGS
================================================================ */
static u8 PomodoroFlow     = 25;
static u8 PomodoroBreak    = 5;
static u8 PomodoroSessions = 4;
static u8 TimeFormat24     = 1;

/*
 * Password uses KEYPAD DIGIT KEYS directly.
 * Press '1','2','3','4','0','6','7','8','9' to type a digit.
 * Press '5' to CONFIRM that digit and move to the next.
 * Default password: 1 2 3 4
 */
static u8 Password[4]    = {1, 2, 3, 4};
static u8 PassEntered[4] = {0, 0, 0, 0};
static u8 PassIndex      = 0;
static u8 CurrentDigit   = 0; /* digit currently being typed */

/* ================================================================
   STATE: MAIN SCREEN
   Only redraw when something actually changes to prevent flicker
================================================================ */
static u8 PrevSec = 255; /* force first draw */

static void HandleMain(void) {
    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        /* Static labels that never change */
        HLCD_voidGoToPos(L3, C1);
        HLCD_voidSendString((u8*)"Temp: 25.5 C        ");
        HLCD_voidGoToPos(L4, C1);
        HLCD_voidSendString((u8*)"Hum:  45.2 %  OK=5  ");
        RefreshScreen = 0;
        PrevSec = 255; /* force time redraw */
    }

    /* Only update time display when seconds change */
    if (MockSec != PrevSec) {
        PrevSec = MockSec;

        HLCD_voidGoToPos(L1, C1);
        HLCD_voidSendString((u8*)"Time: ");
        LCD_PrintTwoDigit(MockHour); HLCD_voidSendData(':');
        LCD_PrintTwoDigit(MockMin);  HLCD_voidSendData(':');
        LCD_PrintTwoDigit(MockSec);
        HLCD_voidSendString((u8*)"     ");

        HLCD_voidGoToPos(L2, C1);
        HLCD_voidSendString((u8*)"Date: ");
        LCD_PrintTwoDigit(MockDay);
        HLCD_voidSendData('/');
        LCD_PrintTwoDigit(MockMonth);
        HLCD_voidSendData('/');
        HLCD_voidDisplayNumber(MockYear);
        HLCD_voidSendString((u8*)"   ");
    }

    if (ConsumeKey() == '5') {
        PassIndex    = 0;
        CurrentDigit = 0;
        CurrentState = STATE_PASSWORD;
        RefreshScreen = 1;
    }
}

/* ================================================================
   STATE: PASSWORD
   Use number keys to type a digit. Press '5' to confirm it.
================================================================ */
static void HandlePassword(void) {
    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        HLCD_voidGoToPos(L1, C1);
        HLCD_voidSendString((u8*)"Enter Password:     ");
        HLCD_voidGoToPos(L3, C1);
        HLCD_voidSendString((u8*)"Type digit, OK=5    ");
        HLCD_voidGoToPos(L4, C1);
        HLCD_voidSendString((u8*)"Pwd: 1 2 3 4        ");
        RefreshScreen = 0;
    }

    /* Check if a numeric key was pressed to update current digit */
    u8 k = LastKey; /* peek without consuming */
    if (k >= '0' && k <= '9') {
        CurrentDigit = k - '0';
        LastKey = KEY_NOT_PRESSED; /* consume it */
    }

    /* Show slots */
    HLCD_voidGoToPos(L2, C1);
    for (u8 i = 0; i < 4; i++) {
        if      (i < PassIndex)  HLCD_voidSendString((u8*)"* ");
        else if (i == PassIndex) { HLCD_voidDisplayNumber(CurrentDigit); HLCD_voidSendData(' '); }
        else                     HLCD_voidSendString((u8*)"- ");
    }

    /* Confirm digit with '5' */
    if (ConsumeKey() == '/') {
        PassEntered[PassIndex] = CurrentDigit;
        PassIndex++;
        CurrentDigit = 0;

        if (PassIndex >= 4) {
            u8 ok = 1;
            for (u8 i = 0; i < 4; i++) {
                if (PassEntered[i] != Password[i]) { ok = 0; break; }
            }
            if (ok) {
                RotaryValue  = 0;
                CurrentState = STATE_SETTINGS;
            } else {
                HLCD_voidGoToPos(L3, C1);
                HLCD_voidSendString((u8*)"!! Wrong Password !!"); 
                DelayAndPoll(1500);
                CurrentState = STATE_MAIN;
            }
            RefreshScreen = 1;
        }
    }
}

/* ================================================================
   STATE: SETTINGS MENU
================================================================ */
static const u8* MenuItems[] = {
    (u8*)"1. Time Format  ",
    (u8*)"2. Pomodoro     ",
    (u8*)"3. Stopwatch    ",
    (u8*)"4. Exit         "
};
#define MENU_ITEMS_COUNT 4

static u8 PrevMenuIdx = 255;

static void HandleSettings(void) {
    RotaryValue = Constrain(RotaryValue, 0, MENU_ITEMS_COUNT - 1);
    u8 idx = (u8)RotaryValue;

    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        HLCD_voidGoToPos(L1, C1);
        HLCD_voidSendString((u8*)"--- Settings Menu --");
        HLCD_voidGoToPos(L4, C1);
        HLCD_voidSendString((u8*)"Rotate=scroll OK=5  ");
        PrevMenuIdx = 255;
        RefreshScreen = 0;
    }

    if (idx != PrevMenuIdx) {
        PrevMenuIdx = idx;
        HLCD_voidGoToPos(L2, C1);
        HLCD_voidSendString((u8*)"> ");
        HLCD_voidSendString((u8*)MenuItems[idx]);
    }

    if (ConsumeKey() == '5') {
        if      (idx == 0) {
            TimeFormat24 = !TimeFormat24;
            HLCD_voidGoToPos(L3, C1);
            if (TimeFormat24) HLCD_voidSendString((u8*)"Saved: 24h format   ");
            else              HLCD_voidSendString((u8*)"Saved: 12h format   ");
            DelayAndPoll(800);
            RefreshScreen = 1;
        }
        else if (idx == 1) { RotaryValue = PomodoroFlow; CurrentState = STATE_POMODORO;  RefreshScreen = 1; }
        else if (idx == 2) {                              CurrentState = STATE_STOPWATCH; RefreshScreen = 1; }
        else               { RotaryValue = 0;             CurrentState = STATE_MAIN;     RefreshScreen = 1; }
    }
}

/* ================================================================
   STATE: POMODORO
================================================================ */
static void HandlePomodoro(void) {
    static u8 pState = 0;

    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        RefreshScreen = 0;
    }

    if (pState == 0) {
        RotaryValue = Constrain(RotaryValue, 1, 60);
        PomodoroFlow = (u8)RotaryValue;
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"Set Flow Time (min):");
        HLCD_voidGoToPos(L2, C1); HLCD_voidDisplayNumber(PomodoroFlow);
        HLCD_voidSendString((u8*)" min              ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"Rotate + OK=5 to set");
        if (ConsumeKey() == '5') { pState = 1; RotaryValue = PomodoroBreak; RefreshScreen = 1; }
    }
    else if (pState == 1) {
        RotaryValue = Constrain(RotaryValue, 1, 30);
        PomodoroBreak = (u8)RotaryValue;
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"Set Break Time(min):");
        HLCD_voidGoToPos(L2, C1); HLCD_voidDisplayNumber(PomodoroBreak);
        HLCD_voidSendString((u8*)" min              ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"Rotate + OK=5 to set");
        if (ConsumeKey() == '5') {
            pState = 2; RefreshScreen = 1;
        }
    }
    else {
        static u8 anim = 0;
        anim ^= 1;
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"  Pomodoro Running  ");
        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"Flow:  ");
        HLCD_voidDisplayNumber(PomodoroFlow); HLCD_voidSendString((u8*)" min  ");
        HLCD_voidGoToPos(L3, C1); HLCD_voidSendString((u8*)"Break: ");
        HLCD_voidDisplayNumber(PomodoroBreak); HLCD_voidSendString((u8*)" min  ");
        HLCD_voidGoToPos(L3, C13);
        if (anim) HLCD_voidSendString((u8*)"(@)");
        else      HLCD_voidSendString((u8*)"(0)");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"OK=5 to exit        ");
        if (ConsumeKey() == '5') { pState = 0; CurrentState = STATE_SETTINGS; RefreshScreen = 1; }
    }
}

/* ================================================================
   STATE: STOPWATCH
================================================================ */
static void HandleStopwatch(void) {
    static u8  running  = 0;
    static u16 ticks    = 0;
    static u16 prevTicks = 0;

    if (RefreshScreen) {
        HLCD_voidClearDisplay();
        HLCD_voidGoToPos(L1, C1); HLCD_voidSendString((u8*)"    -- Stopwatch -- ");
        HLCD_voidGoToPos(L4, C1); HLCD_voidSendString((u8*)"5=Start/Pause 4=Back");
        ticks = 0; running = 0; prevTicks = 0;
        RefreshScreen = 0;
    }

    if (running) ticks++;

    /* Only redraw when ticks change */
    if (ticks != prevTicks) {
        prevTicks = ticks;
        u16 total_sec = ticks / 5; /* 200ms per tick → 5 ticks/sec */
        u8  sw_h = (u8)(total_sec / 3600);
        u8  sw_m = (u8)((total_sec % 3600) / 60);
        u8  sw_s = (u8)(total_sec % 60);

        HLCD_voidGoToPos(L2, C1); HLCD_voidSendString((u8*)"Time: ");
        LCD_PrintTwoDigit(sw_h); HLCD_voidSendData(':');
        LCD_PrintTwoDigit(sw_m); HLCD_voidSendData(':');
        LCD_PrintTwoDigit(sw_s);
        HLCD_voidSendString((u8*)"      ");

        HLCD_voidGoToPos(L3, C1);
        if (running) HLCD_voidSendString((u8*)"Status: RUNNING     ");
        else         HLCD_voidSendString((u8*)"Status: PAUSED      ");
    }

    u8 k = ConsumeKey();
    if (k == '5') { running = !running; }
    if (k == '4') { CurrentState = STATE_SETTINGS; RefreshScreen = 1; }
}

/* ================================================================
   PUBLIC API
================================================================ */
void APP_voidInit(void) {
    CurrentState  = STATE_MAIN;
    RefreshScreen = 1;
}

void APP_voidUpdate(void) {
    /* Tick the mock RTC once per second (5 * 200ms = 1s) */
    static u8 tickCounter = 0;
    tickCounter++;
    if (tickCounter >= 5) {
        tickCounter = 0;
        MockRTC_Tick();
    }

    switch (CurrentState) {
        case STATE_MAIN:      HandleMain();      break;
        case STATE_PASSWORD:  HandlePassword();  break;
        case STATE_SETTINGS:  HandleSettings();  break;
        case STATE_POMODORO:  HandlePomodoro();  break;
        case STATE_STOPWATCH: HandleStopwatch(); break;
        default:              CurrentState = STATE_MAIN; break;
    }

    DelayAndPoll(200);
}
