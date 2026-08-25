/*
 * I2C_CFG.h
 *
 * Created: 8/25/2026
 * Author: FaresKan
 */

#ifndef I2C_CFG_H_
#define I2C_CFG_H_


/*=========================================================
 * CPU and I2C clock configuration
 *=========================================================*/

#define I2C_CPU_FREQUENCY_HZ       16000000UL
#define I2C_SCL_FREQUENCY_HZ       100000UL


/*=========================================================
 * TWI Prescaler Options
 *=========================================================*/

#define I2C_PRESCALER_1            1UL
#define I2C_PRESCALER_4            4UL
#define I2C_PRESCALER_16           16UL
#define I2C_PRESCALER_64           64UL

#define I2C_PRESCALER_VALUE        I2C_PRESCALER_1


/*=========================================================
 * Device address configuration
 *
 * The address must be a 7-bit address: 0x00 to 0x7F.
 * This address is mainly used when ATmega32 works as slave.
 *=========================================================*/

#define I2C_OWN_SLAVE_ADDRESS      0x01U


/*=========================================================
 * General Call Recognition
 *=========================================================*/

#define I2C_GENERAL_CALL_DISABLE   0U
#define I2C_GENERAL_CALL_ENABLE    1U

#define I2C_GENERAL_CALL_MODE      I2C_GENERAL_CALL_DISABLE


/*=========================================================
 * TWBR value calculation
 *
 * SCL Frequency =
 * CPU Frequency / (16 + 2 × TWBR × Prescaler)
 *=========================================================*/

#define I2C_TWBR_VALUE                                             \
    ((I2C_CPU_FREQUENCY_HZ / I2C_SCL_FREQUENCY_HZ - 16UL) /       \
     (2UL * I2C_PRESCALER_VALUE))


#endif /* I2C_CFG_H_ */