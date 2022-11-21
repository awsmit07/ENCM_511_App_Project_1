/*
 * File: io.h
 * Author: Andy Smit
 * Comments:
 * Revision history:
 */

// This is a guard condition so that contents of this file are not included
// more than once.
#ifndef IO_H
#define	IO_H

#include <xc.h> // include processor files - each processor file is guarded.

typedef enum
{
    NO_BUTTONS =0,
    BUTTON1 = 1,
    BUTTON2 = 2,
    BUTTON3 = 4,
    BUTTON1_LONG = 5,
    BUTTON2_LONG = 6,
    BUTTON3_LONG = 7,
} button_t;

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

// Set up the IO configuration
void IO_init();

void IO_handle_button_event();

void IO_set_button_callback();

void LED_toggle();

void LED_on();

void LED_off();

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* IO_H */

