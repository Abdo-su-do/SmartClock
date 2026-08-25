/*
 * ROTARY_CFG.h
 *
 *  Created on: Aug 24, 2026
 *      Author: Abdullah
 */

#ifndef INCLUDE_HAL_ROTARY_ROTARY_CFG_H_
#define INCLUDE_HAL_ROTARY_ROTARY_CFG_H_

#define ROTARY_PORT_A       PORTD
#define ROTARY_PIN_A        PIN2
#define ROTARY_EXTI_LINE    EXTI0

// Channel B can be ANY pin (Let's use PD3)
#define ROTARY_PORT_B       PORTD
#define ROTARY_PIN_B        PIN3

#endif /* INCLUDE_HAL_ROTARY_ROTARY_CFG_H_ */
