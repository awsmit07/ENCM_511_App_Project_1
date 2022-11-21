/*
 * File: app.h
 * Author: Andy Smit
 * Comments:
 * Revision history:
 */

// This is a guard condition so that contents of this file are not included
// more than once.
#ifndef APP_H
#define	APP_H

#include <xc.h> // include processor files - each processor file is guarded.

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    /**
     * \b app_state_machine_main \b
     *
     * Fuction that runs every loop of main in the state machine. Responsible for
     */
    void APP_state_machine_main(void);

    void APP_state_enter_time_init(void);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* APP_H */

