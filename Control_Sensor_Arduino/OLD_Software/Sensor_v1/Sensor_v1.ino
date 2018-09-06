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



*/

#include "HX711.h"


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

int frequency = 0;

//WEIGHT
const int weightSCK = 5;
const int weightDT = 4;
HX711 scale(weightDT, weightSCK);    // parameter "gain" is ommited; the default value 128 is used by the library

// SERIAL read/write
const uint8_t MaxCharBuffer = 8;
String CharBuffer;
int c;

void setup() {

  pinMode(colourS0, OUTPUT);
  pinMode(colourS1, OUTPUT);
  pinMode(colourS2, OUTPUT);
  pinMode(colourS3, OUTPUT);
  pinMode(colourLED, OUTPUT);  
  pinMode(colourOUT, INPUT);
  
  pinMode(PIRPin, INPUT);
  
  digitalWrite(colourLED,LOW);  // Switch OFF the LEDs?
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

  // if there's any serial available, read it:
  while (Serial.available() > 0)
  {
    c = Serial.read();
    if (c < 0)
    {
      Serial.println("serial error");
    }
    else if (c == '?')
    {
      handleBuffer();
    }
    else if (c >= 32 && c <= 127)
    {
      if (CharBuffer.length() < MaxCharBuffer)
      {
        // printable character, append to buffer
        CharBuffer += (char)c;
      }
      else
      {
        Serial.println("Max Buffer");
        CharBuffer = ""; // Clear the buffer and wait on next one
      }
    }
  }
}

void handleBuffer ()
{
  // This is where we deal with the char buffer
  //Serial.println(CharBuffer);
  if (CharBuffer == "COLOUR")
  {
    Serial.println("Asked for COLOUR");
    digitalWrite(colourLED,HIGH);  // Switch OFF the LEDs?
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
    digitalWrite(colourLED,LOW);  // Switch OFF the LEDs?

  }
  else if (CharBuffer == "WEIGHT")
  {
    Serial.println("Asked for WEIGHT");
    scale.power_up();
    float weightData = scale.get_units(10);

    Serial.print("W");
    Serial.println(weightData, 0);
    scale.power_down();              // put the ADC in sleep mode

  }
  else if (CharBuffer == "PIR")
  {
    Serial.println("Asked for PIR");
    bool PIRread = digitalRead(PIRPin);
    if(PIRread==HIGH)
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

