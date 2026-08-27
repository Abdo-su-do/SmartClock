#include "../INCLUDE/INCLUDES.h"
#include "../INCLUDE/TIMER.h"
#include "../INCLUDE/SET.h"
// Global Var
s8 G_u8ScreenCounter = 0;
s8 G_u8TempScreenCounter = 0;
s8 G_u8SetScreenCounter =0;
s8 G_u8TempSetScreenCounter = 0;

LCD_TEMP_UNIT G_tempUnit = LCD_TEMP_CELSIUS;

u8 G_u8UserPresseKey = 'N';
u16 G_u16TempRaw = 0;
u16 G_u16HumidRaw = 0;

#define KEY_OK '/'
#define KEY_BACK '='
#define KEY_NEXT '+'
#define KEY_PREV '-'
#define KEY_SETT '*'


#define TIME_DATE_SCREEN   0
#define TEMP_HUMID_SCREEN  1
#define TIMER_SCREEN       2
#define ALARM_SCREEN       3
#define STOPWATCH_SCREEN   4
#define POMODORRO_SCREEN   5
#define SETTINGS_SCREEN    6

#define MAX_SCREENS 2
#define SET_MAX_SCREENS 1

int main(void)
{
	MDIO_voidInit();
	HLCD_voidInit();
	MI2C_voidInit();
	HRTC_voidInit();
	HKEPAD_voidInit();
	MADC_voidInit();
	TIMER_APP_voidInit();
	MGI_voidEnable();


	RTC_DATE_TIME set_time;
	set_time.Hours = 22;
	set_time.Minutes = 59;
	set_time.Seconds = 50;
	set_time.ClockMode = RTC_MODE_24_HOUR;
	set_time.Period = RTC_AM; /* Ignored in 24h mode */
	set_time.Day = RTC_SUNDAY;
	set_time.Date = 31;
	set_time.Month = 12;
	set_time.Year = 2026;


	/* Write time to the RTC */
	HRTC_voidSetDateTime(&set_time);
	HLCD_voidClearDisplay();
	HLCD_voidGoToPos(L1, C1);
	HLCD_voidSendString((u8*)" {v1.0} ");
	_delay_ms(1000);
	HLCD_voidClearDisplay();
	while (1)
	{
		// Reading time from RTC
		RTC_DATE_TIME current_time;
		HRTC_voidGetDateTime(&current_time);
		G_u8UserPresseKey = HKEYPAD_u8GetPressedKey();
		switch (G_u8UserPresseKey)
		{
			case KEY_NEXT:
			if(G_u8SetScreenCounter != SET_TIME_MODE && G_u8SetScreenCounter != SET_TIME_VALUE && G_u8SetScreenCounter != SET_DATE_VALUE)
			{
				if(G_u8ScreenCounter != SETTINGS_SCREEN)
				{
					G_u8ScreenCounter++;
					if (G_u8ScreenCounter > MAX_SCREENS)
					{
						G_u8ScreenCounter = 0;
					}
				}
				else
				{
					G_u8SetScreenCounter++;
					if (G_u8SetScreenCounter > SET_MAX_SCREENS)
					{
						G_u8SetScreenCounter = 0;
					}
				}
			}

			break;
			case KEY_PREV:
			if(G_u8SetScreenCounter != SET_TIME_MODE && G_u8SetScreenCounter != SET_TIME_VALUE && G_u8SetScreenCounter != SET_DATE_VALUE)
			{
				if(G_u8ScreenCounter != SETTINGS_SCREEN)
				{
					G_u8ScreenCounter--;
					if (G_u8ScreenCounter < 0)
					{
						G_u8ScreenCounter = MAX_SCREENS;
					}
				}
				else
				{
					G_u8SetScreenCounter--;
					if (G_u8SetScreenCounter < 0)
					{
						G_u8SetScreenCounter = SET_MAX_SCREENS;
					}
				}
			}
			break;
			case KEY_SETT:
			if(G_u8ScreenCounter != SETTINGS_SCREEN)
			{
				G_u8TempScreenCounter = G_u8ScreenCounter;
				G_u8ScreenCounter = SETTINGS_SCREEN;
			}
			else
			{
				G_u8ScreenCounter = G_u8TempScreenCounter;
			}
			break;
			case '0' : break;
			case '1' :
				 if (G_u8ScreenCounter == SETTINGS_SCREEN)
				 {
					 if (G_u8SetScreenCounter == SET_SCREEN1)
					 {
						 /* Enter the time-mode setting screen */
						 G_u8SetScreenCounter = SET_TIME_MODE;
					 }
					 else if (G_u8SetScreenCounter == SET_TIME_MODE)
					 {
						 /* Select 24-hour mode and return */
						 HRTC_voidSetClockMode(RTC_MODE_24_HOUR);
						 G_u8SetScreenCounter = SET_SCREEN1;
					 }
					 else if (G_u8SetScreenCounter == SET_TEMP_UNIT)
					 {
						 G_tempUnit = LCD_TEMP_CELSIUS;
						 G_u8SetScreenCounter = SET_SCREEN2;
					 }

				 }
			break;
			case '2' :
			 if (G_u8ScreenCounter == SETTINGS_SCREEN)
			 {
				 if (G_u8SetScreenCounter == SET_SCREEN1)
				 {
					 /* Enter the time-mode setting screen */
					 G_u8SetScreenCounter = SET_TIME_VALUE;
					 G_u8UserPresseKey = '0';
				 }
				 else if (G_u8SetScreenCounter == SET_TIME_MODE)
				 {
					 /* Select 24-hour mode and return */
					 HRTC_voidSetClockMode(RTC_MODE_12_HOUR);
					 G_u8SetScreenCounter = SET_SCREEN1;
				 }
				 else if (G_u8SetScreenCounter == SET_TEMP_UNIT)
				 {
					 G_tempUnit = LCD_TEMP_FAHRENHEIT;
					 G_u8SetScreenCounter = SET_SCREEN2;
				 }
			 }
			break;
			case '3' :
				 if (G_u8ScreenCounter == SETTINGS_SCREEN)
					 {
					 if (G_u8SetScreenCounter == SET_SCREEN1)
					 {
						 /* Enter the time-mode setting screen */
						 G_u8SetScreenCounter = SET_DATE_VALUE;
						 G_u8UserPresseKey = '0';
					 }
					 else if (G_u8SetScreenCounter == SET_TEMP_UNIT)
					 {
						 G_tempUnit = LCD_TEMP_KELVIN;
						 G_u8SetScreenCounter = SET_SCREEN2;
					 }
				 }

				break;
			case '4' :
				if ((G_u8ScreenCounter == SETTINGS_SCREEN) &&
				        (G_u8SetScreenCounter == SET_SCREEN2))
				    {
				        G_u8SetScreenCounter = SET_TEMP_UNIT;
				    }
				break;
			case '5' : break;
			case '6' : break;
			case '7' : break;
			case '8' : break;
			case '9' : break;


			default:
			break;
		}
		switch (G_u8ScreenCounter)
		{
			case TIME_DATE_SCREEN:
			HLCD_voidDisplayDateTime(current_time.Hours, current_time.Minutes,
			current_time.Seconds, current_time.Period, current_time.Day,
			current_time.Date, current_time.Month, current_time.Year,
			current_time.ClockMode);
			break;

			case TEMP_HUMID_SCREEN:
			G_u16TempRaw = MADC_u16GetDigitalValue(ADC0);
			G_u16HumidRaw = MADC_u16GetDigitalValue(ADC1);
			f32 L_f32Temp = ((f32)G_u16TempRaw * 500.0f) / 1024.0f;
			f32 L_f32Humid = ((f32)G_u16HumidRaw * 100.0f) / 1023.0f;
			HLCD_voidDisplayTempHumidity(L_f32Temp, L_f32Humid, G_tempUnit);
			break;
			case TIMER_SCREEN:
			TIMER_APP_voidUpdate(G_u8UserPresseKey);
			break;
			case ALARM_SCREEN:
			break;
			case STOPWATCH_SCREEN:
			break;
			case POMODORRO_SCREEN:
			break;
			case SETTINGS_SCREEN :
			switch(G_u8SetScreenCounter)
			{
				case SET_SCREEN1 : ASET_voidSetScreen1(); break;
				case SET_SCREEN2 : ASET_voidSetScreen2(); break;
				//case SET_SCREEN3 : ASET_voidSetScreen3(); break;
			    case SET_TIME_MODE  : ASET_voidSetScreenTimeMode();    break;
			    case SET_TIME_VALUE : ASET_voidSetScreenTimeValue(G_u8UserPresseKey,&G_u8SetScreenCounter,current_time.ClockMode);break;
			    case SET_DATE_VALUE : ASET_voidSetScreenDateValue(G_u8UserPresseKey,&G_u8SetScreenCounter);break;
			    case SET_TEMP_UNIT : ASET_voidSetScreenTempUnit();  break;
				default : break;
			}

			break;
			default:
			break;
		}
	}
}
