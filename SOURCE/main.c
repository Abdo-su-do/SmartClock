/*
 * main.c

 *
 *  Created on: Aug 18, 2026
 *      Author: Abdullah
 */

#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"
#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"
#include "../INCLUDE/HAL/LCD/LCD_INTERFACE.h"
#include "../INCLUDE/HAL/KEYPAD/KEYPAD_INTERFACE.h"
#include "../INCLUDE/HAL/KEYPAD/KEYPAD_CFG.h" 

int main(void)
{
    // 1. Initialize the Ports (Reads your DIO_CFG.h)
    MDIO_voidInit();
    
    // 2. Boot up the LCD screen
    HLCD_voidInit();
    
    // 3. Print a startup message to verify LCD works
    HLCD_voidSendString((u8*)"NTI MVP Ready:");
    
    // Move cursor to the second row for the keypad typing
    HLCD_voidGoToPos(ROW2, col1); 
    
    u8 Local_u8Key;

    while(1)
    {
        // 4. Scan the Keypad continuously
        Local_u8Key = HKEYPAD_u8GetPressedKey();
        
        // 5. If a button was pushed, print it to the screen!
        if(Local_u8Key != KEY_NOT_PRESSED)
        {
            HLCD_voidSendData(Local_u8Key);
        }
    }
    return 0;
}
jhoijio