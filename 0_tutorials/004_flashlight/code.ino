int ledPin = 13;
int sensorPin = 2;
int sensorVal;
int buttonState = 0;
boolean ledState = false;


void setup()
{
  // Pins definieren
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
}

void loop()
{
  // Button gepresst oder nicht
  sensorVal = digitalRead(sensorPin);
  Serial.println(sensorVal);

  // falls button gedrückt und state aus
  if (sensorVal == 1 && buttonState == 0)
  {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    buttonState = 1;
    Serial.println("button pressed");
  }
  
  // falls button nicht gedrückt und state an
  else if (sensorVal == 0 && buttonState == 1)
  {
    buttonState = 0;
  }
}