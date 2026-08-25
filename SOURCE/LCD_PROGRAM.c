/*
 * LCD_INTERFACE.c
 *
 * Created: 8/23/2026 8:20:51 PM
 *  Author: FaresKan
 */ 



#include "../../INCLUDE/HAL/LCD/LCD_INTERFACE.h"
#include <util/delay.h>


static void HLCD_voidSend4Bits(u8 A_u8Data)
{
	MDIO_voidSetPinValue(LCD_DATA_PORT,PIN4,GET_BIT(A_u8Data, 0));
	MDIO_voidSetPinValue(LCD_DATA_PORT,PIN5,GET_BIT(A_u8Data, 1));
	MDIO_voidSetPinValue(LCD_DATA_PORT,PIN6,GET_BIT(A_u8Data, 2));
	MDIO_voidSetPinValue(LCD_DATA_PORT,PIN7,GET_BIT(A_u8Data, 3));
	/* Send enable pulse */
	MDIO_voidSetPinValue(LCD_E_PORT, LCD_E_PIN, DIO_HIGH);
	_delay_us(1);
	MDIO_voidSetPinValue(LCD_E_PORT, LCD_E_PIN, DIO_LOW);
	_delay_us(50);
}

void HLCD_voidSendCommand(u8 A_u8Command)
{
	/* Select command register */
	MDIO_voidSetPinValue(LCD_RS_PORT, LCD_RS_PIN, DIO_LOW);

	/* Select write operation */
	MDIO_voidSetPinValue(LCD_RW_PORT, LCD_RW_PIN, DIO_LOW);

	#if LCD_MODE == LCD_MODE_8BIT

	/* Send the complete command */
	MDIO_voidSetPortValue(LCD_DATA_PORT, A_u8Command);

	/* Enable pulse */
	MDIO_voidSetPinValue(LCD_E_PORT, LCD_E_PIN, DIO_HIGH);
	_delay_us(1);
	MDIO_voidSetPinValue(LCD_E_PORT, LCD_E_PIN, DIO_LOW);

	#elif LCD_MODE == LCD_MODE_4BIT

	/* Send high nibble: bits 7,6,5,4 */
	MDIO_voidSetPinValue(LCD_DATA_PORT,PIN4,GET_BIT(A_u8Command, 4));
    MDIO_voidSetPinValue(LCD_DATA_PORT,PIN5,GET_BIT(A_u8Command, 5));
	MDIO_voidSetPinValue(LCD_DATA_PORT,PIN6,GET_BIT(A_u8Command, 6));
	MDIO_voidSetPinValue(LCD_DATA_PORT,PIN7,GET_BIT(A_u8Command, 7));

	/* Enable pulse */
	MDIO_voidSetPinValue(LCD_E_PORT, LCD_E_PIN, DIO_HIGH);
	_delay_us(1);
	MDIO_voidSetPinValue(LCD_E_PORT, LCD_E_PIN, DIO_LOW);

	/* Send low nibble: bits 3,2,1,0 */
	MDIO_voidSetPinValue(LCD_DATA_PORT,PIN4,GET_BIT(A_u8Command, 0));
	MDIO_voidSetPinValue(LCD_DATA_PORT,PIN5,GET_BIT(A_u8Command, 1));
	MDIO_voidSetPinValue(LCD_DATA_PORT,PIN6,GET_BIT(A_u8Command, 2));
	MDIO_voidSetPinValue(LCD_DATA_PORT,PIN7,GET_BIT(A_u8Command, 3));

	/* Enable pulse */
	MDIO_voidSetPinValue(LCD_E_PORT, LCD_E_PIN, DIO_HIGH);
	_delay_us(1);
	MDIO_voidSetPinValue(LCD_E_PORT, LCD_E_PIN, DIO_LOW);

	#else
	#error "Wrong LCD_MODE configuration"
	#endif

	/* Wait until the command is executed */
	_delay_ms(2);
}


