/*
 * File: timer.h
 * Author: Andy Smit
 * Comments:
 * Revision history:
 */

// This is a guard condition so that contents of this file are not included
// more than once.
#ifndef TIMER_H
#define	TIMER_H

#include <xc.h> // include processor files - each processor file is guarded.

typedef enum
{
    TIMER1,
    TIMER2,
    TIMER3
} timer_t;

typedef struct
{
    uint8_t timer1_flag:1;
    uint8_t timer2_flag:1;
    uint8_t timer3_flag:1;
} timer_flags_t;
extern timer_flags_t timer_flags;


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

void timer_init(void);

void timer_start(timer_t timer, uint16_t delay_ms);

void timer_stop(timer_t timer);

void delay_ms(uint16_t time_ms);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* TIMER_H */

