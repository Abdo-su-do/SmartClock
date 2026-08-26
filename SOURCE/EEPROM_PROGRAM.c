/*
 * EEPROM_PROGRAM.c
 *
 *  Created on: Aug 26, 2026
 *      Author: MALAH
 */
#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"

#include "../INCLUDE/MCAL/EEPROM/EEPROM_CFG.h"
#include "../INCLUDE/MCAL/EEPROM/EEPROM_PRIVATE.h"
#include "../INCLUDE/MCAL/EEPROM/EEPROM_INTERFACE.h"

void MEEPROM_voidInit(void)
{
#if (EEPROM_WRITE_MODE == EEPROM_POLLING_MODE)

    /* Disable EEPROM Ready Interrupt */
    CLR_BIT(EECR, EERIE);

#elif (EEPROM_WRITE_MODE == EEPROM_INTERRUPT_MODE)

    /* Enable EEPROM Ready Interrupt */
    SET_BIT(EECR, EERIE);

#endif
}

EEPROM_ErrorStatus MEEPROM_EEPROM_ErrorStatusWriteByte(u16 A_u16Address, u8 A_u8Data)
{
    EEPROM_ErrorStatus Local_ErrorStatus = EEPROM_OK;

#if (EEPROM_ADDRESS_CHECK == STD_ON)

    if (A_u16Address > EEPROM_MAX_ADDRESS)
    {
        Local_ErrorStatus = EEPROM_INVALID_ADDRESS;
        return Local_ErrorStatus;
    }

#endif

    // Wait for previous write operation

    while(GET_BIT(EECR, EEWE) == 1)
    {
    }

    // Set EEPROM Address


    EEARL = (u8)(A_u16Address & 0xFF);

    EEARH = (u8)(A_u16Address >> 8);

     // Set EEPROM Data


    EEDR = A_u8Data;


     // EEPROM Write Sequence

    SET_BIT(EECR, EEMWE);

    SET_BIT(EECR, EEWE);

     // Wait until EEPROM write completes


    while(GET_BIT(EECR, EEWE) == 1)
    {
    }

    return Local_ErrorStatus;
}

EEPROM_ErrorStatus MEEPROM_EEPROM_ErrorStatusReadByte(u16 A_u16Address, u8 *A_pu8Data)
{
	EEPROM_ErrorStatus Local_ErrorStatus = EEPROM_OK;

	if (A_pu8Data == NULL)
	{
		Local_ErrorStatus = EEPROM_NOK;
		return Local_ErrorStatus;
	}

#if (EEPROM_ADDRESS_CHECK == STD_ON)
	if (A_u16Address > EEPROM_MAX_ADDRESS)
	{
		Local_ErrorStatus = EEPROM_INVALID_ADDRESS;
		return Local_ErrorStatus;
	}
#endif

	/* 1- Wait for completion of any previous write operation */
	while (GET_BIT(EECR, EEWE));

	/* 2- Set up the address in the EEPROM Address Register */
	EEARL = (u8)A_u16Address;
	EEARH = (u8)(A_u16Address >> 8);

	/* 3- Set EERE to start the read operation */
	SET_BIT(EECR, EERE);

	/* 4- Data is available right after setting EERE */
	*A_pu8Data = EEDR;

	return Local_ErrorStatus;
}

EEPROM_ErrorStatus MEEPROM_EEPROM_ErrorStatusWriteBuffer(u16 A_u16Address, u8 *A_pu8Buffer, u16 A_u16Length)
{
	EEPROM_ErrorStatus Local_ErrorStatus = EEPROM_OK;
	u16 Local_u16Counter;

	if (A_pu8Buffer == NULL)
	{
		Local_ErrorStatus = EEPROM_NOK;
		return Local_ErrorStatus;
	}

	for (Local_u16Counter = 0; Local_u16Counter < A_u16Length; Local_u16Counter++)
	{
		Local_ErrorStatus = MEEPROM_EEPROM_ErrorStatusWriteByte((A_u16Address + Local_u16Counter), A_pu8Buffer[Local_u16Counter]);

		if (Local_ErrorStatus != EEPROM_OK)
		{
			break;
		}
	}

	return Local_ErrorStatus;
}

EEPROM_ErrorStatus MEEPROM_EEPROM_ErrorStatusReadBuffer(u16 A_u16Address, u8 *A_pu8Buffer, u16 A_u16Length)
{
	EEPROM_ErrorStatus Local_ErrorStatus = EEPROM_OK;
	u16 Local_u16Counter;

	if (A_pu8Buffer == NULL)
	{
		Local_ErrorStatus = EEPROM_NOK;
		return Local_ErrorStatus;
	}

	for (Local_u16Counter = 0; Local_u16Counter < A_u16Length; Local_u16Counter++)
	{
		Local_ErrorStatus = MEEPROM_EEPROM_ErrorStatusReadByte((A_u16Address + Local_u16Counter), &A_pu8Buffer[Local_u16Counter]);

		if (Local_ErrorStatus != EEPROM_OK)
		{
			break;
		}
	}

	return Local_ErrorStatus;
}
