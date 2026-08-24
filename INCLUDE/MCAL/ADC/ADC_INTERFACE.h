#ifndef INCLUDE_MCAL_ADC_ADC_INTERFACE_H_
#define INCLUDE_MCAL_ADC_ADC_INTERFACE_H_


/* ADC Input Channels */
typedef enum
{
    /* Single Ended */
    ADC0 = 0,
    ADC1,
    ADC2,
    ADC3,
    ADC4,
    ADC5,
    ADC6,
    ADC7,

    /* Differential Inputs */
    ADC_0_0_10,
    ADC_1_0_10,
    ADC_0_0_200,
    ADC_1_0_200,

    ADC_2_2_10,
    ADC_3_2_10,
    ADC_2_2_200,
    ADC_3_2_200,

    ADC_0_1_1,
    ADC_1_1_1,
    ADC_2_1_1,
    ADC_3_1_1,
    ADC_4_1_1,
    ADC_5_1_1,
    ADC_6_1_1,
    ADC_7_1_1,

    ADC_0_2_1,
    ADC_1_2_1,
    ADC_2_2_1,
    ADC_3_2_1,
    ADC_4_2_1

} ADC_CHANNELS;


/* ADC Functions */
void MADC_voidInit(void);

u16 MADC_u16GetDigitalValue(ADC_CHANNELS A_ADCChannel);


#endif /* INCLUDE_MCAL_ADC_ADC_INTERFACE_H_ */
