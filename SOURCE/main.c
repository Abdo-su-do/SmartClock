#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"

#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"
#include "../INCLUDE/MCAL/ADC/ADC_INTERFACE.h"
#include "../INCLUDE/MCAL/EEPROM/EEPROM_INTERFACE.h"
#include "../INCLUDE/MCAL/EEPROM/EEPROM_PRIVATE.h"

#include "../INCLUDE/HAL/LCD/LCD_INTERFACE.h"



#include "../INCLUDE/HAL/LCD/LCD_INTERFACE.h"

#include <util/delay.h>


int main(void)
{
    /* 1. Initialize DIO and LCD */
    MDIO_voidInit();
    HLCD_voidInit();

    /* 2. Clear Screen */
    HLCD_voidClearDisplay();
    HLCD_voidGoToPos(ROW1, col1);
    HLCD_voidSendString((u8*)"Testing EEPROM...");
    _delay_ms(1000);

    /* 3. Write data to EEPROM address 0x0010 */
    u16 test_address = 0x0110;
    u8 write_data = 'A';
    u8 read_data = 0;

    MEEPROM_EEPROM_ErrorStatusWriteByte(test_address, write_data);

    /* 4. Read back from the SAME address */
    MEEPROM_EEPROM_ErrorStatusReadByte(test_address, &read_data);

    /* 5. Display Result */
    HLCD_voidClearDisplay();
    HLCD_voidGoToPos(ROW1, col1);
    if(read_data == write_data)
    {
        HLCD_voidSendString((u8*)"EEPROM OK!");
        HLCD_voidGoToPos(ROW2, col1);
        HLCD_voidSendString((u8*)"Data: ");
        HLCD_voidSendData(read_data);
    }
    else
    {
        HLCD_voidSendString((u8*)"EEPROM ERROR");
        HLCD_voidGoToPos(ROW2, col1);
        HLCD_voidSendString((u8*)"Read: ");
        HLCD_voidSendData(read_data);
    }

    /* 6. Infinite Loop so the MCU doesn't reset */
    while(1)
    {
        
    }
    return 0;
}
