int ledPin = 13;
int sensorPin = 2;
int sensorVal;

void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
}

void loop()
{
  sensorVal = digitalRead(sensorPin);
  Serial.println(sensorVal);

  if (sensorVal == 0)
  {
    digitalWrite(ledPin, HIGH);
    delay(100);
  }
  else
  {
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}