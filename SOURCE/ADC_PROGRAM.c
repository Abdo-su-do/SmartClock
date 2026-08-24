#include "../INCLUDE/LIB/STD_TYPES.h"
#include "../INCLUDE/LIB/BIT_MATH.h"

#include "../INCLUDE/MCAL/ADC/ADC_CFG.h"
#include "../INCLUDE/MCAL/ADC/ADC_PRIVATE.h"
#include "../INCLUDE/MCAL/ADC/ADC_INTERFACE.h"


void MADC_voidInit(void)
{
    /* Select AVCC as reference */
    CLR_BIT(ADMUX, ADC_REF_SELECT_BIT1);
    SET_BIT(ADMUX, ADC_REF_SELECT_BIT0);

    /* Right adjustment */
    CLR_BIT(ADMUX, ADC_LEFT_ADJUST_BIT);

    /* Prescaler = 128 */
    SET_BIT(ADCSRA, ADC_PRESCALER_BIT2);
    SET_BIT(ADCSRA, ADC_PRESCALER_BIT1);
    SET_BIT(ADCSRA, ADC_PRESCALER_BIT0);

    /* Enable ADC */
    SET_BIT(ADCSRA, ADC_ENABLE_BIT);
}


u16 MADC_u16GetDigitalValue(ADC_CHANNELS A_ADCChannel)
{
    u16 Local_u16ADCValue;

    /* Select channel */
    ADMUX &= ADC_MUX_KEEP_MASK;
    ADMUX |= (A_ADCChannel & ADC_CHANNEL_MASK);

    /* Start conversion */
    SET_BIT(ADCSRA, ADC_START_CONVERSION_BIT);

    /* Wait for conversion */
    while (GET_BIT(ADCSRA, ADC_INTERRUPT_FLAG_BIT) == 0)
    {
    }

    /* Clear flag */
    SET_BIT(ADCSRA, ADC_INTERRUPT_FLAG_BIT);

    /* Read result */
    Local_u16ADCValue = ADCL;
    Local_u16ADCValue |= ((u16)ADCH << 8);

    return Local_u16ADCValue;
}
