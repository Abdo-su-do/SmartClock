/*
 * GIE_PROGRAM.c
 *
 *  Created on: Aug 24, 2026
 *      Author: Abdullah
 */

#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"

#include "../INCLUDE/MCAL/GIE/GIE_CFG.h"
#include "../INCLUDE/MCAL/GIE/GIE_INTERFACE.h"
#include "../INCLUDE/MCAL/GIE/GIE_PRIVATE.h"

void MGI_voidEnable(void) {
	SET_BIT(SREG, GI_ENABLE_BIT);
}

void MGI_voidDisable(void) {
	CLR_BIT(SREG, GI_ENABLE_BIT);
}
