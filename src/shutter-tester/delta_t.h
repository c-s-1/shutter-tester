#ifndef DELTA_T_H_
#define DELTA_T_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define DELTA_T_STEPS_PER_SECOND F_CPU/256; // max number of steps fitting into one second.

uint32_t delta_t_steps;
uint8_t delta_t_seconds;

void delta_t_arm(); // call before measurement to prepare everything.
float delta_t_get(); // get last measurement in seconds as float. Returns 0 if last measurement isn't finished yet.

#endif