void HLCD_voidSendData(u8 A_u8Data)
{
    /* RS = 1: select data register */
    MDIO_voidSetPinValue(LCD_RS_PORT,LCD_RS_PIN,DIO_HIGH );

    /* R/W = 0: select write operation */
    MDIO_voidSetPinValue( LCD_RW_PORT, LCD_RW_PIN, DIO_LOW );

#if LCD_MODE == LCD_MODE_8BIT

    /* Send the complete 8-bit data */
    MDIO_voidSetPortValue(LCD_DATA_PORT,A_u8Data);

    /* Generate enable pulse */
    MDIO_voidSetPinValue(LCD_E_PORT,LCD_E_PIN,DIO_HIGH);
    _delay_us(1);
    MDIO_voidSetPinValue( LCD_E_PORT,LCD_E_PIN,DIO_LOW);

#elif LCD_MODE == LCD_MODE_4BIT

    /*
     * Send high nibble first.
     * LCD D4 receives data bit 4.
     * LCD D5 receives data bit 5.
     * LCD D6 receives data bit 6.
     * LCD D7 receives data bit 7.
     */

    MDIO_voidSetPinValue( LCD_DATA_PORT, PIN4, GET_BIT(A_u8Data, 4));
    MDIO_voidSetPinValue(LCD_DATA_PORT,PIN5,GET_BIT(A_u8Data, 5));
    MDIO_voidSetPinValue(LCD_DATA_PORT,PIN6,GET_BIT(A_u8Data, 6));
    MDIO_voidSetPinValue( LCD_DATA_PORT,PIN7, GET_BIT(A_u8Data, 7));

    /* Generate enable pulse */
    MDIO_voidSetPinValue( LCD_E_PORT, LCD_E_PIN, DIO_HIGH);
    _delay_us(1);
    MDIO_voidSetPinValue( LCD_E_PORT, LCD_E_PIN, DIO_LOW);

    /*
     * Send low nibble second.
     * LCD D4 receives data bit 0.
     * LCD D5 receives data bit 1.
     * LCD D6 receives data bit 2.
     * LCD D7 receives data bit 3.
     */

    MDIO_voidSetPinValue(LCD_DATA_PORT,PIN4, GET_BIT(A_u8Data, 0));
    MDIO_voidSetPinValue(LCD_DATA_PORT,PIN5, GET_BIT(A_u8Data, 1));
    MDIO_voidSetPinValue( LCD_DATA_PORT, PIN6, GET_BIT(A_u8Data, 2));
    MDIO_voidSetPinValue(LCD_DATA_PORT,PIN7,GET_BIT(A_u8Data, 3));
    /* Generate enable pulse */
    MDIO_voidSetPinValue(LCD_E_PORT,LCD_E_PIN,DIO_HIGH);
    _delay_us(1);
    MDIO_voidSetPinValue(LCD_E_PORT,LCD_E_PIN,DIO_LOW);

#else
    #error "Wrong LCD_MODE configuration"
#endif

    /* Wait until the LCD processes the data */
    _delay_ms(2);
}

void HLCD_voidInit(void)
{
    /* Configure LCD control pins as outputs */
    MDIO_voidSetPinDirection(LCD_RS_PORT, LCD_RS_PIN, DIO_OUTPUT);
    MDIO_voidSetPinDirection(LCD_RW_PORT, LCD_RW_PIN, DIO_OUTPUT);
    MDIO_voidSetPinDirection(LCD_E_PORT,  LCD_E_PIN,  DIO_OUTPUT);

    /* Set initial values for control pins */
    MDIO_voidSetPinValue(LCD_RS_PORT, LCD_RS_PIN, DIO_LOW);
    MDIO_voidSetPinValue(LCD_RW_PORT, LCD_RW_PIN, DIO_LOW);
    MDIO_voidSetPinValue(LCD_E_PORT,  LCD_E_PIN,  DIO_LOW);

#if LCD_MODE == LCD_MODE_8BIT

    /* Configure all data pins as outputs */
    MDIO_voidSetPortDirection(LCD_DATA_PORT, 0xFF);

#elif LCD_MODE == LCD_MODE_4BIT

    /* Configure D4-D7 as outputs */
    MDIO_voidSetPinDirection(LCD_DATA_PORT, PIN4 , DIO_OUTPUT);
    MDIO_voidSetPinDirection(LCD_DATA_PORT, PIN5 , DIO_OUTPUT);
    MDIO_voidSetPinDirection(LCD_DATA_PORT, PIN6 , DIO_OUTPUT);
    MDIO_voidSetPinDirection(LCD_DATA_PORT, PIN7 , DIO_OUTPUT);

#else

#error "Invalid LCD_MODE configuration"

#endif

    /* Wait for the LCD to finish powering up */
    _delay_ms(40);

#if LCD_MODE == LCD_MODE_8BIT

    /*
     * 8-bit mode
     * Two lines
     * 5x8 font
     */
    HLCD_voidSendCommand(SET_LCD);

#elif LCD_MODE == LCD_MODE_4BIT

    /*
     * Special initialization sequence required
     * when the LCD operates in 4-bit mode.
     */
    HLCD_voidSend4Bits(0x03);
    _delay_ms(5);

    HLCD_voidSend4Bits(0x03);
    _delay_us(150);

    HLCD_voidSend4Bits(0x03);
    _delay_us(150);

    /* Change the LCD to 4-bit mode */
    HLCD_voidSend4Bits(0x02);
    _delay_us(150);

    /* Function set: 4-bit, two lines, 5x8 font */
    HLCD_voidSendCommand(SET_LCD);

#endif

    /* Display ON, cursor OFF, blinking OFF */
    HLCD_voidSendCommand(DISPLAY_CONTROL);
    _delay_ms(1);

    /* Clear the LCD */
    HLCD_voidSendCommand(CLEAR_DISPLAY);
    _delay_ms(2);

    /* Cursor moves right after every character */
    HLCD_voidSendCommand(SET_ENTRY_MODE);
}


