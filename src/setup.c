#include "xc.h"
#include "setup.h"

void setup(void)
{
    _RCDIV = 0;         // System Frequency: 16 MHZ
    
    AD1PCFG = 0x9FFF;   // Set All Pins to Digital
    
    TRISA = 0x0000;     // Designate I/O for PORTA
    LATA  = 0x0000;     // Clear LATA
    
    TRISB = 0x0003;     // Designate I/O for PORTB
    LATB  = 0x0000;     // Clear LATB
}