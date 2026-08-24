#include "../INCLUDE/LIB/STD_TYPES.h"
#include "../INCLUDE/LIB/BIT_MATH.h"

#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"
#include "../INCLUDE/MCAL/ADC/ADC_INTERFACE.h"

#include "../INCLUDE/HAL/LCD/LCD_INTERFACE.h"

#include <util/delay.h>


static void APP_voidDisplayNumber(u16 A_u16Number)
{
    HLCD_voidSendData((A_u16Number / 1000) + '0');

    A_u16Number %= 1000;

    HLCD_voidSendData((A_u16Number / 100) + '0');

    A_u16Number %= 100;

    HLCD_voidSendData((A_u16Number / 10) + '0');

    A_u16Number %= 10;

    HLCD_voidSendData(A_u16Number + '0');
}


int main(void)
{
    u16 Local_u16ADCValue;
    u8 Local_u8Channel;


    MDIO_voidInit();

    HLCD_voidInit();

    MADC_voidInit();


    while (1)
    {
        for (Local_u8Channel = ADC0;
             Local_u8Channel <= ADC7;
             Local_u8Channel++)
        {
            /* Read ADC value */
            Local_u16ADCValue =
                MADC_u16GetDigitalValue(
                    (ADC_CHANNELS)Local_u8Channel
                );


            /* Display channel */
            HLCD_voidClearDisplay();

            HLCD_voidSendString((u8 *)"ADC");

            HLCD_voidSendData(
                Local_u8Channel + '0'
            );


            /* Display value */
            HLCD_voidGoToPos(ROW2,col1);

            HLCD_voidSendString((u8 *)"Value: ");

            APP_voidDisplayNumber(
                Local_u16ADCValue
            );


            /* Wait 2 seconds */
            _delay_ms(2000);
        }
    }
}
