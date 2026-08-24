/*
 * main.c
 *
 * Created on: Aug 23, 2026
 * Author: MALAH
 */

#include <util/delay.h>

#include "../INCLUDE/lib/STD_TYPES.h"

#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"

#include "../INCLUDE/HAL/LCD/LCD_INTERFACE.h"
#include "../INCLUDE/HAL/KEYPAD/KEYPAD_INTERFACE.h"


int main(void)
{
    u8 Local_u8PressedKey;


    /*==================================================
     *                  INITIALIZATION
     *==================================================*/

    MDIO_voidInit();

    HLCD_voidInit();

    HKEPAD_voidInit();

    HLCD_voidClearDisplay();


    /*==================================================
     *                  MAIN LOOP
     *==================================================*/

    while (1)
    {
        Local_u8PressedKey = HKEYPAD_u8GetPressedKey();


        /*
         * KEY_NOT_PRESSED = 0
         */

        if (Local_u8PressedKey != 0)
        {
            HLCD_voidSendData(Local_u8PressedKey);
        }
    }
}
