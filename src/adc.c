#include "xc.h"
#include "stdio.h"
#include "adc.h"
#include "iLed.h"
#include "lcd.h"

#define ADC_FREQ 16 // Hz

#define LENGTH 18 // cm

#define BUFF_SIZE 8 // 2^n

#define BUFF_MASK (BUFF_SIZE-1) // Handles Array Index Wraparound

#define RBITSHIFT 3 // Used in Average Calculation = log_2(BUFF_SIZE)

volatile int buff[4][BUFF_SIZE]; // [Array 1] = Sensor, [Array 2] = Data

volatile int B[4] = {-1, -1, -1, -1}; // Index Array for Sensor Buffers

int avg[4] = {0, 0, 0, 0}; // Running Average for Data

int zero[4] = {0, 0, 0, 0}; // Current Zero for Data Calibration

unsigned int tangent[46] = {0,9,18,27,36,45,54,63,72,81,90,100,109,118,128,137,
                            147,157,166,176,186,197,207,217,228,239,250,261,272,
                            284,296,308,320,332,345,359,372,386,400,415,430,445,
                            461,477,494,512}; // Values for 512*tan[angle]
                                              // From 0-45 degrees

void adc_init(void)
{   
    // INT0 for Zero Button
    _TRISB7 = 1;            // Button Input Pin
    _CN23PUE = 1;           // Pulled Up High, Button Press Shorts Pin to Ground
    INTCON2bits.INT0EP = 0; // Interrupt on Falling Edge
    _INT0IF = 0;            // Clear Flag
    _INT0IE = 1;            // Enable Interrupt
    
    // ADC Configuration
    AD1CON1 = 0;                // Clear Register
    AD1CON2 = 0;                // Clear Register
    AD1CON3 = 0;                // Clear Register
    AD1CON1bits.FORM = 0b00;    // Mode: Integer
    AD1CON1bits.SSRC = 0b010;   // Conversion Trigger: Timer 3
    AD1CON1bits.ASAM = 1;       // Auto-Sampling: On
    AD1CON2bits.VCFG = 0b001;   // References: External Vref+ & Vss-
    AD1CON2bits.CSCNA = 1;      // Scanning Mode: On
    AD1CON2bits.SMPI = 0b0111;  // Interrupt After Sample: 8
    AD1CON2bits.BUFM = 1;       // Buffer Mode: Two 8-Word
    AD1CON2bits.ALTS = 0;       // Multiplexer Settings: MUX A
    AD1CON3bits.ADRC = 0;       // Clock Source: System Clock
    AD1CON3bits.ADCS = 1;       // A/D Conversion Clock: 2*Tcy
    
    // Analog Port Designation
    _PCFG12 = 0;    // AN12 - Stretch 2
    _PCFG11 = 0;    // AN11 - Stretch 1
    _PCFG10 = 0;    // AN10 - Flex 2
    _PCFG9  = 0;    // AN9  - Flex 1
    _PCFG0 = 0;     // AN0  - Vref+
    
    // Designate I/O 
    _TRISB12 = 1;   // AN12 - Stretch 2
    _TRISB13 = 1;   // AN11 - Stretch 1
    _TRISB14 = 1;   // AN10 - Flex 2
    _TRISB15 = 1;   // AN9  - Flex 1
    _TRISA0 = 1;    // AN0  - Vref+
    
    // Choose Scanning 
    AD1CSSL = 0;    // Clear Register
    _CSSL12 = 1;    // Stretch 2
    _CSSL11 = 1;    // Stretch 1
    _CSSL10 = 1;    // Flex 2
    _CSSL9  = 1;    // Flex 1
    
    AD1CHS  = 0x0000;   // Unimplemented
    
    // Timer 3 for ADC Conversion
    T3CON = 0;  // Clear Register
    TMR3 = 0;   // Reset Count
    
    // Calculations for Timer Period and Pre-scalar based on System Limitations
    if(ADC_FREQ > 256)
    {
        T3CONbits.TCKPS = 0b00;
        PR3 = (unsigned int)(16000000/ADC_FREQ)-1;
    }
    else if(ADC_FREQ > 32)
    {
        T3CONbits.TCKPS = 0b01;
        PR3 = (unsigned int)(2000000/ADC_FREQ)-1;
    }
    else if(ADC_FREQ > 4)
    {
        T3CONbits.TCKPS = 0b10;
        PR3 = (unsigned int)(250000/ADC_FREQ)-1;
    }
    else
    {
        T3CONbits.TCKPS = 0b11;
        PR3 = (unsigned int)(62500/ADC_FREQ)-1;
    }
    
    // Turning ON the ADC Module and Timer 3 
    _AD1IF = 0;             // Clear ADC Flag
    _AD1IE = 1;             // Enable ADC Interrupt
    AD1CON1bits.ADON = 1;   // Turn ADC Module ON
    _T3IF = 0;              // Clear T3 Flag
    T3CONbits.TON = 1;      // Turn Timer 3 ON
}

