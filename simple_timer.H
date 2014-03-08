/* 
    File: simple_timer.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 09/03/19

*/

#ifndef _SIMPLE_TIMER_H_
#define _SIMPLE_TIMER_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "exceptions.H"

/* Note: The register set data structure REGS is defined in "exceptions.H" */

/*--------------------------------------------------------------------------*/
/* S I M P L E   T I M E R  */
/*--------------------------------------------------------------------------*/

class SimpleTimer {

private:

  /* How long has the system been running? */
  static unsigned long seconds; 
  static int           ticks;   /* ticks since last "seconds" update.    */

  /* At what frequency do we update the ticks counter? */
  static int hz;                /* Actually, by defaults it is 18.22Hz.
                                   In this way, a 16-bit counter wraps
                                   around every hour.                    */

  static void set_frequency(int _hz);
  /* Set the interrupt frequency for the simple timer. */

public :

  static void handler(REGS *_r);
  /* This must be installed as the interrupt handler for the timer 
     when the system gets initialized. (e.g. in "main.C")  
  */

  static void init(int _hz);
  /* Initialize the simple timer, and set its frequency. Do not install the 
     timer handler before you have the timer initialized! */

  static void current(unsigned long * _seconds, int * _ticks);
  /* Return the current "time" since the system started. */

  static void wait(unsigned long _seconds);
  /* Wait for a particular time to be passed. The implementation is based 
     on busy looping! */

};

#endif
