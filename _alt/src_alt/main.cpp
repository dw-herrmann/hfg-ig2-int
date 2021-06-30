#include <Arduino.h>
/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  The circuit:
  - LED attached from pin 13 to ground through 220 ohm resistor
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Button
*/

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 5; // the number of the pushbutton pin
const int ledPin = 13;   // the number of the LED pin

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status
int buttonSensor = 0; // variable for reading the pushbutton status

bool onoff = false;


void setup()
{
    // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);
    // initialize the pushbutton pin as an input:
    pinMode(buttonPin, INPUT);

    Serial.begin(9600);
}

void loop()
{
    // read the state of the pushbutton value:
    buttonSensor = digitalRead(buttonPin);

    // Serial.print("sensorVal: ");
    // Serial.println(buttonSensor);
    // Serial.print("ledState: ");
    // Serial.println(buttonState);
    // Serial.print("onoff: ");
    // Serial.println(onoff);

    Serial.print(buttonState);
    Serial.print(buttonSensor);
    Serial.println(onoff);

    // falls button gedrückt und state aus
    if (buttonSensor && !buttonState)
    {
        buttonState = true;
        Serial.print("button pressed");
        onoff = !onoff;
    }

    // falls button nicht gedrückt und state an
    else if (!buttonSensor && buttonState)
    {
        buttonState = false;
    }

    delay(100);
}