void __attribute__ ((__interrupt__, __auto_psv__)) _ADC1Interrupt(void)
{
    _AD1IF = 0; // Clear ADC Flag
    
    if(_BUFS) // BUFS = 1 if ADC is filling BUF[8:F], read data in BUF[0:7]
    {
        B[0] = (B[0]+1)&BUFF_MASK;
        buff[0][B[0]] = ADC1BUF0;
        
        B[1] = (B[1]+1)&BUFF_MASK;
        buff[1][B[1]] = ADC1BUF1;
        
        B[2] = (B[2]+1)&BUFF_MASK;
        buff[2][B[2]] = ADC1BUF2;
        
        B[3] = (B[3]+1)&BUFF_MASK;
        buff[3][B[3]] = ADC1BUF3;
        
        B[0] = (B[0]+1)&BUFF_MASK;
        buff[0][B[0]] = ADC1BUF4;
        
        B[1] = (B[1]+1)&BUFF_MASK;
        buff[1][B[1]] = ADC1BUF5;
        
        B[2] = (B[2]+1)&BUFF_MASK;
        buff[2][B[2]] = ADC1BUF6;
        
        B[3] = (B[3]+1)&BUFF_MASK;
        buff[3][B[3]] = ADC1BUF7;
    }
    else // BUFS = 0 if ADC is filling BUF[0:7], read data in BUF[8:F]
    {
        B[0] = (B[0]+1)&BUFF_MASK;
        buff[0][B[0]] = ADC1BUF8;
        
        B[1] = (B[1]+1)&BUFF_MASK;
        buff[1][B[1]] = ADC1BUF9;
        
        B[2] = (B[2]+1)&BUFF_MASK;
        buff[2][B[2]] = ADC1BUFA;
        
        B[3] = (B[3]+1)&BUFF_MASK;
        buff[3][B[3]] = ADC1BUFB;
        
        B[0] = (B[0]+1)&BUFF_MASK;
        buff[0][B[0]] = ADC1BUFC;
        
        B[1] = (B[1]+1)&BUFF_MASK;
        buff[1][B[1]] = ADC1BUFD;
        
        B[2] = (B[2]+1)&BUFF_MASK;
        buff[2][B[2]] = ADC1BUFE;
        
        B[3] = (B[3]+1)&BUFF_MASK;
        buff[3][B[3]] = ADC1BUFF;
    }
}

void __attribute__ ((__interrupt__,__auto_psv__)) _INT0Interrupt(void)
{
    _INT0IF = 0; // Clear INT0 Flag
    zero[0] = avg[0];   // 
    zero[1] = avg[1];   // 
    zero[2] = avg[2];   // 
    zero[3] = avg[3];   // 
}