void HLCD_voidClearDisplay(void){
	// Clear Display
	HLCD_voidSendCommand(CLEAR_DISPLAY);
	
	_delay_ms(2);
}

void HLCD_voidSendString(u8 *A_Pu8String){
	u8 Local_u8Counter = 0;

	while (A_Pu8String[Local_u8Counter] != '\0')
	{
		HLCD_voidSendData(A_Pu8String[Local_u8Counter]);

		Local_u8Counter++;
	}
}

void HLCD_voidDisplayNumber (u32 A_u32Number){
	u8 Local_u8Digits[10];
	u8 Local_u8Counter = 0;

	if (A_u32Number == 0)
	{
		HLCD_voidSendData('0');
		return;
	}

	while (A_u32Number != 0)
	{
		Local_u8Digits[Local_u8Counter] = A_u32Number % 10;

		A_u32Number /= 10;

		Local_u8Counter++;
	}

	while (Local_u8Counter > 0)
	{
		Local_u8Counter--;

		HLCD_voidSendData(Local_u8Digits[Local_u8Counter] + '0');
	}
}


void HLCD_voidGoToPos(LCD_ROWS A_LcdRowNo,LCD_COLS A_LcdColNo){
	u8 Local_u8Address=0;

	if (A_LcdColNo >= LCD_NUMBER_OF_COLUMNS)
	{
		return;
	}

	switch (A_LcdRowNo)
	{
		case L1:Local_u8Address = LCD_LINE1_ADDRESS + A_LcdColNo; break;
		case L2:Local_u8Address = LCD_LINE2_ADDRESS + A_LcdColNo; break;

		#if LCD_NUMBER_OF_ROWS == 4
		case L3:Local_u8Address =  LCD_LINE3_ADDRESS + A_LcdColNo; break;
		case L4: Local_u8Address = LCD_LINE4_ADDRESS + A_LcdColNo; break;
		#endif

		default:
		return;
	}

	HLCD_voidSendCommand(0x80 | Local_u8Address);
}


////////////////////////////////////////////////////////////////////////


/*
 * Every word has three characters plus '\0'.
 */
static const u8 G_u8Days[7][4] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
static const u8 G_u8Months[12][4] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};


static void HLCD_voidDisplayTwoDigits(u8 A_u8Number)
{
    if (A_u8Number < 10){HLCD_voidSendData('0');}
    HLCD_voidDisplayNumber(A_u8Number);
}
static void HLCD_voidDisplayFloatOneDigit(f32 A_f32Number)
{
	u32 Local_u32IntegerPart;
	u8  Local_u8DecimalPart;

	if (A_f32Number < 0.0f)
	{
		HLCD_voidSendData('-');
		A_f32Number = -A_f32Number;
	}

	Local_u32IntegerPart = (u32)A_f32Number;

	Local_u8DecimalPart =
	(u8)(((A_f32Number - Local_u32IntegerPart) * 10.0f) + 0.5f);

	/* Handle rounding, for example 25.96 becomes 26.0 */
	if (Local_u8DecimalPart >= 10)
	{
		Local_u32IntegerPart++;
		Local_u8DecimalPart = 0;
	}

	HLCD_voidDisplayNumber(Local_u32IntegerPart);
	HLCD_voidSendData('.');
	HLCD_voidDisplayNumber(Local_u8DecimalPart);
}



