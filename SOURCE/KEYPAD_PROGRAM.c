/*
 * KEYPAD_PROGRAM.c
 *
 * Created on: Aug 20, 2026
 * Author: Abdullah
 */

#include <util/delay.h>

#include "../INCLUDE/LIB/STD_TYPES.h"
#include "../INCLUDE/LIB/BIT_MATH.h"

#include "../INCLUDE/MCAL/DIO/DIO_INTERFACE.h"

#include "../INCLUDE/HAL/KEYPAD/KEYPAD_CFG.h"
#include "../INCLUDE/HAL/KEYPAD/KEYPAD_PRIVATE.h"
#include "../INCLUDE/HAL/KEYPAD/KEYPAD_INTERFACE.h"


/*========================================================
 *                  KEYPAD MAP
 *========================================================*/

static u8 G_u8KeyValues[ROW_NO][COL_NO] =
{
    {'7', '8', '9', '/'},
    {'4', '5', '6', '*'},
    {'1', '2', '3', '-'},
    {'C', '0', '=', '+'}
};


/*========================================================
 *                  KEYPAD PINS
 *========================================================*/

static u8 G_u8Rows[ROW_NO] = KEYPAD_ROWS;
static u8 G_u8Cols[COL_NO] = KEYPAD_COLS;


/*========================================================
 *                  KEYPAD INITIALIZATION
 *========================================================*/

void HKEPAD_voidInit(void)
{
    u8 Local_u8Iterator;


    /*
     * Rows:
     * PB0 -> PB3
     * OUTPUT
     *
     * Initial state = HIGH
     */

    for (Local_u8Iterator = 0;
         Local_u8Iterator < ROW_NO;
         Local_u8Iterator++)
    {
        MDIO_voidSetPinDirection(
            KEYPAD_PORT,
            G_u8Rows[Local_u8Iterator],
            DIO_OUTPUT
        );

        MDIO_voidSetPinValue(
            KEYPAD_PORT,
            G_u8Rows[Local_u8Iterator],
            DIO_HIGH
        );
    }


    /*
     * Columns:
     * PB4 -> PB7
     * INPUT
     *
     * Enable internal pull-up
     */

    for (Local_u8Iterator = 0;
         Local_u8Iterator < COL_NO;
         Local_u8Iterator++)
    {
        MDIO_voidSetPinDirection(
            KEYPAD_PORT,
            G_u8Cols[Local_u8Iterator],
            DIO_INPUT
        );

        MDIO_voidSetPinValue(
            KEYPAD_PORT,
            G_u8Cols[Local_u8Iterator],
            DIO_HIGH
        );
    }
}


/*========================================================
 *                  GET PRESSED KEY
 *========================================================*/

u8 HKEYPAD_u8GetPressedKey(void)
{
    u8 Local_u8KeyValue = KEY_NOT_PRESSED;

    u8 Local_u8RowCounter;
    u8 Local_u8ColCounter;

    u8 Local_u8PinValue;


    /*
     * Scan rows one by one
     */

    for (Local_u8RowCounter = 0;
         Local_u8RowCounter < ROW_NO;
         Local_u8RowCounter++)
    {
        /*
         * Set all rows HIGH first
         */

        for (u8 Local_u8Temp = 0;
             Local_u8Temp < ROW_NO;
             Local_u8Temp++)
        {
            MDIO_voidSetPinValue(
                KEYPAD_PORT,
                G_u8Rows[Local_u8Temp],
                DIO_HIGH
            );
        }


        /*
         * Activate current row
         * Active LOW
         */

        MDIO_voidSetPinValue(
            KEYPAD_PORT,
            G_u8Rows[Local_u8RowCounter],
            DIO_LOW
        );


        /*
         * Check columns
         */

        for (Local_u8ColCounter = 0;
             Local_u8ColCounter < COL_NO;
             Local_u8ColCounter++)
        {
            Local_u8PinValue =
                MDIO_PinstatusGetPinValue(
                    KEYPAD_PORT,
                    G_u8Cols[Local_u8ColCounter]
                );


            /*
             * Column became LOW
             * => key is pressed
             */

            if (Local_u8PinValue == DIO_LOW)
            {
                /*
                 * Debouncing
                 */

                _delay_ms(KEYPAD_DEBOUNCE_DELAY);


                /*
                 * Check again
                 */

                Local_u8PinValue =
                    MDIO_PinstatusGetPinValue(
                        KEYPAD_PORT,
                        G_u8Cols[Local_u8ColCounter]
                    );


                if (Local_u8PinValue == DIO_LOW)
                {
                    /*
                     * Get key
                     */

                    Local_u8KeyValue =
                        G_u8KeyValues
                        [Local_u8RowCounter]
                        [Local_u8ColCounter];


                    /*
                     * Wait until key is released
                     */

                    while (
                        MDIO_PinstatusGetPinValue(
                            KEYPAD_PORT,
                            G_u8Cols[Local_u8ColCounter]
                        ) == DIO_LOW
                    )
                    {
                        /* Wait */
                    }


                    _delay_ms(50);

                    /*
                     * Restore row HIGH
                     */

                    MDIO_voidSetPinValue(
                        KEYPAD_PORT,
                        G_u8Rows[Local_u8RowCounter],
                        DIO_HIGH
                    );

                    return Local_u8KeyValue;
                }
            }
        }


        /*
         * Restore current row HIGH
         */

        MDIO_voidSetPinValue(
            KEYPAD_PORT,
            G_u8Rows[Local_u8RowCounter],
            DIO_HIGH
        );
    }


    /*
     * No key pressed
     */

    return Local_u8KeyValue;
}
