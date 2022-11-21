/*
 * File:   app.c
 * Author: andy
 *
 * Created on November 18, 2022, 9:39 AM
 */
#include <xc.h>
#include <stdio.h>
#include "main.h"
#include "timer.h"
#include "app.h"
#include "io.h"
#include "UART2.h"


// Private function prototypes
void app_state_enter_time_main(void);
void app_state_countdown_main(void);
void app_state_timer_finish_main(void);

void app_state_countdown_init(void);
void app_state_timer_finish_init(void);

void app_enter_time_button_press(button_t buttons);
void app_countdown_button_press(button_t buttons);
void app_timer_finish_button_press(button_t buttons);
void app_display_time(void);
void app_clear_term_line(void);


// Private variables
static uint16_t countdown_s = 0;

// Public Functions
/*
 * APP_state_enter_time_init
 *
 * Initialize STATE_ENTER_TIME. Sets appropriate button callback, turns
 * off LED, clears display.
 *
 * @param None
 * @returns None
 */
void APP_state_enter_time_init(void)
{
    app_state = STATE_ENTER_TIME;
    IO_set_button_callback(&app_enter_time_button_press);
    LED_off();
    app_clear_term_line();
    app_display_time();
}


/*
 * APP_state_machine_main
 *
 * Main function to be run every main loop for the state machine. Calls
 * current states main functions.
 *
 * @param None
 * @returns None
 */
void APP_state_machine_main(void)
{
    switch (app_state)
    {
        case STATE_ENTER_TIME:
        {
            app_state_enter_time_main();
            break;
        }
        case STATE_COUNTDOWN:
        {
            app_state_countdown_main();
            break;
        }
        case STATE_TIMER_FINISH:
        {
            app_state_timer_finish_main();
            break;
        }
        default:
            break;
    }
}


// Private functions

/*
 * app_state_enter_time_main
 *
 * Main function for STATE_ENTER_TIME
*/
void app_state_enter_time_main(void)
{
    app_display_time();
}


/*
 * app_state_countdown_main
 *
 * Main function for STATE_COUNTDOWN
 */
void app_state_countdown_main(void)
{
    // When the timer trigers, decrement countdown, update display,
    // toggle LED
    if(interrupt_state.timer1_trig)
    {
        interrupt_state.timer1_trig = 0;
        countdown_s--;
        app_display_time();
        LED_toggle();

        if (countdown_s == 0)
        {
            app_state_timer_finish_init();
        }
    }
}

/*
 * app_state_timer_finish_main
 *
 * Main function for STATE_TIMER_FINISH
 */
void app_state_timer_finish_main(void)
{

}

/*
 * app_state_countdown_init
 *
 * Initialize STATE_COUNTDOWN. Start a 1000ms timer to keep track of the
 * countdown and sets appropriate button callback.
 *
 * @param None
 * @returns None
 */
void app_state_countdown_init(void)
{
    app_state = STATE_COUNTDOWN;
    IO_set_button_callback(&app_countdown_button_press);
    // Start a 1000ms timeSet a callback to be used when a button is pressed.
    timer_start(TIMER1, 1000);
}


/*
 * app_state_countdown_init
 *
 * Initialize STATE_TIMER_FINISH. Stops the countdown timer, displays
 * the alarm text, and sets appropriate button callback. Turns LED on.
 *
 * @param None
 * @returns None
 */
void app_state_timer_finish_init(void)
{
    app_state = STATE_TIMER_FINISH;
    timer_stop(TIMER1);
    Disp2String(" -- ALARM");
    IO_set_button_callback(&app_timer_finish_button_press);
    LED_on();
}


/*
 * app_enter_time_button_press
 *
 * Button callback function for STATE_ENTER_TIME.
 *
 * @param buttons The button being pressed.
 */
void app_enter_time_button_press(button_t buttons)
{
    switch (buttons)
    {
        // Button 1 increment minutes
        case BUTTON1:
        {
            countdown_s += 60;
            // If the countdown timer goes above 59 minutes reset
            // minutes to 0.
            if ((countdown_s / 60) > 59)
            {
                countdown_s %= 60;
            }
            app_display_time();
            break;
        }
        // Button 2 increment seconds
        case BUTTON2:
        {
            countdown_s ++;
            app_display_time();
            break;
        }
        // Short button 3 start countdown
        case BUTTON3:
        {
            if(countdown_s != 0)
            {
                app_state_countdown_init();
            }
            break;
        }
        // Long button reset countdown
        case BUTTON3_LONG:
        {
            countdown_s = 0;
            break;
        }
        default:
            break;
    }
}


/*
 * app_countdown_button_press
 *
 * Button callback function for STATE_COUNTDOWN.
 *
 * @param buttons The button being pressed.
 */
void app_countdown_button_press(button_t buttons)
{
    switch (buttons)
    {
        case BUTTON3:
        {
            // If PB3 is pressed, pause the timer until PB3 is pressed
            // again
            if(timer_flags.timer1_flag)
            {
                timer_stop(TIMER1);
            }
            else
            {
                timer_start(TIMER1, 1000);
            }
            break;
        }
        case BUTTON3_LONG:
        {
            countdown_s = 0;
            APP_state_enter_time_init();
            break;
        }
        default:
            break;
    }
}


/*
 * app_timer_finish_button_press
 *
 * Button callback function for STATE_TIMER_FINISH.
 *
 * @param buttons The button being pressed.
 */
void app_timer_finish_button_press(button_t buttons)
{
    APP_state_enter_time_init();
}


/*
 * app_display_time
 *
 * Display the current time remaining in the countdown to the UART
 * terminal
 *
 * @param none
 * @returns none
 */
void app_display_time()
{
    // Convert the current countdown value to minutes and seconds
    uint8_t minutes = countdown_s / 60;
    uint8_t seconds = countdown_s % 60;

    // Create a string with the countdown value displayed as specified
    // in the project document
    char time_display[10];
    Disp2String("\r");
    snprintf(time_display, 10, "%02dm:%02ds", minutes, seconds);
    debug_hex(countdown_s);

    // Use UART interface to display the time
    Disp2String(time_display);
}


/*
 * app_clear_term_line
 *
 * Clear a line on the UART console. Assumes a standard 80 character
 * terminal width.
 *
 * @param none
 * @returns none
 */
void app_clear_term_line()
{
    // Create an 80 character string of spaces to use to clear the line.
    char tmp[80] = {0};
    int i;
    for(i = 0; i < 79; i++)
    {
        tmp[i] = ' ';
    }

    // Use the UART interface to write the line and move the cursor.
    Disp2String("\r");
    Disp2String(tmp);
    Disp2String("\r");
}
