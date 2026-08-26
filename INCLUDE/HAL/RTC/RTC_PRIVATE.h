/*
 * RTC_PRIVATE.h
 *
 *  Created on: ??þ/??þ/????
 *      Author: ALRYADA
 */

#ifndef INCLUDE_HAL_RTC_RTC_PRIVATE_H_
#define INCLUDE_HAL_RTC_RTC_PRIVATE_H_


/*=========================================================
 * DS3231 / DS3232 I2C Address
 *=========================================================*/

/*
 * Fixed 7-bit address = 0x68
 *
 * Address byte with write bit = 0xD0
 * Address byte with read bit  = 0xD1
 */
#define RTC_SLAVE_ADDRESS             0x68
#define RTC_WRITE_ADDRESS             0xD0
#define RTC_READ_ADDRESS              0xD1


/*=========================================================
 * Time and Date Registers
 *=========================================================*/

#define RTC_SECONDS_REG               0x00
#define RTC_MINUTES_REG               0x01
#define RTC_HOURS_REG                 0x02
#define RTC_DAY_REG                   0x03
#define RTC_DATE_REG                  0x04
#define RTC_MONTH_REG                 0x05
#define RTC_YEAR_REG                  0x06


/*=========================================================
 * Alarm 1 Registers
 *=========================================================*/

#define RTC_ALARM1_SECONDS_REG        0x07
#define RTC_ALARM1_MINUTES_REG        0x08
#define RTC_ALARM1_HOURS_REG          0x09
#define RTC_ALARM1_DAY_DATE_REG       0x0A


/*=========================================================
 * Alarm 2 Registers
 *=========================================================*/

#define RTC_ALARM2_MINUTES_REG        0x0B
#define RTC_ALARM2_HOURS_REG          0x0C
#define RTC_ALARM2_DAY_DATE_REG       0x0D


/*=========================================================
 * Control, Status and Temperature Registers
 *=========================================================*/

#define RTC_CONTROL_REG               0x0E
#define RTC_STATUS_REG                0x0F
#define RTC_AGING_OFFSET_REG          0x10
#define RTC_TEMPERATURE_MSB_REG       0x11
#define RTC_TEMPERATURE_LSB_REG       0x12


/*=========================================================
 * DS3232 SRAM Addresses
 *=========================================================*/

/*
 * Only the DS3232 contains battery-backed SRAM.
 *
 * SRAM range:
 * 0x14 to 0xFF
 */
#define RTC_SRAM_START_ADDRESS        0x14
#define RTC_SRAM_END_ADDRESS          0xFF


/*=========================================================
 * Hours Register Bits
 *=========================================================*/

/*
 * Hours register:
 *
 * Bit 6:
 * 0 -> 24-hour mode
 * 1 -> 12-hour mode
 *
 * Bit 5 in 12-hour mode:
 * 0 -> AM
 * 1 -> PM
 */
#define RTC_HOUR_MODE_BIT             6
#define RTC_AM_PM_BIT                 5


/*=========================================================
 * Month Register Bits
 *=========================================================*/

#define RTC_CENTURY_BIT               7


/*=========================================================
 * Alarm Register Bits
 *=========================================================*/

/*
 * Bit 7 of each Alarm 1 register is a mask bit:
 *
 * A1M1 -> Alarm 1 seconds register
 * A1M2 -> Alarm 1 minutes register
 * A1M3 -> Alarm 1 hours register
 * A1M4 -> Alarm 1 day/date register
 */
#define RTC_ALARM_MASK_BIT            7

#define RTC_ALARM1_A1M1_BIT           7
#define RTC_ALARM1_A1M2_BIT           7
#define RTC_ALARM1_A1M3_BIT           7
#define RTC_ALARM1_A1M4_BIT           7

/*
 * Bit 7 of each Alarm 2 register is a mask bit:
 *
 * A2M2 -> Alarm 2 minutes register
 * A2M3 -> Alarm 2 hours register
 * A2M4 -> Alarm 2 day/date register
 */
