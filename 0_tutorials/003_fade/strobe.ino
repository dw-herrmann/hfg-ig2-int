/*
  Stroboskob
*/

// Pins als Variable
// int pins = {
//   default: LED_BUILTIN
// }

int led = 11;
int brightness = 0;
int fadeamount = 5;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(led, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  analogWrite(led, brightness); // turn the LED on (HIGH is the voltage level)

  brightness += fadeamount;

  if (brightness >= 255 || brightness <= 0) {
    fadeamount *= -1;
  }

  if (brightness <= 0) {
    delay(300); // wait long
  } else
  {
    delay(30); // wait short
  }
  

}