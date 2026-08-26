/*
 * Timer_INTERFACE.h
 *
 *  Created on: Aug 25, 2026
 *      Author: Teko
 */

#ifndef INCLUDE_MCAL_TIMER_TIMER_INTERFACE_H_
#define INCLUDE_MCAL_TIMER_TIMER_INTERFACE_H_

#define NORMAL_MODE          0
#define PHASE_CORRECT_MODE   1
#define CTC_MODE             2
#define FAST_PWM_MODE        3

#define NORMAL_DIO_PIN       0
#define TOGGLE_CTC           1
#define CLR_CTC              2
#define SET_CTC              3

/* PWM COM00 & COM01 Bit Values */
#define NON_INVERTED_PWM     2
#define INVERTED_PWM         3

void MTIMER0_voidInit(void);
void MTIMER0_voidStartTimer(void);
void MTIMER0_voidStopTimer(void);

void MTIMER0_voidSetPreloadValue(u8 A_u8Preload);
void MTIMER0_voidSetOCR0Value(u8 A_u8OC0Value);

void MTIMER0_voidSetOVFCallback(void (*A_PtrToFunc)(void));
void MTIMER0_voidSetCTCCallback(void (*A_PtrToFunc)(void));

#endif /* INCLUDE_MCAL_TIMER_TIMER_INTERFACE_H_ */
