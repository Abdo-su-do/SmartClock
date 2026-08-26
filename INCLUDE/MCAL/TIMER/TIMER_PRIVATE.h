/*
 * Timer_PRIVATE.h
 *
 *  Created on: Aug 25, 2026
 *      Author: Teko
 */

#ifndef INCLUDE_MCAL_TIMER_TIMER_PRIVATE_H_
#define INCLUDE_MCAL_TIMER_TIMER_PRIVATE_H_

/* Timer0 Hardware Registers */
#define TCCR0   (*(volatile u8 *) 0x53)
#define TCNT0   (*(volatile u8 *) 0x52)
#define OCR0    (*(volatile u8 *) 0x5C)
#define TIMSK   (*(volatile u8 *) 0x59)
#define TIFR    (*(volatile u8 *) 0x58)

#endif /* INCLUDE_MCAL_TIMER_TIMER_PRIVATE_H_ */
