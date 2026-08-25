/*
 * RTC_PRIVATE.h
 *
 * DS3231 RTC private definitions
 */

#ifndef RTC_PRIVATE_H_
#define RTC_PRIVATE_H_


/*================ DS3231 I2C Address ================*/

/*
 * DS3231 fixed 7-bit address = 0x68
 *
 * Write address = 11010000 = 0xD0
 * Read address  = 11010001 = 0xD1
 */
#define DS3231_SLAVE_ADDRESS          0x68
#define DS3231_WRITE_ADDRESS          0xD0
#define DS3231_READ_ADDRESS           0xD1


/*================ Time and Date Registers ================*/

#define DS3231_SECONDS_REG            0x00
#define DS3231_MINUTES_REG            0x01
#define DS3231_HOURS_REG              0x02
#define DS3231_DAY_REG                0x03
#define DS3231_DATE_REG               0x04
#define DS3231_MONTH_REG              0x05
#define DS3231_YEAR_REG               0x06


/*================ Alarm 1 Registers ================*/

#define DS3231_ALARM1_SECONDS_REG     0x07
#define DS3231_ALARM1_MINUTES_REG     0x08
#define DS3231_ALARM1_HOURS_REG       0x09
#define DS3231_ALARM1_DAY_DATE_REG    0x0A


/*================ Alarm 2 Registers ================*/

#define DS3231_ALARM2_MINUTES_REG     0x0B
#define DS3231_ALARM2_HOURS_REG       0x0C
#define DS3231_ALARM2_DAY_DATE_REG    0x0D


/*================ Control and Status Registers ================*/

#define DS3231_CONTROL_REG            0x0E
#define DS3231_STATUS_REG             0x0F
#define DS3231_AGING_OFFSET_REG       0x10
#define DS3231_TEMP_MSB_REG           0x11
#define DS3231_TEMP_LSB_REG           0x12


/*================ Hours Register Bits ================*/

#define DS3231_HOUR_MODE_BIT          6
#define DS3231_AM_PM_BIT              5


/*================ Month Register Bits ================*/

#define DS3231_CENTURY_BIT            7


/*================ Control Register Bits ================*/

#define DS3231_CONTROL_A1IE_BIT       0
#define DS3231_CONTROL_A2IE_BIT       1
#define DS3231_CONTROL_INTCN_BIT      2
#define DS3231_CONTROL_RS1_BIT        3
#define DS3231_CONTROL_RS2_BIT        4
#define DS3231_CONTROL_CONV_BIT       5
#define DS3231_CONTROL_BBSQW_BIT      6
#define DS3231_CONTROL_EOSC_BIT       7


/*================ Status Register Bits ================*/

#define DS3231_STATUS_A1F_BIT         0
#define DS3231_STATUS_A2F_BIT         1
#define DS3231_STATUS_BSY_BIT         2
#define DS3231_STATUS_EN32KHZ_BIT     3
#define DS3231_STATUS_OSF_BIT         7


/*================ Private Conversion Functions ================*/

static u8 RTC_u8DecimalToBCD(u8 A_u8DecimalValue);
static u8 RTC_u8BCDToDecimal(u8 A_u8BCDValue);


#endif /* RTC_PRIVATE_H_ */