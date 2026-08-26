/*
 * EEPROM_INTERFACE.h
 *
 *  Created on: Aug 26, 2026
 *      Author: elmal
 */

#ifndef INCLUDE_MCAL_EEPROM_EEPROM_INTERFACE_H_
#define INCLUDE_MCAL_EEPROM_EEPROM_INTERFACE_H_

#include "../../lib/STD_TYPES.h"

typedef enum
{
    EEPROM_OK = 0,
    EEPROM_NOK,
    EEPROM_INVALID_ADDRESS

} EEPROM_ErrorStatus;


void MEEPROM_voidInit(void);

EEPROM_ErrorStatus MEEPROM_EEPROM_ErrorStatusWriteByte(
        u16 A_u16Address,
        u8 A_u8Data
);


EEPROM_ErrorStatus MEEPROM_EEPROM_ErrorStatusReadByte(
        u16 A_u16Address,
        u8 *A_pu8Data
);


EEPROM_ErrorStatus MEEPROM_EEPROM_ErrorStatusWriteBuffer(
        u16 A_u16Address,
        u8 *A_pu8Buffer,
        u16 A_u16Length
);


EEPROM_ErrorStatus MEEPROM_EEPROM_ErrorStatusReadBuffer(
        u16 A_u16Address,
        u8 *A_pu8Buffer,
        u16 A_u16Length
);

#endif /* INCLUDE_MCAL_EEPROM_EEPROM_INTERFACE_H_ */
