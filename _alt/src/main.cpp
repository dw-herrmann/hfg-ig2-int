#include <Arduino.h>
#include <FastLED.h>

const int inPins[6] = {A2, A3, A4, A5, A6, A7};
const int button = 6;
int analogvalues[6];
int analogvaluesHighest[6];
int analogvaluesLowest[6] = {200, 200, 200, 200, 200, 200};
int analogvaluesPercentage[6];

void setup()
{
    Serial.begin(9600);
    for (size_t i = 0; i < 6; i++)
    {
        pinMode(inPins[i], INPUT);
    }
}

void getAnalogArray()
{
    for (int i = 0; i < 6; i++)
    {
        analogvalues[i] = analogRead(inPins[i]);

        // höchsten Wert finden
        if (analogvalues[i] > analogvaluesHighest[i])
        {
            analogvaluesHighest[i] = analogvalues[i];
        }

        // niedrigsten Wert finden
        if (analogvalues[i] < analogvaluesLowest[i])
        {
            analogvaluesLowest[i] = analogvalues[i];
        }

        analogvaluesPercentage[i] = map(analogvalues[i], analogvaluesLowest[i], analogvaluesHighest[i], 0, 100);
    }
}

int findHighestValue(int array[], int posOrValue = 0)
{
    int highestValue = 0;
    int highestPos = 0;

    // Höchsten Wert finden
    for (size_t i = 0; i < 6; i++)
    {
        if (array[i] > highestValue)
        {
            highestValue = array[i];
        }
    }

    // Höchste Position finden
    for (size_t i = 0; i < 6; i++)
    {
        if (highestValue == array[i])
        {
            highestPos = i;
        }
    }

    if (posOrValue == 1)
    {
        return highestPos;
    }

    return highestValue;
}

void loop()
{
    Serial.println(" ");
    getAnalogArray();

    Serial.print(
        findHighestValue(analogvaluesPercentage, 1));
    Serial.print("\t");

    // hier Werte der Liste abfragen
    for (int i = 0; i < 6; i++)
    {
        // Serial.print(analogvalues[i]);
        // Serial.print(analogvaluesHighest[i]);
        Serial.print(analogvaluesPercentage[i]);
        Serial.print("\t");
    }

    Serial.print("\t");
    Serial.println("-");
}