int ledPin = 13;
int sensorPin = 2;
int sensorVal;
int buttonState = 0;
int ledState = 0;

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
  Serial.print("sensorVal: ");
  Serial.println(sensorVal);
  Serial.print("ledState: ");
  Serial.println(ledState);

  // falls button gedrückt und state aus
  if (sensorVal == 1 && buttonState == 0)
  {
    ledState = ledState + 1;
    buttonState = 1;
    Serial.print("button pressed");
  }

  // falls button nicht gedrückt und state an
  else if (sensorVal == 0 && buttonState == 1)
  {
    buttonState = 0;
  }

  switch (ledState)
  {
  case 0:
    digitalWrite(ledPin, LOW);
    break;
  case 1:
    digitalWrite(ledPin, HIGH);
    break;
  case 2:
    ledState = 0;
    break;
  }
}