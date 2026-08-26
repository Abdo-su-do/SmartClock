#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"
#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"
#include "../INCLUDE/MCAL/ADC/ADC_INTERFACE.h"
#include "../INCLUDE/HAL/LCD/LCD_INTERFACE.h"
#include "../INCLUDE/HAL/KEYPAD/KEYPAD_INTERFACE.h"
#include "../INCLUDE/app.h"
#include <util/delay.h>

int main(void)
{
    /* Initialize all pins from DIO_CFG.h */
    MDIO_voidInit();

    /* Initialize ADC (AVCC reference, prescaler 128) */
    MADC_voidInit();

    /* Initialize LCD — sets all control/data pin directions internally */
    HLCD_voidInit();

    /* Initialize Keypad — sets row/col pin directions internally */
    HKEPAD_voidInit();

    /* No interrupts needed — rotary is polled inside APP_voidUpdate */

    /* Splash screen */
    HLCD_voidGoToPos(L1, C1);
    HLCD_voidSendString((u8*)"   Smart Clock v1   ");
    HLCD_voidGoToPos(L2, C1);
    HLCD_voidSendString((u8*)"  ATmega32 Project  ");
    _delay_ms(1500);
    HLCD_voidClearDisplay();

    /* Initialize application state machine */
    APP_voidInit();

    while(1)
    {
        APP_voidUpdate();
    }
    return 0;
}
