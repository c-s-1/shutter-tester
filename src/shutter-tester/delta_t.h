#ifndef DELTA_T_H_
#define DELTA_T_H_

#include <avr/io.h>
#include <avr/interrupt.h>

void delta_t_arm(); // call before measurement to prepare everything.
void delta_t_disarm(); // call to reset prepared things
float delta_t_get(); // get last measurement in seconds as float. Returns 0 if last measurement isn't finished yet.

#endif

/* Usage:
   call delta_t_arm() to prepare the the timer and the interrupts.
   If a falling Edge on OC1A is detected, the current value of the timer is dumped.
   On the second time a falling Edge is detected on OC1A, the delta between the timer values is calculated.
   From now on, delta_t_get() returns the valid timedelta in seconds as a float.
   Calling delta_t_get() during measurement (after calling delta_t_arm but befor the second falling edge was detected),  0 is returned.
*/