#define RTC_ALARM2_A2M2_BIT           7
#define RTC_ALARM2_A2M3_BIT           7
#define RTC_ALARM2_A2M4_BIT           7


/*=========================================================
 * Day/Date Selection Bit
 *=========================================================*/

/*
 * Bit 6 in Alarm 1/2 day-date register:
 *
 * 0 -> Register represents a date from 1 to 31
 * 1 -> Register represents a day from 1 to 7
 */
#define RTC_ALARM_DAY_DATE_BIT        6


/*=========================================================
 * Control Register Bits
 *=========================================================*/

#define RTC_CONTROL_A1IE_BIT          0
#define RTC_CONTROL_A2IE_BIT          1
#define RTC_CONTROL_INTCN_BIT         2
#define RTC_CONTROL_RS1_BIT           3
#define RTC_CONTROL_RS2_BIT           4
#define RTC_CONTROL_CONV_BIT          5
#define RTC_CONTROL_BBSQW_BIT         6
#define RTC_CONTROL_EOSC_BIT          7


/*=========================================================
 * Status Register Bits
 *=========================================================*/

#define RTC_STATUS_A1F_BIT            0
#define RTC_STATUS_A2F_BIT            1
#define RTC_STATUS_BSY_BIT            2
#define RTC_STATUS_EN32KHZ_BIT        3
#define RTC_STATUS_OSF_BIT            7


/*=========================================================
 * Register Masks
 *=========================================================*/

#define RTC_SECONDS_MASK              0x7F
#define RTC_MINUTES_MASK              0x7F

#define RTC_24_HOUR_MASK              0x3F
#define RTC_12_HOUR_MASK              0x1F

#define RTC_DAY_MASK                  0x07
#define RTC_DATE_MASK                 0x3F
#define RTC_MONTH_MASK                0x1F

#define RTC_ALARM_DAY_DATE_MASK       0x3F


/*=========================================================
 * Useful Register Values
 *=========================================================*/

#define RTC_24_HOUR_BIT_VALUE         0
#define RTC_12_HOUR_BIT_VALUE         1

#define RTC_AM_BIT_VALUE              0
#define RTC_PM_BIT_VALUE              1

#define RTC_ALARM_DATE_MODE           0
#define RTC_ALARM_DAY_MODE            1

#ifndef NULL
#define NULL ((void *)0)
#endif

#define DS3231_SLAVE_ADDRESS          RTC_SLAVE_ADDRESS

#define DS3231_SECONDS_REG            RTC_SECONDS_REG
#define DS3231_DAY_REG                RTC_DAY_REG

#define DS3231_ALARM1_SECONDS_REG     RTC_ALARM1_SECONDS_REG
#define DS3231_ALARM2_MINUTES_REG     RTC_ALARM2_MINUTES_REG

#define DS3231_CONTROL_REG            RTC_CONTROL_REG
#define DS3231_STATUS_REG             RTC_STATUS_REG

#define DS3231_HOUR_MODE_BIT          RTC_HOUR_MODE_BIT
#define DS3231_AM_PM_BIT              RTC_AM_PM_BIT
#define DS3231_CENTURY_BIT            RTC_CENTURY_BIT

#define DS3231_CONTROL_EOSC_BIT       RTC_CONTROL_EOSC_BIT
#define DS3231_CONTROL_INTCN_BIT      RTC_CONTROL_INTCN_BIT
#define DS3231_CONTROL_RS1_BIT        RTC_CONTROL_RS1_BIT
#define DS3231_CONTROL_RS2_BIT        RTC_CONTROL_RS2_BIT
#define DS3231_CONTROL_A1IE_BIT       RTC_CONTROL_A1IE_BIT
#define DS3231_CONTROL_A2IE_BIT       RTC_CONTROL_A2IE_BIT

#define DS3231_STATUS_A1F_BIT         RTC_STATUS_A1F_BIT
#define DS3231_STATUS_A2F_BIT         RTC_STATUS_A2F_BIT

#endif /* INCLUDE_HAL_RTC_RTC_PRIVATE_H_ */
