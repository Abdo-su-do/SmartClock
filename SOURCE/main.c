#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"
#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"
#include "../INCLUDE/MCAL/GI/GI_INTERFACE.h"
#include "../INCLUDE/HAL/ROTARY/ROTARY_INTERFACE.h"

s32 App_MenuIndex = 0;

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

    // Output the raw number to PORTA!
    MDIO_voidSetPortValue(PORTA, (u8)App_MenuIndex);
}

int main(void)
{
    // Set all of PORTA as output
    MDIO_voidSetPortDirection(PORTA, 0xFF);

    HROTARY_voidInit();
    HROTARY_voidSetCallback(App_MenuScroll);
    MGI_voidEnable();

    while(1)
    {
        // Waiting for the wheel to turn...
    }
    return 0;
}
