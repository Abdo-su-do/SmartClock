/*
 * EXTI_PROGRAM.c
 *
 *  Created on: Aug 23, 2026
 *      Author: Abdullah
 */

#include "../INCLUDE/MCAL/EXTI/EXTI_CFG.h"
#include "../INCLUDE/MCAL/EXTI/EXTI_PRIVATE.h"
#include "../INCLUDE/MCAL/EXTI/EXTI_INTERFACE.h"

#include <util/delay.h>
#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"
#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"

#include <avr/interrupt.h>

void (* volatile EXTI0_Callback)(void) = NULL;
void (* volatile EXTI1_Callback)(void) = NULL;
void (* volatile EXTI2_Callback)(void) = NULL;

void MEXTI_voidConfig(EXTI_ID A_EXIT_ID, EXTI_SENSE_MODE  A_EXTI_SENSE_MODE)
{
	// Fixed the typo in A_EXTI_SENSE_MODE
	if((A_EXIT_ID <= EXTI2) && (A_EXTI_SENSE_MODE <= RISING))
	{
		switch (A_EXIT_ID)
		{
		case EXTI0 :
			MCUCR &= ~(EXTI_SENSE_MODE_MASK<< EXTI0_SENSE_MODE_BITS);
			MCUCR |=  (A_EXTI_SENSE_MODE << EXTI0_SENSE_MODE_BITS); 
			break;
		case EXTI1 :
			MCUCR &= ~( EXTI_SENSE_MODE_MASK<< EXTI1_SENSE_MODE_BITS);
			MCUCR |=  (A_EXTI_SENSE_MODE << EXTI1_SENSE_MODE_BITS); 
			break;
		case EXTI2 :
			switch (A_EXTI_SENSE_MODE)
			{
			case RISING:
				SET_BIT(MCUCSR, EXTI2_SENSE_MODE_BITS); 
				break;
			case FALLING:
				CLR_BIT(MCUCSR, EXTI2_SENSE_MODE_BITS); 
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

void MEXTI_voidEnable(EXTI_ID A_EXTI_ID){
	if (A_EXTI_ID <= EXTI2)
	{
		switch (A_EXTI_ID)
		{
		case EXTI0:
			SET_BIT(GICR, EXTI0_ENABLE_BIT); 
			break;
		case EXTI1:
			SET_BIT(GICR, EXTI1_ENABLE_BIT);
			break;
		case EXTI2:
			SET_BIT(GICR, EXTI2_ENABLE_BIT);
			break;
		default:
			break;
		}
	}
}


void MEXTI_voidClearFlag(EXTI_ID A_EXTI_ID){
	if (A_EXTI_ID <= EXTI2)
	{
		switch (A_EXTI_ID)
		{
		case EXTI0:
			SET_BIT(GIFR, EXTI0_FLAG_BIT);
			break;
		case EXTI1:
			SET_BIT(GIFR, EXTI1_FLAG_BIT);
			break;
		case EXTI2:
			SET_BIT(GIFR, EXTI2_FLAG_BIT);
			break;
		default:
			break;
		}
	}
}

void MEXTI_voidDisable(EXTI_ID A_EXTI_ID){
	if (A_EXTI_ID <= EXTI2)
	{
		switch (A_EXTI_ID)
		{
		case EXTI0:
			CLR_BIT(GICR, EXTI0_ENABLE_BIT);
			break;
		case EXTI1:
			CLR_BIT(GICR, EXTI1_ENABLE_BIT);
			break;
		case EXTI2:
			CLR_BIT(GICR, EXTI2_ENABLE_BIT);
			break;
		default:
			break;
		}
	}
}

void MEXTI_voidSetCallBack(EXTI_ID A_EXTI_ID, void (* PTRTOFunction)(void)){
	if ((A_EXTI_ID <= EXTI2) && (PTRTOFunction != NULL))
	{
		switch (A_EXTI_ID)
		{
		case EXTI0:
			EXTI0_Callback = PTRTOFunction;
			break;
		case EXTI1:
			EXTI1_Callback = PTRTOFunction;
			break;
		case EXTI2:
			EXTI2_Callback = PTRTOFunction;
			break;
		default:
			break;
		}
	}
}

ISR(INT0_vect) {
	if (EXTI0_Callback != NULL)
	{
		EXTI0_Callback();
	}
}

ISR(INT1_vect) {
	if (EXTI1_Callback != NULL)
	{
		EXTI1_Callback();
	}
}

ISR(INT2_vect) {
	if (EXTI2_Callback != NULL)
	{
		EXTI2_Callback();
	}
}

