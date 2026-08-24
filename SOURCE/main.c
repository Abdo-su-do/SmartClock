
#include "../INCLUDE/INCLUDES.h"


volatile u8 Global_u8LedState = DIO_LOW;

void App_ToggleLED(void)
{
    if (Global_u8LedState == DIO_LOW)
    {
        MDIO_voidSetPinValue(PORTA, PIN0, DIO_HIGH);
        Global_u8LedState = DIO_HIGH;
    }
    else
    {
        MDIO_voidSetPinValue(PORTA, PIN0, DIO_LOW);
        Global_u8LedState = DIO_LOW;
    }
}

int main(void)
{
    MDIO_voidSetPinDirection(PORTA, PIN0, DIO_OUTPUT);
    MDIO_voidSetPinDirection(PORTD, PIN2, DIO_INPUT);
    MDIO_voidSetPinValue(PORTD, PIN2, DIO_HIGH);

    MEXTI_voidConfig(EXTI0, FALLING);
    MEXTI_voidSetCallBack(EXTI0, App_ToggleLED);
    MEXTI_voidClearFlag(EXTI0);
    MEXTI_voidEnable(EXTI0);

    MGI_voidEnable();

    while(1) { }
    return 0;
}
