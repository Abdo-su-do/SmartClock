/*
 * I2C_INTERFACE.h
 *
 *  Created on: ??�/??�/????
 *      Author: ALRYADA
 */

#ifndef INCLUDE_MCAL_I2C_I2C_INTERFACE_H_
#define INCLUDE_MCAL_I2C_I2C_INTERFACE_H_

#include "../../lib/STD_TYPES.h"



/*=========================================================
 * I2C Read/Write Selection
 *=========================================================*/

typedef enum
{
    I2C_WRITE = 0,
    I2C_READ  = 1

} I2C_READ_WRITE;


/*=========================================================
 * I2C Error Status
 *=========================================================*/

typedef enum
{
    I2C_OK = 0,

    I2C_START_ERROR,
    I2C_REPEATED_START_ERROR,
    I2C_SLAVE_ADDRESS_WRITE_ERROR,
    I2C_SLAVE_ADDRESS_READ_ERROR,
    I2C_DATA_WRITE_ERROR,
    I2C_DATA_READ_ERROR,
    I2C_ARBITRATION_LOST_ERROR

} I2C_ERROR_STATUS;


/*=========================================================
 * Initialization
 *=========================================================*/

/**
 * @brief Initialize the ATmega32 TWI/I2C peripheral.
 */
void MI2C_voidInit(void);


/*=========================================================
 * Master Transmitter Functions
 *=========================================================*/

/**
 * @brief Send a START condition.
 *
 * @return I2C_OK if START was transmitted successfully.
 */
I2C_ERROR_STATUS MI2C_enuSendStartCondition(void);


/**
 * @brief Send a repeated START condition.
 *
 * @return I2C_OK if repeated START was transmitted.
 */
I2C_ERROR_STATUS MI2C_enuSendRepeatedStartCondition(void);


/**
 * @brief Send the slave address followed by the write bit.
 *
 * @param A_u8SlaveAddress Seven-bit slave address.
 *
 * @return I2C_OK if the slave acknowledges the address.
 */
I2C_ERROR_STATUS MI2C_enuSendSlaveAddressWithWrite(
    u8 A_u8SlaveAddress
);


/**
 * @brief Send the slave address followed by the read bit.
 *
 * @param A_u8SlaveAddress Seven-bit slave address.
 *
 * @return I2C_OK if the slave acknowledges the address.
 */
I2C_ERROR_STATUS MI2C_enuSendSlaveAddressWithRead(
    u8 A_u8SlaveAddress
);


/**
 * @brief Send one data byte.
 *
 * @param A_u8Data Data byte to transmit.
 *
 * @return I2C_OK if the receiver acknowledges the byte.
 */
I2C_ERROR_STATUS MI2C_enuMasterWriteDataByte( u8 A_u8Data);


/*=========================================================
 * Master Receiver Functions
 *=========================================================*/

/**
 * @brief Read one byte and return ACK.
 *
 * ACK tells the slave that the master wants another byte.
 *
 * @param A_pu8ReceivedData Address used to return received data.
 *
 * @return I2C_OK if data was received successfully.
 */
I2C_ERROR_STATUS MI2C_enuMasterReadDataByteWithACK( u8 *A_pu8ReceivedData);


/**
 * @brief Read one byte and return NACK.
 *
 * NACK tells the slave that this is the last required byte.
 *
 * @param A_pu8ReceivedData Address used to return received data.
 *
 * @return I2C_OK if data was received successfully.
 */
I2C_ERROR_STATUS MI2C_enuMasterReadDataByteWithNACK( u8 *A_pu8ReceivedData);


/*=========================================================
 * Stop Function
 *=========================================================*/

/**
 * @brief Send a STOP condition and release the I2C bus.
 */
void MI2C_voidSendStopCondition(void);

#endif /* INCLUDE_MCAL_I2C_I2C_INTERFACE_H_ */