void HLCD_voidDisplayDateTime(u8 A_u8Hours,u8 A_u8Minutes,u8 A_u8Seconds,LCD_CLOCK_PERIOD A_enuPeriod,LCD_DAYS A_enuDay,u8 A_u8Date,u8 A_u8Month,u16 A_u16Year,LCD_CLOCK_MODE A_enuMode){
    /*
     * Check day and month.
     */
    if ((A_enuDay > LCD_SATURDAY) ||(A_u8Month < 1) || (A_u8Month > 12))
    {  return; }
    /*
     * Line 1:
     * DIGITAL CLOCK    12H
     */
    HLCD_voidGoToPos(L1, C1);
    HLCD_voidSendString((u8 *)"DIGITAL CLOCK    ");

    if (A_enuMode == LCD_MODE_12_HOUR){ HLCD_voidSendString((u8 *)"12H");}
    else{    HLCD_voidSendString((u8 *)"24H"); }
    /*
     * Line 2:
     * Time: 08:35:09 PM
     */
    HLCD_voidGoToPos(L2, C1);
    HLCD_voidSendString((u8 *)"Time: ");
    HLCD_voidDisplayTwoDigits(A_u8Hours);
    HLCD_voidSendData(':');
    HLCD_voidDisplayTwoDigits(A_u8Minutes);
    HLCD_voidSendData(':');
    HLCD_voidDisplayTwoDigits(A_u8Seconds);
    if (A_enuMode == LCD_MODE_12_HOUR)
    {
        if (A_enuPeriod == LCD_AM_PERIOD){  HLCD_voidSendString((u8 *)" AM"); }
        else{ HLCD_voidSendString((u8 *)" PM"); }
        /* Complete the 20-character line */
        HLCD_voidSendString((u8 *)"   ");
    }
    else
    {
        /* Clear the AM/PM area */
        HLCD_voidSendString((u8 *)"      ");
    }


    /*
     * Line 3:
     * Date: 24/08/2026
     */
    HLCD_voidGoToPos(L3, C1);
    HLCD_voidSendString((u8 *)"Date: ");
    HLCD_voidDisplayTwoDigits(A_u8Date);
    HLCD_voidSendData('/');
    HLCD_voidDisplayTwoDigits(A_u8Month);
    HLCD_voidSendData('/');
    HLCD_voidDisplayNumber(A_u16Year);
    /* Complete the 20-character line */
    HLCD_voidSendString((u8 *)"    ");
    /*
     * Line 4:
     *  MON   24 AUG
     */
    HLCD_voidGoToPos(L4, C1);
    HLCD_voidSendData(' ');
    /* Day index: 0 to 6 */
    HLCD_voidSendString((u8 *)G_u8Days[A_enuDay]);
    HLCD_voidSendString((u8 *)"   ");
    HLCD_voidDisplayTwoDigits(A_u8Date);
    HLCD_voidSendData(' ');
    /* Month number 1 corresponds to array index 0 */
    HLCD_voidSendString((u8 *)G_u8Months[A_u8Month - 1]);
    /* Complete the 20-character line */
    HLCD_voidSendString((u8 *)"       ");
}

void HLCD_voidDisplayTempHumidity(f32 A_f32TemperatureCelsius,f32 A_f32Humidity,LCD_TEMP_UNIT A_TempUnit){
	f32 Local_f32DisplayedTemperature;
	u8  Local_u8UnitCharacter;

	/* Select temperature unit */
	switch (A_TempUnit)
	{
		case LCD_TEMP_FAHRENHEIT:
		Local_f32DisplayedTemperature =
		(A_f32TemperatureCelsius * 9.0f / 5.0f) + 32.0f;
		Local_u8UnitCharacter = 'F';
		break;

		case LCD_TEMP_KELVIN:
		Local_f32DisplayedTemperature =
		A_f32TemperatureCelsius + 273.15f;
		Local_u8UnitCharacter = 'K';
		break;

		case LCD_TEMP_CELSIUS:
		default:

		Local_f32DisplayedTemperature =
		A_f32TemperatureCelsius;

		Local_u8UnitCharacter = 'C';
		break;
	}

	/* Line 1: title */
	HLCD_voidGoToPos(L1, C1);
	HLCD_voidSendString((u8 *)" ENVIRONMENT MONITOR ");

	/* Line 2: temperature */
	HLCD_voidGoToPos(L2, C1);
	HLCD_voidSendString((u8 *)"Temperature: ");

	HLCD_voidDisplayFloatOneDigit(Local_f32DisplayedTemperature);

	HLCD_voidSendData(' ');
	HLCD_voidSendData(Local_u8UnitCharacter);

	/* Clear remaining places in line 2 */
	HLCD_voidSendString((u8 *)"    ");

	/* Line 3: humidity */
	HLCD_voidGoToPos(L3, C1);
	HLCD_voidSendString((u8 *)"Humidity:    ");

	HLCD_voidDisplayFloatOneDigit(A_f32Humidity);

	HLCD_voidSendString((u8 *)" %    ");

	/* Line 4: empty */
	HLCD_voidGoToPos(L4, C1);
	HLCD_voidSendString((u8 *)"                    ");
}

