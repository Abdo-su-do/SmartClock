/*
 * I2C_PROGRAM.c
 *
 *  Created on: ??�/??�/????
 *      Author: ALRYADA
 */

#include "../INCLUDE/lib/STD_TYPES.h"
#include "../INCLUDE/lib/BIT_MATH.h"

#include "../INCLUDE/MCAL/I2C/I2C_INTERFACE.h"
#include "../INCLUDE/MCAL/I2C/I2C_PRIVATE.h"
#include "../INCLUDE/MCAL/I2C/I2C_CFG.h"

/*=========================================================
 * Private Functions
 *=========================================================*/

/**
 * @brief Wait until the current TWI operation finishes.
 */
static void MI2C_voidWaitForInterruptFlag(void)
{
    while ((TWCR_REG & (1U << TWCR_TWINT_BIT)) == 0U)
    {
        /* Wait until TWINT becomes 1 */
    }
}


/**
 * @brief Return the current I2C status code.
 */
static u8 MI2C_u8GetStatus(void)
{
    return (TWSR_REG & I2C_STATUS_MASK);
}


/*=========================================================
 * Public Functions
 *=========================================================*/

void MI2C_voidInit(void)
{
    /*
     * Configure the TWI prescaler through TWPS1 and TWPS0.
     */

#if I2C_PRESCALER_VALUE == I2C_PRESCALER_1

    TWSR_REG &= (u8)~((1U << TWSR_TWPS1_BIT) |
                     (1U << TWSR_TWPS0_BIT));

#elif I2C_PRESCALER_VALUE == I2C_PRESCALER_4

    TWSR_REG &= (u8)~(1U << TWSR_TWPS1_BIT);
    TWSR_REG |=  (1U << TWSR_TWPS0_BIT);

#elif I2C_PRESCALER_VALUE == I2C_PRESCALER_16

    TWSR_REG |=  (1U << TWSR_TWPS1_BIT);
    TWSR_REG &= (u8)~(1U << TWSR_TWPS0_BIT);

#elif I2C_PRESCALER_VALUE == I2C_PRESCALER_64

    TWSR_REG |= (1U << TWSR_TWPS1_BIT) |
                (1U << TWSR_TWPS0_BIT);

#else
    #error "Invalid I2C prescaler configuration"
#endif


    /*
     * Set the bit-rate register.
     *
     * For:
     * F_CPU = 16 MHz
     * SCL   = 100 kHz
     * Prescaler = 1
     *
     * TWBR = 72
     */

    TWBR_REG = (u8)I2C_TWBR_VALUE;


    /*
     * Set the device's own slave address.
     *
     * TWAR bits:
     *
     * Bit 7:1 = Slave address
     * Bit 0   = General Call Recognition Enable
     */

    TWAR_REG = (u8)(I2C_OWN_SLAVE_ADDRESS << 1U);

#if I2C_GENERAL_CALL_MODE == I2C_GENERAL_CALL_ENABLE

    TWAR_REG |= 1U;

#else

    TWAR_REG &= (u8)~1U;

#endif


    /*
     * Enable the TWI peripheral.
     *
     * TWEN = 1
     */

    TWCR_REG = (1U << TWCR_TWEN_BIT);
}


I2C_ERROR_STATUS MI2C_enuSendStartCondition(void)
{
    u8 Local_u8Status;


    /*
     * TWINT = 1: Clear the interrupt flag
     * TWSTA = 1: Request START condition
     * TWEN  = 1: Enable TWI
     */

    TWCR_REG = (1U << TWCR_TWINT_BIT) |
               (1U << TWCR_TWSTA_BIT) |
               (1U << TWCR_TWEN_BIT);


    MI2C_voidWaitForInterruptFlag();

    Local_u8Status = MI2C_u8GetStatus();


    if (Local_u8Status == I2C_START_CONDITION_TRANSMITTED)
    {
        return I2C_OK;
    }
    else if (Local_u8Status == I2C_ARBITRATION_LOST)
    {
        return I2C_ARBITRATION_LOST_ERROR;
    }
    else
    {
        return I2C_START_ERROR;
    }
}


I2C_ERROR_STATUS MI2C_enuSendRepeatedStartCondition(void)
{
    u8 Local_u8Status;


    TWCR_REG = (1U << TWCR_TWINT_BIT) |
               (1U << TWCR_TWSTA_BIT) |
               (1U << TWCR_TWEN_BIT);


    MI2C_voidWaitForInterruptFlag();

    Local_u8Status = MI2C_u8GetStatus();


    if (Local_u8Status == I2C_REPEATED_START_TRANSMITTED)
    {
        return I2C_OK;
    }
    else if (Local_u8Status == I2C_ARBITRATION_LOST)
    {
        return I2C_ARBITRATION_LOST_ERROR;
    }
    else
    {
        return I2C_REPEATED_START_ERROR;
    }
}


