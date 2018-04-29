#include "xc.h"
#include "iLed.h"
#include "iLedASM.h"

void iLed_init(void)
{
    _TRISB6 = 0;
    _LATB6 = 0;
}

void change_color(unsigned long int color)
{
    unsigned long mask = 0x800000;
    while(mask > 0)
    {
        if(mask&color)
            write_1();
        else
            write_0();
        mask = mask>>1;
    }
}

void green(void)
{
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    
    _LATB6 = 0;
}

void yellow(void)
{
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
        
    _LATB6 = 0;
}

void red(void)
{
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_1();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    write_0();
    
    _LATB6 = 0;
}

