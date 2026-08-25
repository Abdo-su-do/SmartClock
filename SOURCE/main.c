#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"
#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"
#include "../INCLUDE/MCAL/GI/GI_INTERFACE.h"
#include "../INCLUDE/MCAL/ADC/ADC_INTERFACE.h"
#include "../INCLUDE/HAL/ROTARY/ROTARY_INTERFACE.h"
#include "../INCLUDE/HAL/LCD/LCD_INTERFACE.h"
#include "../INCLUDE/HAL/KEYPAD/KEYPAD_INTERFACE.h"
#include "../INCLUDE/HAL/LCD/LCD_CFG.h"
#include "../INCLUDE/HAL/KEYPAD/KEYPAD_CFG.h"
#include <util/delay.h>

volatile s32 App_MenuIndex = 0;

void App_MenuScroll(ROTARY_DIRECTION Direction)
{
    if (Direction == ROTARY_CW)
    {
        App_MenuIndex++; // Scroll Down
    }
    else if (Direction == ROTARY_CCW)
    {
        App_MenuIndex--; // Scroll Up
    }
}

int main(void)
{
    /* Initialize drivers */
    MADC_voidInit();
    HLCD_voidInit();
    HKEPAD_voidInit();
    
    HROTARY_voidInit();
    HROTARY_voidSetCallback(App_MenuScroll);
    
    /* Enable Global Interrupts (Needed for Rotary) */
    MGI_voidEnable();

    u16 adc_value = 0;
    u8 key = KEY_NOT_PRESSED;

    HLCD_voidSendString((u8*)"Driver Test:");
    _delay_ms(1000);

    while(1)
    {
        /* 1. Test ADC from ADC3 (PORTA PIN3) */
        adc_value = MADC_u16GetDigitalValue(ADC3);

        /* 2. Test Keypad */
        key = HKEYPAD_u8GetPressedKey();

        /* 3. Display all values on LCD */
        HLCD_voidClearDisplay();
        
        // Line 1: Show ADC and Keypad
        HLCD_voidGoToPos(L1, C1);
        HLCD_voidSendString((u8*)"ADC:");
        HLCD_voidDisplayNumber(adc_value);
        
        HLCD_voidGoToPos(L1, C10);
        HLCD_voidSendString((u8*)"Key:");
        if(key != KEY_NOT_PRESSED)
        {
            HLCD_voidSendData(key);
        }
        else
        {
            HLCD_voidSendString((u8*)"None");
        }

        // Line 2: Show Rotary
        HLCD_voidGoToPos(L2, C1);
        HLCD_voidSendString((u8*)"Rotary:");
        HLCD_voidDisplayNumber((u32)App_MenuIndex);

        _delay_ms(500); // Increased delay to 500ms so it's easier to see
    }
    return 0;
}
