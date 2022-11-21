/*
 * File: main.h
 * Author: Andy Smit
 * Comments:
 * Revision history:
 */

// This is a guard condition so that contents of this file are not included
// more than once.
#ifndef MAIN_H
#define	MAIN_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <p24F16KA101.h>
#include "UART2.h"
#include <stdio.h>

// Configuration macro for different hardware IO setup.
#define ANDY_HARDWARE
#define DEBUG 0

#if DEBUG
#define debug_print(x) do\
{\
    char s[100];\
    snprintf(s, 100, "%s %d: %s\n\r", __func__, __LINE__, x);\
    Disp2String(s);\
} while(0)

#define debug_hex(x)\
do\
{\
    char s[100];\
    snprintf(s, 100, "%s %d: 0x%x\n\r", __func__, __LINE__, x);\
    Disp2String(s);\
} while(0)
#else
#define debug_print(x) do{}while(0)

#define debug_hex(x) do{}while(0)
#endif

typedef enum state
{
    STATE_ENTER_TIME = 0,
    STATE_COUNTDOWN,
    STATE_TIMER_FINISH
} App_State;

// Datatype to hold the current interrupt states
typedef struct
{
        uint8_t cn_trig:1;
        uint8_t timer1_trig:1;
        uint8_t timer2_trig:1;
        uint8_t timer3_trig:1;
        uint8_t :4;
} interrupt_State;

// Make state variable global
extern interrupt_State interrupt_state;
extern App_State app_state;


#endif	/* XC_HEADER_TEMPLATE_H */

