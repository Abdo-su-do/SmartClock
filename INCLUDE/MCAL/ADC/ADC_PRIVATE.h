#ifndef INCLUDE_MCAL_ADC_ADC_PRIVATE_H_
#define INCLUDE_MCAL_ADC_ADC_PRIVATE_H_


/* ADC Registers */
#define ADMUX    (*(volatile u8 *)0x27)
#define ADCSRA   (*(volatile u8 *)0x26)
#define ADCH     (*(volatile u8 *)0x25)
#define ADCL     (*(volatile u8 *)0x24)

#define SFIOR    (*(volatile u8 *)0x50)


/* ADMUX Register Bits */
#define ADC_REF_SELECT_BIT1       7
#define ADC_REF_SELECT_BIT0       6
#define ADC_LEFT_ADJUST_BIT       5


/* ADCSRA Register Bits */
#define ADC_ENABLE_BIT            7
#define ADC_START_CONVERSION_BIT  6
#define ADC_AUTO_TRIGGER_BIT      5
#define ADC_INTERRUPT_FLAG_BIT    4
#define ADC_INTERRUPT_ENABLE_BIT  3

#define ADC_PRESCALER_BIT2        2
#define ADC_PRESCALER_BIT1        1
#define ADC_PRESCALER_BIT0        0


/* ADC MUX */
#define ADC_CHANNEL_MASK          0x1F
#define ADC_MUX_KEEP_MASK         0xE0


#endif /* INCLUDE_MCAL_ADC_ADC_PRIVATE_H_ */
