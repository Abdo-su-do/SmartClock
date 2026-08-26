#include "../INCLUDE/INCLUDES.h"

static volatile u8 CurrentMode = 0;

/* Callback لنمط Normal Mode (1 ثانية) */
void TIMER0_OVF_APP(void)
{
    static u16 counter = 0;
    if(CurrentMode == 1)
    {
        counter++;
        if(counter == 3906)
        {
            MDIO_voidTogglePinValue(PORTB, PIN0); // يومض LED الأخضر
            MTIMER0_voidSetPreloadValue(192);
            counter = 0;
        }
    }
}

/* Callback لنمط CTC Mode (0.5 ثانية) */
void TIMER0_CTC_APP(void)
{
    static u16 counter = 0;
    if(CurrentMode == 2)
    {
        counter++;
        if(counter == 2000)
        {
            MDIO_voidTogglePinValue(PORTB, PIN1); // يومض LED الأصفر
            counter = 0;
        }
    }
}

int main(void)
{
    u8 key = KEY_NOT_PRESSED;

    /* 1. تهيئة الـ DIO */
    MDIO_voidInit();
    MDIO_voidSetPinDirection(PORTB, PIN0, DIO_OUTPUT); // LED OVF
    MDIO_voidSetPinDirection(PORTB, PIN1, DIO_OUTPUT); // LED CTC
    MDIO_voidSetPinDirection(PORTB, PIN3, DIO_OUTPUT); // OC0 Pin

    /* 2. تهيئة الـ LCD */
    HLCD_voidInit();

    /* 3. تسجيل الـ Callbacks وتفعيل المقاطعات */
    MTIMER0_voidSetOVFCallback(TIMER0_OVF_APP);
    MTIMER0_voidSetCTCCallback(TIMER0_CTC_APP);
    MGI_voidEnable();

    /* 4. رسالة البداية */
    HLCD_voidSendString((u8*)"Select Mode 1-5:");

    while(1)
    {
        key = HKEYPAD_u8GetPressedKey();

        if(key != KEY_NOT_PRESSED)
        {
            switch(key)
            {
                case '1': /* Normal Mode */
                    CurrentMode = 1;
                    MTIMER0_voidStopTimer();
                    MTIMER0_voidSetPreloadValue(192);
                    TCCR0 = 0x02; // Prescaler 8, Normal mode
                    SET_BIT(TIMSK, 0); // OVF Enable
                    CLR_BIT(TIMSK, 1);
                    HLCD_voidClearDisplay();
                    HLCD_voidSendString((u8*)"Mode: Normal OVF");
                    break;

                case '2': /* CTC Mode */
                    CurrentMode = 2;
                    MTIMER0_voidStopTimer();
                    MTIMER0_voidSetOCR0Value(250);
                    TCCR0 = 0x0A; // Prescaler 8, CTC mode
                    CLR_BIT(TIMSK, 0);
                    SET_BIT(TIMSK, 1); // CTC Enable
                    HLCD_voidClearDisplay();
                    HLCD_voidSendString((u8*)"Mode: CTC Match");
                    break;

                case '3': /* Fast PWM 25% */
                    CurrentMode = 3;
                    CLR_BIT(TIMSK, 0);
                    CLR_BIT(TIMSK, 1);
                    MTIMER0_voidSetOCR0Value(64); // 25%
                    TCCR0 = 0x6A; // Fast PWM, Non-Inverting, Prescaler 8
                    HLCD_voidClearDisplay();
                    HLCD_voidSendString((u8*)"Fast PWM: 25%");
                    break;

                case '4': /* Fast PWM 75% */
                    CurrentMode = 4;
                    CLR_BIT(TIMSK, 0);
                    CLR_BIT(TIMSK, 1);
                    MTIMER0_voidSetOCR0Value(191); // 75%
                    TCCR0 = 0x6A; // Fast PWM, Non-Inverting, Prescaler 8
                    HLCD_voidClearDisplay();
                    HLCD_voidSendString((u8*)"Fast PWM: 75%");
                    break;

                case '5': /* Phase Correct PWM 50% */
                    CurrentMode = 5;
                    CLR_BIT(TIMSK, 0);
                    CLR_BIT(TIMSK, 1);
                    MTIMER0_voidSetOCR0Value(128); // 50%
                    TCCR0 = 0x62; // Phase Correct, Non-Inverting, Prescaler 8
                    HLCD_voidClearDisplay();
                    HLCD_voidSendString((u8*)"Phase Corr: 50%");
                    break;
            }
        }
    }

    return 0;
}
