/*
 * ROTARY_PROGRAM.c
 *
 *  Created on: Aug 24, 2026
 *      Author: Abdullah
 */



/*
 * ROTARY_PROGRAM.c
 *
 *  Created on: Aug 24, 2026
 *      Author: Abdullah
 */

#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"

// Include MCAL Drivers
#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"
#include "../INCLUDE/MCAL/EXTI/EXTI_INTERFACE.h"

// Include HAL Rotary Files
#include "../INCLUDE/HAL/ROTARY/ROTARY_CFG.h"
#include "../INCLUDE/HAL/ROTARY/ROTARY_PRIVATE.h"
#include "../INCLUDE/HAL/ROTARY/ROTARY_INTERFACE.h"

// Pointer to store the Application's function
static void (*HROTARY_Callback)(ROTARY_DIRECTION) = NULL;

void HROTARY_voidInit(void)
{
    // 1. Configure Pin A (EXTI Pin) as Input + Pull-up
    MDIO_voidSetPinDirection(ROTARY_PORT_A, ROTARY_PIN_A, DIO_INPUT);
    MDIO_voidSetPinValue(ROTARY_PORT_A, ROTARY_PIN_A, DIO_HIGH);

    // 2. Configure Pin B (Direction Pin) as Input + Pull-up
    MDIO_voidSetPinDirection(ROTARY_PORT_B, ROTARY_PIN_B, DIO_INPUT);
    MDIO_voidSetPinValue(ROTARY_PORT_B, ROTARY_PIN_B, DIO_HIGH);

    // 3. Configure the EXTI to trigger on the Falling Edge of Pin A
    MEXTI_voidConfig(ROTARY_EXTI_LINE, FALLING);

    // 4. Link the MCAL interrupt to our Private HAL Handler
    MEXTI_voidSetCallBack(ROTARY_EXTI_LINE, HROTARY_voidInterruptHandler);

    // 5. Enable the EXTI channel
    MEXTI_voidEnable(ROTARY_EXTI_LINE);
}

void HROTARY_voidSetCallback(void (*ptrToFunc)(ROTARY_DIRECTION))
{
    if (ptrToFunc != NULL)
    {
        HROTARY_Callback = ptrToFunc;
    }
}

// This function fires automatically every time the encoder clicks to a new step
static void HROTARY_voidInterruptHandler(void)
{
    if (HROTARY_Callback != NULL)
    {
        // Check the state of Channel B to determine the direction
        PIN_STATUS PinB_State = MDIO_PinstatusGetPinValue(ROTARY_PORT_B, ROTARY_PIN_B);

        if (PinB_State == DIO_HIGH)
        {
            // Pin A fell, Pin B is still High -> Clockwise
            HROTARY_Callback(ROTARY_CW);
        }
        else
        {
            // Pin A fell, Pin B is already Low -> Counter-Clockwise
            HROTARY_Callback(ROTARY_CCW);
        }
    }
}

 /* SOURCE_ROTARY_PROGRAM_C_ */