I2C_ERROR_STATUS MI2C_enuSendSlaveAddressWithWrite(
    u8 A_u8SlaveAddress)
{
    u8 Local_u8Status;


    /*
     * TWDR format:
     *
     * Bits 7:1 = Seven-bit slave address
     * Bit 0    = 0 for write
     */

    TWDR_REG = (u8)((A_u8SlaveAddress << 1U) |
                    I2C_WRITE_BIT);


    /*
     * Start transmitting the address.
     */

    TWCR_REG = (1U << TWCR_TWINT_BIT) |
               (1U << TWCR_TWEN_BIT);


    MI2C_voidWaitForInterruptFlag();

    Local_u8Status = MI2C_u8GetStatus();


    if (Local_u8Status == I2C_SLA_WRITE_ACK_RECEIVED)
    {
        return I2C_OK;
    }
    else if (Local_u8Status == I2C_ARBITRATION_LOST)
    {
        return I2C_ARBITRATION_LOST_ERROR;
    }
    else
    {
        return I2C_SLAVE_ADDRESS_WRITE_ERROR;
    }
}


I2C_ERROR_STATUS MI2C_enuSendSlaveAddressWithRead(
    u8 A_u8SlaveAddress)
{
    u8 Local_u8Status;


    /*
     * Bits 7:1 = Seven-bit slave address
     * Bit 0    = 1 for read
     */

    TWDR_REG = (u8)((A_u8SlaveAddress << 1U) |
                    I2C_READ_BIT);


    TWCR_REG = (1U << TWCR_TWINT_BIT) |
               (1U << TWCR_TWEN_BIT);


    MI2C_voidWaitForInterruptFlag();

    Local_u8Status = MI2C_u8GetStatus();


    if (Local_u8Status == I2C_SLA_READ_ACK_RECEIVED)
    {
        return I2C_OK;
    }
    else if (Local_u8Status == I2C_ARBITRATION_LOST)
    {
        return I2C_ARBITRATION_LOST_ERROR;
    }
    else
    {
        return I2C_SLAVE_ADDRESS_READ_ERROR;
    }
}


I2C_ERROR_STATUS MI2C_enuMasterWriteDataByte(
    u8 A_u8Data)
{
    u8 Local_u8Status;


    /*
     * Put the data byte inside TWDR.
     */

    TWDR_REG = A_u8Data;


    /*
     * Start transmitting the data.
     */

    TWCR_REG = (1U << TWCR_TWINT_BIT) |
               (1U << TWCR_TWEN_BIT);


    MI2C_voidWaitForInterruptFlag();

    Local_u8Status = MI2C_u8GetStatus();


    if (Local_u8Status == I2C_DATA_TRANSMITTED_ACK_RECEIVED)
    {
        return I2C_OK;
    }
    else if (Local_u8Status == I2C_ARBITRATION_LOST)
    {
        return I2C_ARBITRATION_LOST_ERROR;
    }
    else
    {
        return I2C_DATA_WRITE_ERROR;
    }
}


I2C_ERROR_STATUS MI2C_enuMasterReadDataByteWithACK(
    u8 *A_pu8ReceivedData)
{
    u8 Local_u8Status;


    if (A_pu8ReceivedData == I2C_NULL_POINTER)
    {
        return I2C_DATA_READ_ERROR;
    }


    /*
     * TWEA = 1:
     * Return ACK after receiving the byte.
     *
     * ACK means that the master wants another byte.
     */

    TWCR_REG = (1U << TWCR_TWINT_BIT) |
               (1U << TWCR_TWEN_BIT)  |
               (1U << TWCR_TWEA_BIT);


    MI2C_voidWaitForInterruptFlag();

    Local_u8Status = MI2C_u8GetStatus();


    if (Local_u8Status == I2C_DATA_RECEIVED_ACK_RETURNED)
    {
        *A_pu8ReceivedData = TWDR_REG;

        return I2C_OK;
    }
    else
    {
        return I2C_DATA_READ_ERROR;
    }
}


I2C_ERROR_STATUS MI2C_enuMasterReadDataByteWithNACK(
    u8 *A_pu8ReceivedData)
{
    u8 Local_u8Status;


    if (A_pu8ReceivedData == I2C_NULL_POINTER)
    {
        return I2C_DATA_READ_ERROR;
    }


    /*
     * TWEA = 0:
     * Return NACK after receiving the byte.
     *
     * NACK means that this is the last byte required
     * by the master.
     */

    TWCR_REG = (1U << TWCR_TWINT_BIT) |
               (1U << TWCR_TWEN_BIT);


    MI2C_voidWaitForInterruptFlag();

    Local_u8Status = MI2C_u8GetStatus();


    if (Local_u8Status == I2C_DATA_RECEIVED_NACK_RETURNED)
    {
        *A_pu8ReceivedData = TWDR_REG;

        return I2C_OK;
    }
    else
    {
        return I2C_DATA_READ_ERROR;
    }
}


void MI2C_voidSendStopCondition(void)
{
    /*
     * TWSTO = 1: Generate STOP condition.
     *
     * The hardware clears TWSTO automatically after
     * transmitting the STOP condition.
     */

    TWCR_REG = (1U << TWCR_TWINT_BIT) |
               (1U << TWCR_TWSTO_BIT) |
               (1U << TWCR_TWEN_BIT);
}
