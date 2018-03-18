#include "delta_t.h"

uint8_t state = 0;
/* 0: non operational
   1: waiting for first interrupt
   2: waiting for second interrupt
*/

void delta_t_arm() {
  TCCR1A = TCCR1B = TCCR1C = TIMSK1 = 0; // Reset all TIMER1 config register
  TCCR1B = (1 << WGM12) | (1 << CS12); // Clear Timer Counter on Compare OCR1A Match, 256 prescaler
  TIMSK1 = (1 << ICIE1) | (1 << OCIE1A); // Input Capture Interrupt Enable, Output Compare A Match Interrupt Enable

  OCR1A = DELTA_T_STEPS_PER_SECOND; // TOP of timer

  delta_t_seconds = 0;
  delta_t_steps = 0;
  state = 1;
  sei();
}

float delta_t_get() {
  if (!state) {
    return (float)delta_t_steps / (float)DELTA_T_STEPS_PER_SECOND + (float)delta_t_seconds;
  }
  return 0;
}


ISR(TIME1_COMPA_vect) {
  if (state == 2)
    delta_t_seconds++;
}

ISR(TIMER1_CAPT_vect) {
  uint16_t dumped_time = ICR1;
  switch (state) {
    case 1:
      delta_t_steps = dumped_time;
      state = 2;
      break;
    case 2:
      delta_t_steps = dumped_time - delta_t_steps;
    default:
      state = 0;
      break;
  }
}
