int ledPin1 = 9;
int ledPin2 = 10;
int ledPin3 = 11;
int sensorPin = 2;
int sensorVal;
int buttonState = 0;
int ledState = 0;
void setup()
{
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
}
void loop()
{
  sensorVal = digitalRead(sensorPin);
  // Serial.print("sensorVal: ");
  // Serial.println(sensorVal);
  // Serial.print("ledState: ");
  // Serial.println(ledState);
  if (sensorVal == 1 && buttonState == 0)
  {
    ledState = ledState + 1;
    buttonState = 1;
  }
  else if (sensorVal == 0 && buttonState == 1)
  {
    buttonState = 0;
  }
  switch (ledState)
  {
  case 0:
    analogWrite(ledPin1, 0);
    analogWrite(ledPin2, 0);
    analogWrite(ledPin3, 0);
    break;
  case 1:
    analogWrite(ledPin1, 10);
    analogWrite(ledPin2, 200);
    analogWrite(ledPin3, 0);
    break;
  case 2:
    analogWrite(ledPin1, 200);
    analogWrite(ledPin2, 30);
    analogWrite(ledPin3, 50);
    break;
  case 3:
    ledState = 0;
    break;
  }
}