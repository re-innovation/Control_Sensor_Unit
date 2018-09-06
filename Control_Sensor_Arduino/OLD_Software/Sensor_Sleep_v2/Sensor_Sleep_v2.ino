/*
  This is code for a Weight/PIR/Colour sensro node
  It runs at 9600 Baud
  It takes serial data and then performs actions based upon this:
  If the Serial string is:
  "PIR?"
   Then it returns "TRUE" or "FALSE" Depending upon the PIR sensor
  "COLOUR?"
  Then it returns the colour as "RXXXGXXXBXXX" Where the XXX values are from the colour sensor
  "WEIGHT?"
  The it returns the weight as "WXXXXX" Where XXXXX is the weight in grammes

  This uses the following libraries:
  HX711 library
  https://github.com/bogde/HX711

  The colour sensor is TCS3200

  Sleep will be used to power down
  The unit will wake up in two instances:
    A serial message comes through
    The PIR state is true

*/

#include "HX711.h"

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

#include <avr/io.h>

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

// SERIAL read/write
const uint8_t MaxCharBuffer = 8;
String CharBuffer;
int c;

int sleepStatus = 0; // variable to store a request for sleep

void sleepNow()
{
  // Set switch as interrupt
  // Set pin 2 as interrupt and attach handler:
  attachInterrupt(0, pinInterrupt, LOW);

  delay(100);

  /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     there is a list of sleep modes which explains which clocks and
     wake up sources are available in which sleep modus.

     In the avr/sleep.h file, the call names of these sleep modus are to be found:

     The 5 different modes are:
     SLEEP_MODE_IDLE -the least power savings
     SLEEP_MODE_ADC
     SLEEP_MODE_PWR_SAVE
     SLEEP_MODE_STANDBY
     SLEEP_MODE_PWR_DOWN -the most power savings

     the power reduction management <avr/power.h> is described in
     http://www.nongnu.org/avr-libc/user-manual/group_avr_power.html
  */

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here

  sleep_enable(); // enables the sleep bit in the mcucr register
  // so sleep is possible. just a safety pin

  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();

  sleep_mode(); // here the device is actually put to sleep!!

  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  sleep_disable(); // first thing after waking from sleep:
  // disable sleep...

  power_all_enable();

}

void pinInterrupt(void)
{
  // Does this on interrupt so code can continue
  detachInterrupt(0);
}

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
  Serial.print("Initialising....");
  // Configue the weight sensor
  // Need to calibrate for this load cell used:
  scale.set_scale(2280.f);     // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();                // reset the scale to 0
  Serial.println("Done");

}

void loop() {

  // Want to stay in this loop.
  // Want to sleep if no serial data
  // *** TO DO ******
  // Go to sleep

  Serial.println("Zzzz...");
  sleepNow();

  // Wake up if switch pressed OR PIR sensed
  delay(100);
  Serial.println("Woken Up");

  readColour();
  readWeight();

  digitalWrite(relay,HIGH);
  delay(2000);
  digitalWrite(relay,LOW);
  
  //  digitalWrite(relay,HIGH);
  //  delay(1000);
  //  digitalWrite(relay,LOW);
  while (digitalRead(sw1) == LOW)
  {
    delay(100);
    // Wait here until switch not presseed
  }


  //  // if there's any serial available, read it:
  //  while (Serial.available() > 0)
  //  {
  //    c = Serial.read();
  //    if (c < 0)
  //    {
  //      Serial.println("serial error");
  //    }
  //    else if (c == '?')
  //    {
  //      handleBuffer();
  //    }
  //    else if (c >= 32 && c <= 127)
  //    {
  //      if (CharBuffer.length() < MaxCharBuffer)
  //      {
  //        // printable character, append to buffer
  //        CharBuffer += (char)c;
  //      }
  //      else
  //      {
  //        Serial.println("Max Buffer");
  //        CharBuffer = ""; // Clear the buffer and wait on next one
  //      }
  //    }
  //  }
}

void handleBuffer ()
{
  // This is where we deal with the char buffer
  //Serial.println(CharBuffer);
  if (CharBuffer == "COLOUR")
  {
    Serial.println("Asked for COLOUR");
    readColour();
  }
  else if (CharBuffer == "WEIGHT")
  {
    Serial.println("Asked for WEIGHT");
    readWeight();

  }
  else if (CharBuffer == "PIR")
  {
    Serial.println("Asked for PIR");
    bool PIRread = digitalRead(PIRPin);
    if (PIRread == HIGH)
    {
      Serial.println("TRUE");
    }
    else
    {
      Serial.println("FALSE");
    }
  }
  else
  {
    Serial.println("BAD");
  }
  CharBuffer = ""; // Reset the buffer for next time

}


void readColour()
{
  digitalWrite(colourLED, HIGH); // Switch OFF the LEDs?
  delay(1000); // Wait for LEDs

  // Setting red filtered photodiodes to be read
  digitalWrite(colourS2, LOW);
  digitalWrite(colourS3, LOW);
  // Reading the output frequency
  frequency = pulseIn(colourOUT, LOW);
  // Printing the value on the serial monitor
  Serial.print("R");//printing name
  Serial.print(frequency);//printing RED color frequency
  delay(100);
  // Setting Green filtered photodiodes to be read
  digitalWrite(colourS2, HIGH);
  digitalWrite(colourS3, HIGH);
  // Reading the output frequency
  frequency = pulseIn(colourOUT, LOW);
  // Printing the value on the serial monitor
  Serial.print("G");//printing name
  Serial.print(frequency);//printing RED color frequency
  delay(100);
  // Setting Blue filtered photodiodes to be read
  digitalWrite(colourS2, LOW);
  digitalWrite(colourS3, HIGH);
  // Reading the output frequency
  frequency = pulseIn(colourOUT, LOW);
  // Printing the value on the serial monitor
  Serial.print("B");//printing name
  Serial.println(frequency);//printing RED color frequency
  delay(1000);
  digitalWrite(colourLED, LOW); // Switch OFF the LEDs?
}

void readWeight()
{
  scale.power_up();
  float weightData = scale.get_units(10);

  Serial.print("W");
  Serial.println(weightData, 0);
  scale.power_down();              // put the ADC in sleep mode
}
