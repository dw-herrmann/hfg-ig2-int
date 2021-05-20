int sensorAnalogPin = A0; // Select the Arduino input pin to accept the Sound Sensor's analog output
int sensorDigitalPin = 3; // Select the Arduino input pin to accept the Sound Sensor's digital output
int analogValue = 0;      // Define variable to store the analog value coming from the Sound Sensor
int digitalValue;         // Define variable to store the digital value coming from the Sound Sensor
int Led13 = 13;           // Define LED port; this is the LED built in to the Arduino (labled L)
int Led1 = 11;
int Led2 = 10;
int Led3 = 9;
int min = 650;
int max;
int peak;

void setup()
{
  Serial.begin(9600);               // The IDE settings for Serial Monitor/Plotter (preferred) must match this speed
  pinMode(sensorDigitalPin, INPUT); // Define pin 3 as an input port, to accept digital input
  pinMode(Led13, OUTPUT);           // Define LED13 as an output port, to indicate digital trigger reached
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Led3, OUTPUT);
  pinMode(sensorAnalogPin, INPUT);
}

void loop()
{

  analogValue = analogRead(sensorAnalogPin);    // Read the value of the analog interface A0 assigned to digitalValue
  digitalValue = digitalRead(sensorDigitalPin); // Read the value of the digital interface 7 assigned to digitalValue
  // Serial.print(analogValue);                    // Send the analog value to the serial transmit interface
  // Serial.print(", ");
  // Serial.println(digitalValue);
  if (digitalValue == HIGH) // When the Sound Sensor sends signla, via voltage present, light LED13 (L)
  {
    digitalWrite(Led13, HIGH);
  }
  else
  {
    digitalWrite(Led13, LOW);
  }

  int ledOutput = abs(map(analogValue, 660, 730, 0, 255));

  // Serial.print(analogValue); // Send the analog value to the serial transmit interface
  // Serial.print(" = ");
  // Serial.println(ledOutput);

  analogWrite(Led1, ledOutput);
  analogWrite(Led2, ledOutput);
  analogWrite(Led3, ledOutput);

  if (min != 0)
  {
    if (analogValue < min)
    {
      min = analogValue;
    }
    else if (analogValue > max)
    {
      max = analogValue;
    }
  }

  // Serial.print(min);
  // Serial.print(" & ");
  // Serial.println(max);

  Serial.println(analogValue);
  
}
