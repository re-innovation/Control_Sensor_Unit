
/*
  This is code for a Weight/PIR/Colour sensror node

  It runs at 9600 Baud
  It sleeps and wakes up on watch dog timer (every 1 second?)
  Check weight
  If weight  = initial weight then it pauses sound playback, switches off power relay then goes back to sleep
  If weight != initial weight
  then sets power output (relay) high
  Plays the sound (or starts it playing again)
  Then goes back to check weight

  This is the OLD operation
  //  It takes serial data and then performs actions based upon this:
  //  If the Serial string is:
  //  "PIR?"
  //   Then it returns "TRUE" or "FALSE" Depending upon the PIR sensor
  //  "COLOUR?"
  //  Then it returns the colour as "RXXXGXXXBXXX" Where the XXX values are from the colour sensor
  //  "WEIGHT?"
  //  The it returns the weight as "WXXXXX" Where XXXXX is the weight in grammes
  //
  //  This uses the following libraries:
  //  HX711 library
  //  https://github.com/bogde/HX711
  //
  //  The colour sensor is TCS3200
  //
  //  Sleep will be used to power down
  //  The unit will wake up in two instances:
  //    A serial message comes through
  //    The PIR state is true

*/

#include <avr/wdt.h>            // library for default watchdog functions
#include <avr/interrupt.h>      // library for interrupts handling
#include <avr/sleep.h>          // library for sleep
#include <avr/power.h>          // library for power control
#include <avr/io.h>

#include "HX711.h"

// how many times remain to sleep before wake up
int nbr_remaining;

// pins for the sensors:
//PIR
const int PIRPin = 3;   // This is on an interrupt pin

//COLOUR
const int colourS0 = 6;
const int colourS1 = 7;
const int colourS2 = 8;
const int colourS3 = 9;
const int colourLED = 10;
const int colourOUT = 11;
const int sw1 = 2;
const int relay = 12;

int frequency = 0;

//WEIGHT
const int weightSCK = 5;
const int weightDT = 4;
HX711 scale(weightDT, weightSCK);    // parameter "gain" is ommited; the default value 128 is used by the library
float weightData;

volatile int __watch_dog_timer_flag = 1;

// Define WDT interrupt callback
ISR(WDT_vect)
{
  __watch_dog_timer_flag = 1;
}

// function to configure the watchdog: let it sleep 8 seconds before firing
// when firing, configure it for resuming program execution
void configure_wdt(void)
{
  // Disable processor reset on WDT time-out
  MCUSR &= ~(1 << WDRF);

  // Tell WDT we're going to change its prescaler
  WDTCSR |= (1 << WDCE);

  // Set prescaler to 1 second
  WDTCSR = 1 << WDP1 | 1 << WDP2;

  // Turn on the WDT
  WDTCSR |= (1 << WDIE);
}

void enterSleepMode()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();

  sleep_disable();
  power_all_enable();
}

//// Put the Arduino to deep sleep. Only an interrupt can wake it up.
//void sleep(int ncycles)
//{
//  nbr_remaining = ncycles; // defines how many cycles should sleep
//  // Set sleep to full power down.  Only external interrupts or
//  // the watchdog timer can wake the CPU!
//  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//
//  sleep_enable(); // enables the sleep bit in the mcucr register
//
//  // Turn off the ADC while asleep.
//  power_adc_disable();
//  power_spi_disable();
//  power_timer0_disable();
//  power_timer1_disable();
//  power_timer2_disable();
//  power_twi_disable();
//
//  while (nbr_remaining > 0){ // while some cycles left, sleep!
//
//  // Enable sleep and enter sleep mode.
//  sleep_mode();
//
//  // CPU is now asleep and program execution completely halts!
//  // Once awake, execution will resume at this point if the
//  // watchdog is configured for resume rather than restart
//  // When awake, disable sleep mode
//  sleep_disable();
//  // we have slept one time more
//  nbr_remaining = nbr_remaining - 1;
//  }
//  // put everything on again
//  power_all_enable();
//}

void setup() {
  pinMode(colourS0, OUTPUT);
  pinMode(colourS1, OUTPUT);
  pinMode(colourS2, OUTPUT);
  pinMode(colourS3, OUTPUT);
  pinMode(colourLED, OUTPUT);
  pinMode(colourOUT, INPUT);

  pinMode(sw1, INPUT_PULLUP);
  pinMode(relay, OUTPUT);

  pinMode(PIRPin, INPUT);

  digitalWrite(colourLED, LOW); // Switch OFF the LEDs?
  // Setting frequency-scaling to 20%
  digitalWrite(colourS0, HIGH);
  digitalWrite(colourS1, LOW);

  // initialize serial:
  Serial.begin(9600);
  Serial.print("Starting....");

  // Configue the weight sensor
  // Need to calibrate for this load cell used:
  scale.set_scale(2280.f);     // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();                // reset the scale to 0

  Serial.println("Done");

  // configure the watchdog
  configure_wdt();
}

void loop() {
  // sleep for a given number of cycles (here, 1 * 1 seconds) in lowest power mode
  Serial.println("Zzzz...");
  if (__watch_dog_timer_flag == 1) {
    __watch_dog_timer_flag = 0;
    // do things here ...
    delay(100);
    Serial.println("Woken Up");
    // usefull stuff should be done here before next long sleep
    


    enterSleepMode();
  }
}


void readWeight()
{
  scale.power_up();
  weightData = scale.get_units(10);
  Serial.print("W");
  Serial.println(weightData, 0);
  scale.power_down();              // put the ADC in sleep mode
  delay(100);
}

