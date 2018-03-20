#include "delta_t.h"

uint16_t steps;
uint8_t steps_ovf;
uint8_t state = 0;
/* 0: non operational
   1: waiting for first interrupt
   2: waiting for second interrupt
*/

void delta_t_arm() {
  TCCR1A = TCCR1B = TCCR1C = TIMSK1 = 0; // Reset all TIMER1 config register
  TCCR1B = (0 << ICES1) | (1 << CS12); // Input Capture Edge Select, 256 prescaler
  TIMSK1 = (1 << ICIE1) | (1 << OCIE1A); // Input Capture Interrupt Enable, Output Compare A Match Interrupt Enable

  steps = 0;
  steps_ovf = 0;
  state = 1;
  sei();
}

void delta_t_disarm() {
  TCCR1A = TCCR1B = TCCR1C = TIMSK1 = 0; // Reset all TIMER1 config register
  steps = steps_ovf = state = 0;
}

float delta_t_get() {
  if (!state && (steps || steps_ovf) {
    float steps_per_second = F_CPU / 256.0;
    return  steps_per_second / (float) steps + steps_per_second / (float) steps_ovf * UINT16_MAX;
  }
  return 0.0;
}

ISR(TIMER1_COMPA_vect) {
  if (state == 2)
    steps_ovf++;
}

ISR(TIMER1_CAPT_vect) {
  uint16_t dumped_time = ICR1;
  switch (state) {
    case 1:
      OCR1A = dumped_time;
      steps = dumped_time;
      state = 2;
      break;
    case 2:
      steps = dumped_time - steps;
    default:
      state = 0;
      break;
  }
}
