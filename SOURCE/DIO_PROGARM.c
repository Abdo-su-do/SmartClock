/*
 * DIO_PROGRAM.c
 *
 *  Created on: Aug 18, 2026
 *      Author: MALAH
 */

/* LIB */
#include "../INCLUDE/LIB/BIT_MATH.h"
#include "../INCLUDE/LIB/STD_TYPES.h"

/* DIO */
#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"
#include "../INCLUDE/MCAL/DIO/DIO_PRIVATE.h"
#include "../INCLUDE/MCAL/DIO/DIO_CFG.h"


/*========================================================
 *                  DIO INITIALIZATION
 *========================================================*/

void MDIO_voidInit(void)
{
    /* Set Port Directions */

    DDRA_REG = CONC_BIT(
        PORTA_PIN7_DIRECTION,
        PORTA_PIN6_DIRECTION,
        PORTA_PIN5_DIRECTION,
        PORTA_PIN4_DIRECTION,
        PORTA_PIN3_DIRECTION,
        PORTA_PIN2_DIRECTION,
        PORTA_PIN1_DIRECTION,
        PORTA_PIN0_DIRECTION
    );

    DDRB_REG = CONC_BIT(
        PORTB_PIN7_DIRECTION,
        PORTB_PIN6_DIRECTION,
        PORTB_PIN5_DIRECTION,
        PORTB_PIN4_DIRECTION,
        PORTB_PIN3_DIRECTION,
        PORTB_PIN2_DIRECTION,
        PORTB_PIN1_DIRECTION,
        PORTB_PIN0_DIRECTION
    );

    DDRC_REG = CONC_BIT(
        PORTC_PIN7_DIRECTION,
        PORTC_PIN6_DIRECTION,
        PORTC_PIN5_DIRECTION,
        PORTC_PIN4_DIRECTION,
        PORTC_PIN3_DIRECTION,
        PORTC_PIN2_DIRECTION,
        PORTC_PIN1_DIRECTION,
        PORTC_PIN0_DIRECTION
    );

    DDRD_REG = CONC_BIT(
        PORTD_PIN7_DIRECTION,
        PORTD_PIN6_DIRECTION,
        PORTD_PIN5_DIRECTION,
        PORTD_PIN4_DIRECTION,
        PORTD_PIN3_DIRECTION,
        PORTD_PIN2_DIRECTION,
        PORTD_PIN1_DIRECTION,
        PORTD_PIN0_DIRECTION
    );


    /* Set Initial Port Values */

    PORTA_REG = CONC_BIT(
        PORTA_PIN7_VALUE,
        PORTA_PIN6_VALUE,
        PORTA_PIN5_VALUE,
        PORTA_PIN4_VALUE,
        PORTA_PIN3_VALUE,
        PORTA_PIN2_VALUE,
        PORTA_PIN1_VALUE,
        PORTA_PIN0_VALUE
    );

    PORTB_REG = CONC_BIT(
        PORTB_PIN7_VALUE,
        PORTB_PIN6_VALUE,
        PORTB_PIN5_VALUE,
        PORTB_PIN4_VALUE,
        PORTB_PIN3_VALUE,
        PORTB_PIN2_VALUE,
        PORTB_PIN1_VALUE,
        PORTB_PIN0_VALUE
    );

    PORTC_REG = CONC_BIT(
        PORTC_PIN7_VALUE,
        PORTC_PIN6_VALUE,
        PORTC_PIN5_VALUE,
        PORTC_PIN4_VALUE,
        PORTC_PIN3_VALUE,
        PORTC_PIN2_VALUE,
        PORTC_PIN1_VALUE,
        PORTC_PIN0_VALUE
    );

    PORTD_REG = CONC_BIT(
        PORTD_PIN7_VALUE,
        PORTD_PIN6_VALUE,
        PORTD_PIN5_VALUE,
        PORTD_PIN4_VALUE,
        PORTD_PIN3_VALUE,
        PORTD_PIN2_VALUE,
        PORTD_PIN1_VALUE,
        PORTD_PIN0_VALUE
    );
}


/*========================================================
 *              SET PIN DIRECTION
 *========================================================*/

void MDIO_voidSetPinDirection(
        DIO_PORTS A_DIOPort,
        DIO_PINS A_DIOPin,
        PIN_DIRECTION A_PinDirection)
{
    if ((A_DIOPort <= PORTD) &&
        (A_DIOPin <= PIN7) &&
        (A_PinDirection <= DIO_OUTPUT))
    {
        switch (A_DIOPort)
        {
            case PORTA:

                if (A_PinDirection == DIO_OUTPUT)
                {
                    SET_BIT(DDRA_REG, A_DIOPin);
                }
                else
                {
                    CLR_BIT(DDRA_REG, A_DIOPin);
                }

                break;


            case PORTB:

                if (A_PinDirection == DIO_OUTPUT)
                {
                    SET_BIT(DDRB_REG, A_DIOPin);
                }
                else
                {
                    CLR_BIT(DDRB_REG, A_DIOPin);
                }

                break;


            case PORTC:

                if (A_PinDirection == DIO_OUTPUT)
                {
                    SET_BIT(DDRC_REG, A_DIOPin);
                }
                else
                {
                    CLR_BIT(DDRC_REG, A_DIOPin);
                }

                break;


            case PORTD:

                if (A_PinDirection == DIO_OUTPUT)
                {
                    SET_BIT(DDRD_REG, A_DIOPin);
                }
                else
                {
                    CLR_BIT(DDRD_REG, A_DIOPin);
                }

                break;


            default:
                break;
        }
    }
}


