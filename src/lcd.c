#include "xc.h"
#include "lcd.h"

#define LCD_FREQ 5 // Hz

void lcd_cmd(char command) 
{    
    I2C2CONbits.SEN = 1;    // Start Trasmission
    while(I2C2CONbits.SEN);
    
    I2C2TRN = 0b01111100;   // Slave Address
    while(I2C2STATbits.TRSTAT);
    
    I2C2TRN = 0b00000000; // CO & RS bits
    while(I2C2STATbits.TRSTAT);
    
    I2C2TRN = command;      // Package
    while(I2C2STATbits.TRSTAT);
    
    I2C2CONbits.PEN = 1;    // End Transmission
    while(I2C2CONbits.PEN);
    
    unsigned long int cycles = 0;
    for(cycles = 0; cycles < 2; cycles++); // 2.375us
}

void lcd_init(void) 
{    
    I2C2CONbits.I2CEN = 0;  // Disables I2C2 module for modification
    I2C2BRG = 0x9D;         // Sets baud rate to 16 kHz
    I2C2CONbits.I2CEN = 1;  // Re-enables I2C2 module
    
    char contrast = 0b100000;
    
    char contrast_1 = 0b01110000 | contrast;
    char contrast_2 = 0b01010100 | (contrast >> 4);
    
    unsigned long int cycles;
    for(cycles = 0; cycles < 66666; cycles++);  // 50ms
    
    lcd_cmd(0b00111000);      // Function Set
    lcd_cmd(0b00111001);      // Function Set
    lcd_cmd(0b00010100);      // Internal OSC Freq
    
    lcd_cmd(contrast_1);      // Contrast Set
    lcd_cmd(contrast_2);      // Contrast Set
    
    lcd_cmd(0b01101100);      // Follower Control
    
    for(cycles = 0; cycles < 266666; cycles++); //200ms
    
    lcd_cmd(0b00111000);      // Function Set
    lcd_cmd(0b00001111);      // Display Control
    lcd_cmd(0b00000001);      // Clear Display
        
    T4CON = 0;
    TMR4 = 0;
    if(LCD_FREQ > 256)
    {
        T4CONbits.TCKPS = 0b00;
        PR4 = (unsigned int)(16000000/LCD_FREQ)-1;
    }
    else if(LCD_FREQ > 32)
    {
        T4CONbits.TCKPS = 0b01;
        PR4 = (unsigned int)(2000000/LCD_FREQ)-1;
    }
    else if(LCD_FREQ > 4)
    {
        T4CONbits.TCKPS = 0b10;
        PR4 = (unsigned int)(250000/LCD_FREQ)-1;
    }
    else
    {
        T4CONbits.TCKPS = 0b11;
        PR4 = (unsigned int)(62500/LCD_FREQ)-1;
    }
    
    _T4IF = 0;
    _T4IE = 1;
    T4CONbits.TON = 1;
}

void lcd_printChar(char myChar) 
{    
    I2C2CONbits.SEN = 1;    // Start Transmission
    while(I2C2CONbits.SEN);
    
    I2C2TRN = 0b01111100;   // Slave Address
    while(I2C2STATbits.TRSTAT);
    
    I2C2TRN = 0b01000000; // CO & RS bits
    while(I2C2STATbits.TRSTAT);
    
    I2C2TRN = myChar;      // Package
    while(I2C2STATbits.TRSTAT);
    
    I2C2CONbits.PEN = 1;    // End Transmission
    while(I2C2CONbits.PEN);
    
    unsigned long int cycles;
    for(cycles = 0; cycles < 2; cycles++);	// 2.375us
}

void lcd_printStr(const char *s) 
{
    unsigned int count = 0;
    unsigned int i = 0;
    unsigned int j = 0;
    
    while(*(s+1) != 0)
    {
        count++;
        s++;
    }

    while(i < count)
    {
        s--;
        i++;
    }
    
    I2C2CONbits.SEN = 1;        // Start Transmission
    while(I2C2CONbits.SEN);
    
    I2C2TRN = 0b01111100;       // Slave Address
    while(I2C2STATbits.TRSTAT);
    
    while(j < count) {
        
        I2C2TRN = 0b11000000;   // CO = 1, RS = 1
        while(I2C2STATbits.TRSTAT);

        I2C2TRN = *s;           // Letter in string
        while(I2C2STATbits.TRSTAT);

        s++;
        j++;
    }
    
    I2C2TRN = 0b01000000;   // CO = 0, RS = 1
    while(I2C2STATbits.TRSTAT);

    I2C2TRN = *s;           // Last letter of string
    while(I2C2STATbits.TRSTAT);
    
    I2C2CONbits.PEN = 1;        // End Transmission
    while(I2C2CONbits.PEN);
    
    unsigned long int cycles;
    for(cycles = 0; cycles < 2; cycles++);  // 2.375us
}

void lcd_setCursor(char x, char y) 
{    
    char position = ((0x40 * y) + x) | 0x80;
    lcd_cmd(position);
}