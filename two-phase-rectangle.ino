
/*
Pins A0 and A1 should be connected to the middle rail of potentiometers whose outer rails are connected to +5 and GND. They adjust the duty cycle and frequency.

The two output pins are pin 5 and pin 11. They will always be identical frequency and duty cycle, but 180 degrees phase shifted from each other.
*/

int user_cycles;
int user_duty;

void setup() {
  pinMode(11, OUTPUT);
  pinMode(5, OUTPUT);

  TCCR1A = _BV(COM1A1)
	 | _BV(WGM11);
  TCCR1B = _BV(WGM13);
  ICR1 = 500;  // top
  OCR1A = 250;	// bottom
  TCCR1B |= _BV(CS20);	// set prescale to div 1 and start the timer

  TCCR3A = _BV(COM1A0)
         | _BV(COM1A1)
	 | _BV(WGM11);
  TCCR3B = _BV(WGM13);
  ICR3 = 500; // top
  OCR3A = 250;	// bottom
  TCCR3B |= _BV(CS20);	// set prescale to div 1 and start the timer
}

long i = 0;

void set(int cycles, float duty) {
  char oldSREG;
  int dutyCycle = cycles * duty;
  if (dutyCycle < 8) {
    dutyCycle = 8; // Prevent duty cycle from being too short
  }
  if (dutyCycle > 1013) {
    dutyCycle = 1013; // Prevent duty cycle from being 100%
  }
  if (cycles < 50) {
    cycles = 50; // Prevent frequency from being too high
  }

  oldSREG = SREG;             // Save the registers
  cli();		      // Disable interrupts for 16 bit register access

  ICR1 = cycles + cycles;              // ICR1 is TOP in p & f correct pwm mode
  OCR1A = dutyCycle;          // OCR1A is BOTTOM

  ICR3 = cycles + cycles;              // ICR3 is TOP in p & f correct pwm mode
  OCR3A = cycles + cycles - dutyCycle;          // OCR3A is BOTTOM

  SREG = oldSREG;             // Restore the registers
}


void loop() {
  int temp_cycles = analogRead(A0);
  int temp_duty = analogRead(A1);

  // Add some fudge factor to avoid jittering
  if (abs(temp_cycles - user_cycles) > 4 || abs(temp_duty - user_duty) > 4) {
    user_cycles = temp_cycles;
    user_duty = temp_duty;
    set(user_cycles, user_duty / 1024.0);
  }
}