void __attribute__ ((__interrupt__,__auto_psv__)) _T4Interrupt(void)
{
    _T4IF = 0; // Clear Timer 4 Flag

    long int val[4] = {0, 0, 0, 0}; // Used to Calculate the Average
                                    // of ADC Buffers, (long) handles overflow
    unsigned int i; // Iterator Variable
    for(i=0;i<BUFF_SIZE;i++)
    {
        val[0] += buff[0][i]; // Sum the ADC Buffers
        val[1] += buff[1][i]; 
        val[2] += buff[2][i]; 
        val[3] += buff[3][i]; 
    }
    
    val[0] >>= RBITSHIFT; // Equivalent of Division
    val[1] >>= RBITSHIFT;
    val[2] >>= RBITSHIFT;
    val[3] >>= RBITSHIFT;
    
    avg[0] = (int) val[0]; // Transfer Averages to AVG Array
    avg[1] = (int) val[1];
    avg[2] = (int) val[2];
    avg[3] = (int) val[3];
    
    int diff[4]; // Variable to hold the difference value of buffer and zero
    diff[0] = avg[0] - zero[0];
    diff[1] = avg[1] - zero[1];
    diff[2] = avg[2] - zero[2];
    diff[3] = avg[3] - zero[3];
    
    int stretch = (diff[2]+diff[3]) >> 1; // Averages the two stretch sensors
    int s_zero = (zero[2]+zero[3]) >> 1;  // Averages the zero for the stretch
    int percent = (100*stretch)/(600-s_zero); // Calculates percent stretch
                                              // 600 is a "tested calibration"
    
    float A = 1.9*diff[0]; // Linear approximation of angle A, 1.9 is test value
    float B = 0.6*diff[1]; // Linear approximation of angle B, 0.6 is test value
    
    double location; // X variable
    double depth;    // Y variable
    double other_side; // Equivalent of LENGTH-X, used for depth averaging
    
    if((A==0) || (B==0)) // Avoid dividing by zero.
    {
        location = 0;
        other_side = 0;
        depth = (location*tangent[(int)A]+(other_side)*tangent[(int)B])/2048; 
    }
    else if((A>45)||(B>45)) // Potentially output MAX/ERROR onto LCD?
    {                       // Don't have values for angle > 45
        location = 0;       // Also the bridge should be in the red for this
        other_side = 0;
        depth = 0;
    }
    else // Compute X and Y based on known formulas 
    {
        location = LENGTH*(1.0*tangent[(int)A]/(1.0*(tangent[(int)A]+tangent[(int)B])));
        other_side = LENGTH*(1.0*tangent[(int)B]/(1.0*(tangent[(int)A]+tangent[(int)B])));
        depth = (location*tangent[(int)A]+(other_side)*tangent[(int)B])/2048; 
    }
    
    char line[2][20]; // two lines to be printed onto LCD
                      // Maybe add other ones for no load and exceeding load?
    sprintf(line[0], "X: %4.2f ", location); // generate X: string
    sprintf(line[1], "Y: %4.2f ", depth);    // generate Y: string
    
    if(percent < 10) // If bridge stretch is under 10%
    {
        green();        // Turn iLED to Green
        _LATA2 = 1;     // Green LED ON
        _LATA4 = 0;     // Yellow LED OFF
        _LATB4 = 0;     // Red LED OFF
    }
    else if (percent < 20) // If bridge stretch is between 10-20%
    {
        yellow();       // Turn iLED to Yellow
        _LATA2 = 0;     // Green LED OFF
        _LATB4 = 1;     // Yellow LED ON
        _LATA4 = 0;     // Red LED OFF
    }
    else // Bridge is over 20% stretch
    {
        red();          // Turn iLED to Red
        _LATB4 = 0;     // Turn Green LED OFF
        _LATA2 = 0;     // Turn Yellow LED OFF
        _LATA4 = 1;     // Turn Red LED ON
    }

    lcd_cmd(0b00000001);       // Clear Display
    lcd_setCursor(0,0);        // Reset Cursor
    lcd_printStr(&line[0][0]); // Print Line 1
    lcd_setCursor(0,1);        // Return Cursor
    lcd_printStr(&line[1][0]); // Print Line 2    
}