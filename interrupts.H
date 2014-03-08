/* 
    File: interrupts.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 09/03/05

    Description: High-level exception handling. 

    CPU exceptions are caught by low-level exception and interrupt 
    handler stubs, which all eventually cause the high-level
    exception dispatcher to be called (see 'exd_dispatch() below).

    The dispatcher then looks up the appropriate exception handler 
    for the given exception, and calls it. Exception handlers are 
    installed in form of function pointers. (See definition of 
    'ExceptionHandler' below.)
    

*/

#ifndef _interrupts_H_                   // include file only once
#define _interrupts_H_



/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "utils.H"
#include "assert.H"
#include "exceptions.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/
 
/* Note: the register set structure REGS is defined in "exceptions.H". */

/* Interrupt handlers are functions that take a pointer to a REGS structure 
   as input and return void. (No different than exception handlers.) */
typedef void (* InterruptHandler)(REGS*);

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* I n t e r r u p t  D i s p a t c h e r  */
/*--------------------------------------------------------------------------*/

  
/* -- INITIALIZER */
extern "C" void init_interrupt_dispatcher();
/* This function is called to initialize the high-level exception handling. 
   No high level exception handlers are installed yet. If an exception 
   occurs at this point, the system displays an error message and 
   terminates. */


/* -- HANDLE INTERRUPT */
extern "C" void dispatch_interrupt(REGS * _r);
/* This is called (through some detours) by the low-level exception handler.
   It then looks up the exception code in the register context and calls the
   the installed high-level exception handler. */

/* -- POPULATE EXCEPTION-DISPATCHER TABLE */

extern "C" void register_interrupt_handler(unsigned int      _irq_code,
		                       ExceptionHandler  _handler);
/* This function allows to install an exception handler for the given 
   exception code. The handler is a function pointer defined above. 
   Interrupt handlers are installed as exception handlers as well.
   The 'register_interrupt' function uses irq2isr to map the IRQ 
   number to the code. */


#endif


