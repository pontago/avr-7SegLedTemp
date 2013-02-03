
#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>

#define DIG_MAX 3
#define NUM_MAX 12

void showNumber(char dig, char num) {
  char nums[] = {0b00111111, 0b00000110, 0b01011011, 0b01001111,
    0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111,
    0b01101111, 0b10000000, 0b01000000};

  if (dig < DIG_MAX && num < NUM_MAX) {
    PORTB = 1 << dig;

    if (num == -1) {
      PORTD = 0b00000000;
    }
    else {
      PORTD = nums[num];
    }
  }
}

void adc_init(void) {
  ADMUX = _BV(REFS0) | _BV(REFS1);
  ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADPS0);
  DIDR0 = _BV(ADC0D);

  DDRC = 0b00000000;
  PORTC = 0b00000000;
}

uint16_t adc_read(void){
  uint16_t v;

  ADCSRA |= _BV(ADSC);

  loop_until_bit_is_clear(ADCSRA, ADSC);

  v = ADCL;
  v |= (ADCH << 8);

  return v;
}

int main(void) {
  char currentNums[] = {-1, -1, -1};
  char n = 0, dig = 0;
  uint16_t result = 0;
  int16_t temp = 0;

  DDRB = 0b00000111;
  DDRD = 0b11111111;

  adc_init();

  for (;;) {
    char i, l;

    result = adc_read();
    temp = (int16_t)((result * 1.1 / 1024 - 0.424) / 0.00625);

    if (temp < 0) {
      temp = abs(temp);
      currentNums[0] = 11;
    }
    else {
      currentNums[0] = -1;
    }
    currentNums[2] = (char)(temp % 10);
    currentNums[1] = (char)(temp / 10);

    for (i = 0; i < 100; i++) {
      for (l = 0; l < 3; l++) {
        showNumber(l, currentNums[l]);
        _delay_ms(3);
      }
    }
  }

  return 0;
}