/*========================================================
 *                  SET PIN VALUE
 *========================================================*/

void MDIO_voidSetPinValue(
        DIO_PORTS A_DIOPort,
        DIO_PINS A_DIOPin,
        PIN_STATUS A_PinValue)
{
    if ((A_DIOPort <= PORTD) &&
        (A_DIOPin <= PIN7) &&
        (A_PinValue <= DIO_HIGH))
    {
        switch (A_DIOPort)
        {
            case PORTA:

                if (A_PinValue == DIO_HIGH)
                {
                    SET_BIT(PORTA_REG, A_DIOPin);
                }
                else
                {
                    CLR_BIT(PORTA_REG, A_DIOPin);
                }

                break;


            case PORTB:

                if (A_PinValue == DIO_HIGH)
                {
                    SET_BIT(PORTB_REG, A_DIOPin);
                }
                else
                {
                    CLR_BIT(PORTB_REG, A_DIOPin);
                }

                break;


            case PORTC:

                if (A_PinValue == DIO_HIGH)
                {
                    SET_BIT(PORTC_REG, A_DIOPin);
                }
                else
                {
                    CLR_BIT(PORTC_REG, A_DIOPin);
                }

                break;


            case PORTD:

                if (A_PinValue == DIO_HIGH)
                {
                    SET_BIT(PORTD_REG, A_DIOPin);
                }
                else
                {
                    CLR_BIT(PORTD_REG, A_DIOPin);
                }

                break;


            default:
                break;
        }
    }
}


/*========================================================
 *                  TOGGLE PIN VALUE
 *========================================================*/

void MDIO_voidTogglePinValue(
        DIO_PORTS A_DIOPort,
        DIO_PINS A_DIOPin)
{
    if ((A_DIOPort <= PORTD) &&
        (A_DIOPin <= PIN7))
    {
        switch (A_DIOPort)
        {
            case PORTA:
                TOGGLE_BIT(PORTA_REG, A_DIOPin);
                break;

            case PORTB:
                TOGGLE_BIT(PORTB_REG, A_DIOPin);
                break;

            case PORTC:
                TOGGLE_BIT(PORTC_REG, A_DIOPin);
                break;

            case PORTD:
                TOGGLE_BIT(PORTD_REG, A_DIOPin);
                break;

            default:
                break;
        }
    }
}


/*========================================================
 *                  SET PORT DIRECTION
 *========================================================*/

void MDIO_voidSetPortDirection(
        DIO_PORTS A_DIOPort,
        u8 A_u8PortDirection)
{
    if (A_DIOPort <= PORTD)
    {
        switch (A_DIOPort)
        {
            case PORTA:
                DDRA_REG = A_u8PortDirection;
                break;

            case PORTB:
                DDRB_REG = A_u8PortDirection;
                break;

            case PORTC:
                DDRC_REG = A_u8PortDirection;
                break;

            case PORTD:
                DDRD_REG = A_u8PortDirection;
                break;

            default:
                break;
        }
    }
}


/*========================================================
 *                  SET PORT VALUE
 *========================================================*/

void MDIO_voidSetPortValue(
        DIO_PORTS A_DIOPort,
        u8 A_u8PortValue)
{
    if (A_DIOPort <= PORTD)
    {
        switch (A_DIOPort)
        {
            case PORTA:
                PORTA_REG = A_u8PortValue;
                break;

            case PORTB:
                PORTB_REG = A_u8PortValue;
                break;

            case PORTC:
                PORTC_REG = A_u8PortValue;
                break;

            case PORTD:
                PORTD_REG = A_u8PortValue;
                break;

            default:
                break;
        }
    }
}


/*========================================================
 *                  GET PIN VALUE
 *========================================================*/

PIN_STATUS MDIO_PinstatusGetPinValue(
        DIO_PORTS A_DIOPort,
        DIO_PINS A_DIOPin)
{
    PIN_STATUS PIN_VALUE = DIO_LOW;

    if ((A_DIOPort <= PORTD) &&
        (A_DIOPin <= PIN7))
    {
        switch (A_DIOPort)
        {
            case PORTA:
                PIN_VALUE = GET_BIT(PINA_REG, A_DIOPin);
                break;

            case PORTB:
                PIN_VALUE = GET_BIT(PINB_REG, A_DIOPin);
                break;

            case PORTC:
                PIN_VALUE = GET_BIT(PINC_REG, A_DIOPin);
                break;

            case PORTD:
                PIN_VALUE = GET_BIT(PIND_REG, A_DIOPin);
                break;

            default:
                PIN_VALUE = DIO_LOW;
                break;
        }
    }

    return PIN_VALUE;
}
