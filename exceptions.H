/* 
    File: exceptions.H

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

#ifndef _exceptions_H_                   // include file only once
#define _exceptions_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

#define EXCEPTION_TABLE_SIZE 32

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "utils.H"
#include "assert.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* This defines what the stack looks like when the exception/interrupt reaches
   the exception dispatcher. */
typedef struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;    
} REGS;
 
/* Exception handlers are functions that take a pointer to a REGS structure 
   as input and return void. */
typedef void (* ExceptionHandler)(REGS*);

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* E x c e p t i o n D i s p a t c h e r  */
/*--------------------------------------------------------------------------*/

  
/* -- INITIALIZER */
extern "C" void init_exception_dispatcher();
/* This function is called to initialize the high-level exception handling. 
   No high level exception handlers are installed yet. If an exception 
   occurs at this point, the system displays an error message and 
   terminates. */


/* -- HANDLE EXCEPTION */
extern "C" void dispatch_exception(REGS * _r);
/* This is called (through some detours) by the low-level exception handler.
   It then looks up the exception code in the register context and calls the
   the installed high-level exception handler. */

/* -- POPULATE EXCEPTION-DISPATCHER TABLE */
extern "C" void register_exception_handler(unsigned int      _isr_code,
    	   		  	           ExceptionHandler  _handler);
/* This function allows to install an exception handler for the given 
   exception code. The handler is a function pointer defined above. 
   Interrupt handlers are installed as exception handlers as well.
   The 'register_interrupt' function uses irq2isr to map the IRQ 
   number to the code. */

#endif


