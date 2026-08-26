/*
 * Timer_CFG.h
 *
 *  Created on: Aug 25, 2026
 *      Author: Teko
 */

#ifndef INCLUDE_MCAL_TIMER_TIMER_CFG_H_
#define INCLUDE_MCAL_TIMER_TIMER_CFG_H_

/*
 NORMAL_MODE
 PHASE_CORRECT_MODE
 CTC_MODE
 FAST_PWM_MODE
*/
#define TIMER0_MODE             NORMAL_MODE

/*
 0b000 No clock source (Timer/Counter stopped)
 0b001 clkI/O (No prescaling)
 0b010 clkI/O/8 (From prescaler)
 0b011 clkI/O/64 (From prescaler)
 0b100 clkI/O/256 (From prescaler)
 0b101 clkI/O/1024 (From prescaler)
 0b110 External clock source on T0 pin. Clock on falling edge.
 0b111 External clock source on T0 pin. Clock on rising edge.
*/
#define TIMER0_CLK_CFG          0b010

/*
 NORMAL_DIO_PIN  0
 TOGGLE_CTC      1
 CLR_CTC         2
 SET_CTC         3
*/
#define TIMER0_CTC_OC0_PIN_MODE NORMAL_DIO_PIN

/*
 NON_INVERTED_PWM
 INVERTED_PWM
*/
#define TIMER0_OC0_PIN_MODE     NON_INVERTED_PWM

#endif /* INCLUDE_MCAL_TIMER_TIMER_CFG_H_ */
