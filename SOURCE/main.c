#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"

#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"
#include "../INCLUDE/MCAL/ADC/ADC_INTERFACE.h"
#include "../INCLUDE/MCAL/EEPROM/EEPROM_INTERFACE.h"
#include "../INCLUDE/MCAL/EEPROM/EEPROM_PRIVATE.h"


#include "../INCLUDE/HAL/LCD/LCD_INTERFACE.h"

#include <util/delay.h>


int main(void)
{
	MDIO_voidInit();
	HLCD_voidInit();
	u8 data = 0;

	MEEPROM_EEPROM_ErrorStatusWriteByte(0x0001, 'A');

	MEEPROM_EEPROM_ErrorStatusReadByte(
	    0x0000,
	    &data
	);

	if(data == 'A')
	{
	    HLCD_voidSendString((u8*)"EEPROM OK");
	}
	else
	{
	    HLCD_voidSendString((u8*)"EEPROM ERROR");
	}
}
