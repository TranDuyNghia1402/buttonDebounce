/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

typedef enum
{
    NO_PRESS = 0,
    SINGLE_PRESS,   
    DOUBLE_PRESS,           
} eButtonEvent;

static volatile eButtonEvent buttonEvent;

typedef struct
{
    GPIO_PIN buttonPin;
    volatile bool isSinglePress;    // single or double press
    eButtonEvent buttonEvent;
    volatile bool iSample;
}eButton;
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
volatile bool state = true;
volatile bool isSinglePress = false;
static volatile uint32_t counter = 0;

volatile bool isPrint = false;

void gpio_callback(GPIO_PIN pin, uintptr_t context)
{
    GPIO_PIN* pPin = (GPIO_PIN*)context;
    if (pin == (*pPin))
    {
        if (!USER_BUTTON1_Get() && state)
        {
            TMR3_Start();
            state = false;
        } 
    }
}

void tmr_callback (uint32_t status, uintptr_t context) 
{
    if (USER_BUTTON1_Get() == 0)
    {            
        if (!isSinglePress) { TMR1_Start(); }           
        isSinglePress = (isSinglePress) ? false : true;
        state = true;
        TMR3_Stop();
    }
}

void tmr1_callback(uint32_t status, uintptr_t context)
{
    buttonEvent = (isSinglePress) ? SINGLE_PRESS : DOUBLE_PRESS;
    isSinglePress = false;
    TMR1_Stop();
    isPrint = true;
}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );  
    CORETIMER_Start();
    printf("hello, World\r\n");
    int pin = USER_BUTTON1_PIN;
    GPIO_PinInterruptCallbackRegister(USER_BUTTON1_PIN, gpio_callback, (uintptr_t)&pin);
    GPIO_PinInterruptEnable(USER_BUTTON1_PIN);    
    TMR3_CallbackRegister(tmr_callback, 0);    
    TMR1_CallbackRegister(tmr1_callback, 0);
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        if (isPrint)
        {
            switch (buttonEvent)
            {
                case SINGLE_PRESS:
                    printf("Single\r\n");
                    isPrint = false;
                    break;
                case DOUBLE_PRESS:
                    printf("Double\r\n");
                    isPrint = false;
                    break;
                default:
                    isPrint = false;
                    break;
            }
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

