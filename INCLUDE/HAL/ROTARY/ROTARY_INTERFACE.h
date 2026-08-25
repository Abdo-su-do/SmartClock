/*
 * ROTARY_INTERFACE.h
 *
 *  Created on: Aug 24, 2026
 *      Author: Abdullah
 */

#ifndef INCLUDE_HAL_ROTARY_ROTARY_INTERFACE_H_
#define INCLUDE_HAL_ROTARY_ROTARY_INTERFACE_H_

typedef enum {
    ROTARY_CW = 0, // Clockwise
    ROTARY_CCW     // Counter-Clockwise
} ROTARY_DIRECTION;

void HROTARY_voidInit(void);

// The callback now passes the direction to your main application!
void HROTARY_voidSetCallback(void (*ptrToFunc)(ROTARY_DIRECTION));

#endif /* INCLUDE_HAL_ROTARY_ROTARY_INTERFACE_H_ */
