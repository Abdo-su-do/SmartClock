/*
 * I2C_PRIVTE.h
 *
 * Created: 8/24/2026 8:53:52 PM
 *  Author: FaresKan
 */ 


#ifndef I2C_PRIVTE_H_
#define I2C_PRIVTE_H_

/* TWI/I2C Register Addresses */

#define TWBR_REG    (*((volatile u8 *)0x20))
#define TWSR_REG    (*((volatile u8 *)0x21))
#define TWAR_REG    (*((volatile u8 *)0x22))
#define TWDR_REG    (*((volatile u8 *)0x23))
#define TWCR_REG    (*((volatile u8 *)0x56))


/*=========================================================
 * TWCR Register Bit Numbers
 *=========================================================*/

#define TWCR_TWIE_BIT          0U
#define TWCR_TWEN_BIT          2U
#define TWCR_TWWC_BIT          3U
#define TWCR_TWSTO_BIT         4U
#define TWCR_TWSTA_BIT         5U
#define TWCR_TWEA_BIT          6U
#define TWCR_TWINT_BIT         7U


/*=========================================================
 * TWSR Register Bit Numbers
 *=========================================================*/

#define TWSR_TWPS0_BIT         0U
#define TWSR_TWPS1_BIT         1U


/*=========================================================
 * I2C Status Codes
 *=========================================================*/

#define I2C_STATUS_MASK                        0xF8U

#define I2C_START_CONDITION_TRANSMITTED        0x08U
#define I2C_REPEATED_START_TRANSMITTED         0x10U

#define I2C_SLA_WRITE_ACK_RECEIVED             0x18U
#define I2C_SLA_READ_ACK_RECEIVED              0x40U

#define I2C_DATA_TRANSMITTED_ACK_RECEIVED      0x28U

#define I2C_DATA_RECEIVED_ACK_RETURNED         0x50U
#define I2C_DATA_RECEIVED_NACK_RETURNED        0x58U

#define I2C_ARBITRATION_LOST                   0x38U


/*=========================================================
 * Private Macros
 *=========================================================*/

#define I2C_WRITE_BIT                          0U
#define I2C_READ_BIT                           1U

#define I2C_NULL_POINTER                       ((void *)0)


#endif /* I2C_PRIVTE_H_ */