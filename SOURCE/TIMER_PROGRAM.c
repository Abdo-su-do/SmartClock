/* TIMER_PROGRAM.c */
#include "../INCLUDE/lib/BIT_MATH.h"
#include "../INCLUDE/lib/STD_TYPES.h"

// DIO Directives
#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"

#include "../INCLUDE/MCAL/TIMER/TIMER_PRIVATE.h"
#include "../INCLUDE/MCAL/TIMER/TIMER_INTERFACE.h"
#include "../INCLUDE/MCAL/TIMER/TIMER_CFG.h"

#include <avr/interrupt.h>

static void (*TIMER0_OVF_CALLBACK)(void) = NULL;
static void (*TIMER0_CTC_CALLBACK)(void) = NULL;

void MTIMER0_voidInit(void)
{
#if TIMER0_MODE == NORMAL_MODE
    CLR_BIT(TCCR0, 3);
    CLR_BIT(TCCR0, 6);

    SET_BIT(TIMSK, 0);
    CLR_BIT(TIMSK, 1);

    TCCR0 &= 0b11111000;

#elif TIMER0_MODE == CTC_MODE
    SET_BIT(TCCR0, 3);
    CLR_BIT(TCCR0, 6);

    CLR_BIT(TIMSK, 0);
    SET_BIT(TIMSK, 1);

    TCCR0 &= 0b11001000;
    TCCR0 |= (TIMER0_CLK_CFG | (TIMER0_CTC_OC0_PIN_MODE << 4));

#elif TIMER0_MODE == FAST_PWM_MODE
    SET_BIT(TCCR0, 3);
    SET_BIT(TCCR0, 6);

    CLR_BIT(TIMSK, 0);
    CLR_BIT(TIMSK, 1);

    TCCR0 &= 0b11001000;
    TCCR0 |= (TIMER0_CLK_CFG | ((TIMER0_OC0_PIN_MODE & 0b11) << 4));

#elif TIMER0_MODE == PHASE_CORRECT_MODE
    CLR_BIT(TCCR0, 3);
    SET_BIT(TCCR0, 6);

    CLR_BIT(TIMSK, 0);
    CLR_BIT(TIMSK, 1);

    TCCR0 &= 0b11001000;
    TCCR0 |= (TIMER0_CLK_CFG | ((TIMER0_OC0_PIN_MODE & 0b11) << 4));

#endif
}

void MTIMER0_voidStartTimer(void)
{
    TCCR0 &= 0b11111000;
    TCCR0 |= TIMER0_CLK_CFG;
}

void MTIMER0_voidStopTimer(void)
{
    TCCR0 &= 0b11111000;
}

void MTIMER0_voidSetPreloadValue(u8 A_u8NoOfTicks)
{
    TCNT0 = A_u8NoOfTicks;
}

void MTIMER0_voidSetOCR0Value(u8 A_u8OC0Value)
{
    OCR0 = A_u8OC0Value;
}

void MTIMER0_voidSetOVFCallback(void (*A_PtrToFunc)(void))
{
    if(A_PtrToFunc != NULL)
    {
        TIMER0_OVF_CALLBACK = A_PtrToFunc;
    }
}

void MTIMER0_voidSetCTCCallback(void (*A_PtrToFunc)(void))
{
    if(A_PtrToFunc != NULL)
    {
        TIMER0_CTC_CALLBACK = A_PtrToFunc;
    }
}

ISR(TIMER0_OVF_vect)
{
    if (TIMER0_OVF_CALLBACK != NULL)
    {
        TIMER0_OVF_CALLBACK();
    }
}
ISR(TIMER0_COMP_vect)
{
    if (TIMER0_CTC_CALLBACK != NULL)
    {
        TIMER0_CTC_CALLBACK();
    }
}
