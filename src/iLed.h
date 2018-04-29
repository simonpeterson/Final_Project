#ifndef ILED_H
#define	ILED_H

#ifdef	__cplusplus
extern "C" {
#endif

    void iLed_init(void);
    void change_color(unsigned long int);
    void green(void);
    void yellow(void);
    void red(void);

#ifdef	__cplusplus
}
#endif

#endif	/* ILED_H */

