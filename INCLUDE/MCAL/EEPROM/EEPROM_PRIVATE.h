/*
 * EEPROM_PRIVATE.h
 *
 *  Created on: Aug 26, 2026
 *      Author: elmal
 */

#ifndef INCLUDE_MCAL_EEPROM_EEPROM_PRIVATE_H_
#define INCLUDE_MCAL_EEPROM_EEPROM_PRIVATE_H_


#define EEARL   (*(volatile u8 *)0x3E)
#define EEARH   (*(volatile u8 *)0x3F)
#define EEDR    (*(volatile u8 *)0x3D)
#define EECR    (*(volatile u8 *)0x3C)


#define EERIE   3       /* EEPROM Ready Interrupt Enable */
#define EEMWE   2       /* EEPROM Master Write Enable */
#define EEWE    1       /* EEPROM Write Enable */
#define EERE    0       /* EEPROM Read Enable */


#define EEPROM_MAX_ADDRESS   1023U



#define EEPROM_POLLING_MODE      1
#define EEPROM_INTERRUPT_MODE    2





#endif /* INCLUDE_MCAL_EEPROM_EEPROM_PRIVATE_H_ */