void HLCD_voidDisplayPomodoro(u8 A_u8WorkMinutes,u8 A_u8WorkSeconds,u8 A_u8BreakMinutes,u8 A_u8BreakSeconds,LCD_POMODORO_STATUS A_Status)
{
	/* Validate seconds */
	if (A_u8WorkSeconds > 59)
	{
		A_u8WorkSeconds = 59;
	}

	if (A_u8BreakSeconds > 59)
	{
		A_u8BreakSeconds = 59;
	}

	/* Line 1: title */
	HLCD_voidGoToPos(L1, C1);
	HLCD_voidSendString((u8 *)" POMODORO TECHNIQUE");

	/* Line 2: work time */
	HLCD_voidGoToPos(L2, C1);
	HLCD_voidSendString((u8 *)"Work Time:  ");

	HLCD_voidDisplayTwoDigits(A_u8WorkMinutes);
	HLCD_voidSendData(':');
	HLCD_voidDisplayTwoDigits(A_u8WorkSeconds);

	HLCD_voidSendString((u8 *)"    ");

	/* Line 3: break time */
	HLCD_voidGoToPos(L3, C1);
	HLCD_voidSendString((u8 *)"Break Time: ");

	HLCD_voidDisplayTwoDigits(A_u8BreakMinutes);
	HLCD_voidSendData(':');
	HLCD_voidDisplayTwoDigits(A_u8BreakSeconds);

	HLCD_voidSendString((u8 *)"    ");

	/* Line 4: status */
	HLCD_voidGoToPos(L4, C1);

	if (A_Status == LCD_POMODORO_ON)
	{
		HLCD_voidSendString((u8 *)"Status: ON          ");
	}
	else
	{
		HLCD_voidSendString((u8 *)"Status: OFF         ");
	}
}

void HLCD_voidDisplayStopwatch(u8 A_u8Hours,u8 A_u8Minutes,u8 A_u8Seconds,LCD_STOPWATCH_STATUS A_Status){
	/* Validate time */
	if (A_u8Hours > 99)
	{
		A_u8Hours = 99;
	}

	if (A_u8Minutes > 59)
	{
		A_u8Minutes = 59;
	}

	if (A_u8Seconds > 59)
	{
		A_u8Seconds = 59;
	}

	/* Line 1: title */
	HLCD_voidGoToPos(L1, C1);
	HLCD_voidSendString((u8 *)"     STOPWATCH      ");

	/* Line 2: stopwatch time */
	HLCD_voidGoToPos(L2, C1);
	HLCD_voidSendString((u8 *)"Time: ");

	HLCD_voidDisplayTwoDigits(A_u8Hours);
	HLCD_voidSendData(':');

	HLCD_voidDisplayTwoDigits(A_u8Minutes);
	HLCD_voidSendData(':');

	HLCD_voidDisplayTwoDigits(A_u8Seconds);

	HLCD_voidSendString((u8 *)"      ");

	/* Line 3: status */
	HLCD_voidGoToPos(L3, C1);

	if (A_Status == LCD_STOPWATCH_ON)
	{
		HLCD_voidSendString((u8 *)"Status: ON          ");
	}
	else
	{
		HLCD_voidSendString((u8 *)"Status: OFF         ");
	}

	/* Line 4: empty */
	HLCD_voidGoToPos(L4, C1);
	HLCD_voidSendString((u8 *)"                    ");
}



