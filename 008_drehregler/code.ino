int encoder01 = 2;
int encoder02 = 3;
int state;
int lastState = 0;
int counter = 0;

int ledPin1 = 9;
int ledPin2 = 10;
int ledPin3 = 11;

void setup()
{
  pinMode(encoder01, INPUT);
  pinMode(encoder02, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  state = digitalRead(encoder01);

  // Bei veränderung des States
  if (state != lastState && state == 0)
  {
    // wenn 2. Reihe schneller ist, zähl runter
    if (digitalRead(encoder02) != state)
    {
      counter--;
    }
    else
    {
      counter++;
    }
  }
  lastState = state;

  Serial.println(counter);
  analogWrite(ledPin1, abs(counter * 5));
  analogWrite(ledPin2, abs(counter * 5));
  analogWrite(ledPin3, abs(counter * 5));
}