/*
 * File:   main.c
 * Author: andy
 *
 * Created on October 11, 2022, 12:30 PM
 */


#include "xc.h"
#include "main.h"
#include "app.h"
#include "io.h"
#include "timer.h"
#include "UART2.h"

#ifdef ANDY_HARDWARE
// CLOCK CONTROL
#pragma config IESO = OFF    // 2 Speed Startup disabled
#pragma config FNOSC = LPFRC//FRC  // Start up CLK = 31 KHz
#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled
#pragma config SOSCSEL = SOSCLP // Secondary oscillator for Low Power Operation
#pragma config POSCFREQ = MS  //Primary Oscillator/External clk freq betwn 100kHz and 8 MHz. Options: LS, MS, HS
#pragma config OSCIOFNC = ON  //CLKO output disabled on pin 8, use as IO.
#pragma config POSCMOD = NONE  // Primary oscillator mode is disabled

// Set the PGx3 port as the programmer
#pragma config ICS = PGx3
#endif

#ifndef ANDY_HARDWARE
//// CONFIGURATION BITS - PRE-PROCESSOR DIRECTIVES ////

// Code protection
#pragma config BSS = OFF // Boot segment code protect disabled
#pragma config BWRP = OFF // Boot sengment flash write protection off
#pragma config GCP = OFF // general segment code protecion off
#pragma config GWRP = OFF

// CLOCK CONTROL
#pragma config IESO = OFF    // 2 Speed Startup disabled
#pragma config FNOSC = LPFRC//FRC  // Start up CLK = 31 KHz
#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled
#pragma config SOSCSEL = SOSCLP // Secondary oscillator for Low Power Operation
#pragma config POSCFREQ = HS  //Primary Oscillator/External clk freq betwn 100kHz and 8 MHz. Options: LS, MS, HS
#pragma config OSCIOFNC = ON  //CLKO output disabled on pin 8, use as IO.
#pragma config POSCMOD = NONE  // Primary oscillator mode is disabled

// WDT
#pragma config FWDTEN = OFF // WDT is off
#pragma config WINDIS = OFF // STANDARD WDT/. Applicable if WDT is on
#pragma config FWPSA = PR32 // WDT is selected uses prescaler of 32
#pragma config WDTPS = PS1 // WDT postscler is 1 if WDT selected

//MCLR/RA5 CONTROL
#pragma config MCLRE = OFF // RA5 pin configured as input, MCLR reset on RA5 diabled

//BOR  - FPOR Register
#pragma config BORV = LPBOR // LPBOR value=2V is BOR enabled
#pragma config BOREN = BOR0 // BOR controlled using SBOREN bit
#pragma config PWRTEN = OFF // Powerup timer disabled
#pragma config I2C1SEL = PRI // Default location for SCL1/SDA1 pin

//JTAG FICD Register
#pragma config BKBUG = OFF // Background Debugger functions disabled
#pragma config ICS = PGx2 // PGC2 (pin2) & PGD2 (pin3) are used to connect PICKIT3 debugger

// Deep Sleep RTCC WDT
#pragma config DSWDTEN = OFF // Deep Sleep WDT is disabled
#pragma config DSBOREN = OFF // Deep Sleep BOR is disabled
#pragma config RTCOSC = LPRC// RTCC uses LPRC 32kHz for clock
#pragma config DSWDTOSC = LPRC // DeepSleep WDT uses Lo Power RC clk
#pragma config DSWDTPS = DSWDTPS7 // DSWDT postscaler set to 32768

#endif

interrupt_State interrupt_state = {0};
App_State app_state = STATE_ENTER_TIME;

int main(void)
{
    // Disable Nested Interrupts
    INTCON1bits.NSTDIS = 0;

    // Initialize settings for timer, IO, and UART.
    IO_init();
    timer_init();
    InitUART2();

    APP_state_enter_time_init();
    while(1)
    {
        // Handle non app interrupts
        if(interrupt_state.cn_trig)
        {
            interrupt_state.cn_trig = 0;
            IO_handle_button_event();
            // uint8_t buttons = io_get_buttons();
        }
        if(interrupt_state.timer2_trig)
        {
            interrupt_state.timer2_trig = 0;
            timer_stop(TIMER2);
        }
        APP_state_machine_main();

        // Wait for interrupt to trigger next iteration of main loop
        Idle();
    }
    return 0;
}
