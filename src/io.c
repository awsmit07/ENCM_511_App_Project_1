/*
 * File:   io.c
 * Author: andy
 *
 * Created on October 11, 2022, 12:32 PM
 */

#include <xc.h>
#include "main.h"
#include "io.h"
#include "timer.h"
#include "UART2.h"


typedef void (*button_callback_t)(button_t);
static button_callback_t button_callback = NULL;

/*
 * IO_init
 *
 * Initialize the IO module
 *
 * @param None
 *
 * @return void
 */
void IO_init()
{
    AD1PCFG = 0xFFFF; // Turn all analog pins as digital
#ifndef ANDY_HARDWARE
    // Set input/output
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA4 = 1;
    TRISBbits.TRISB4 = 1;
    TRISBbits.TRISB8 = 0;

    //Set pull-ups for inputs
    CNPU2bits.CN30PUE = 1;
    CNPU1bits.CN0PUE = 1;
    CNPU1bits.CN1PUE = 1;

    //Clock output on REF0
    TRISBbits.TRISB15 = 0;
    REFOCONbits.ROEN = 1;
    REFOCONbits.ROSSLP = 0;
    REFOCONbits.ROSEL = 0;
    REFOCONbits.RODIV = 0b0000;

    // Enable interrupts on input pins
    CNEN2bits.CN30IE = 1;
    CNEN1bits.CN0IE = 1;
    CNEN1bits.CN1IE = 1;
#endif
#ifdef ANDY_HARDWARE
    // Set PORT A<3:0> as inputs and PORT B as output
    TRISA = 0x000f;
    AD1PCFG = 0xDC3F; // Configure all AN pins as digital inputs for use on hardware
    TRISB = 0x0000;

    //Clock output on REF0
    TRISBbits.TRISB15 = 0;
    REFOCONbits.ROEN = 1;
    REFOCONbits.ROSSLP = 0;
    REFOCONbits.ROSEL = 0;
    REFOCONbits.RODIV = 0b0000;

    // Enable interrupts on input pins
    CNPU2bits.CN29PUE = 1;
    CNEN2bits.CN30IE = 1;
    CNEN1bits.CN2IE = 1;
    CNEN1bits.CN3IE = 1;
#endif

    // Set CN interrupt priority to 3.
    IPC4bits.CNIP = 3;

    // Enable the CN interrupt
    IEC1bits.CNIE = 1;

    // Default set the CN interrupt flag to 0
    IFS1bits.CNIF = 0;

    return;
}


/*
 * io_get_buttons
 *
 * Get the current buttons being pressed.
 *
 * @param None
 *
 * @return buttons the button being pressed.
 */
button_t Io_get_buttons()
{
    // Bit field for button state
    union Button_state
    {
        button_t state;
        struct
        {
            uint8_t s0:1;
            uint8_t s1:1;
            uint8_t s2:1;
        };
    } buttons = {0};
    #ifdef ANDY_HARDWARE
    // Using positive logic buttons read directly.
    buttons.state = PORTA & 0x7;
    #else
    buttons.s0 = !PORTAbits.RA2;
    buttons.s1 = !PORTBbits.RB4;
    buttons.s2 = !PORTBbits.RA4;
    #endif
    debug_hex(buttons.state);

    return buttons.state;
}


/*
 * IO_set_button_callback
 *
 * Set a callback to be used when a button is pressed.
 *
 * @param callback function pointer to be called back to.
 */
void IO_set_button_callback(button_callback_t callback)
{
    button_callback = callback;
}


/*
 * IO_handle_button_event
 *
 * Handle buttons being pressed and released. Call the appropriate
 * button callback when buttons are released. Determine if a button
 * press was long or short.
 *
 * @param none
 */
void IO_handle_button_event()
{
    // Current and previous state of the buttons.
    button_t cur_buttons = Io_get_buttons();
    static button_t pre_buttons = NO_BUTTONS;

    debug_print("Button Handler");
    debug_hex(pre_buttons);
    debug_hex(cur_buttons);
    switch (cur_buttons)
    {
        // Handle button events on release of button
        case NO_BUTTONS:
        {
            timer_stop(TIMER3);
            // Check if button timer finished and if true switch button
            // to long press
            if(!timer_flags.timer2_flag)
            {
                switch (pre_buttons)
                {
                    case BUTTON1:
                    {
                        pre_buttons = BUTTON1_LONG;
                        break;
                    }
                    case BUTTON2:
                    {
                        pre_buttons = BUTTON2_LONG;
                        break;
                    }
                    case BUTTON3:
                    {
                        pre_buttons = BUTTON3_LONG;
                        break;
                    }
                    default:
                        break;
                }
            }
            else
            {
                // Stop the button timer if it hasn't expired yet.
                timer_stop(TIMER2);
            }
            // Callback to button handler
            debug_print("Callback");
            (*button_callback)(pre_buttons);
        }
        // If a button has been pressed down start the timer for 3s to
        // check for long presses.
        case BUTTON1:
        case BUTTON2:
        {
            // For buttons 1 and 2 detect if the button has been held
            // down and trigger a timer every 500ms in that case
            timer_start(TIMER3, 500);
        }
        case BUTTON3:
        {
            timer_start(TIMER2, 3000);
            break;
        }
    }
    // Update the previous buttons
    pre_buttons = cur_buttons;
}


void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
    // Clear the interrupt
    IFS1bits.CNIF = 0;
    interrupt_state.cn_trig = 1;

    return;
}


/*
 * LED_toggle
 *
 * Toggle the LED on port B8
 */
void LED_toggle()
{
    LATBbits.LATB8 ^= 1;
}

/*
 * LED_on
 *
 * Turn the LED on port B8 on
 */
void LED_on()
{
    LATBbits.LATB8 = 1;
}

/*
 * LED_off
 *
 * Turn the LED on port B8 off
 */
void LED_off()
{
    LATBbits.LATB8 = 0;
}
