#include "xc.h"
#include "setup.h"
#include "lcd.h"
#include "adc.h"
#include "iLed.h"
#include "iLedASM.h"

#pragma config ICS = PGx1 
#pragma config FWDTEN = OFF
#pragma config GWRP = OFF
#pragma config GCP = OFF
#pragma config JTAGEN = OFF
#pragma config I2C1SEL = PRI
#pragma config IOL1WAY = OFF
#pragma config OSCIOFNC = ON
#pragma config FCKSM = CSECME
#pragma config FNOSC = FRCPLL

int main(void) 
{    
    setup();
    // RCDIV & Frequency
    // PORTA/PORTB Config
    
    iLed_init();
    // RB6 for Data Output
    
    adc_init();
    // ADC Configuration Registers
    // Input Designation
    // Timer 3 for Sample/Conversion
    // Callibration Button
    
    lcd_init();
    // Turn ON and Setup LCD
    // Timter 4 for LCD refresh

    while(1);
    
    return 0;
}