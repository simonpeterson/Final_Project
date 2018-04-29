#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

    void adc_init(void);
    void __attribute__ ((__interrupt__, __auto_psv__)) _ADC1Interrupt(void);
    void __attribute__ ((__interrupt__,__auto_psv__)) _INT0Interrupt(void);
    void __attribute__ ((__interrupt__,__auto_psv__)) _T4Interrupt(void);

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

