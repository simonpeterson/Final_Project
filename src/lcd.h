#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

    void lcd_cmd(char);
    void lcd_init(void);
    void lcd_printStr(const char*);
    void lcd_printChar(char);
    void lcd_setCursor(char, char);

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

