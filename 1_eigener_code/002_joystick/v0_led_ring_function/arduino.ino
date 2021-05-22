// Farben einstellen
void setLED(int firstLED, int amount, int color0, int color1, int color2, bool printToConsole)
{
    int maxLEDs = 32;

    // Start und Ende festlegen
    int startLED = firstLED % maxLEDs;
    int endLED = (firstLED + amount) % maxLEDs;

    // Falls negativ, mach Zahl korrekt
    if (startLED <= 0)
    {
        startLED = startLED + maxLEDs;
    }

    if (endLED <= 0)
    {
        endLED = endLED + maxLEDs;
    }

    // Wenn die erstel LED kleiner als die Letzte ist
    if (startLED < endLED)
    {
        // Alle LEDs in Bereich einfärben
        for (size_t i = startLED; i < endLED; i++)
        {
            leds[i].setHSV(color0, color1, color2);
        }

        if (printToConsole)
        {
            Serial.print(startLED);
            Serial.print("-");
            Serial.print(endLED);
        }
    }
    else
    {
        // Alle LEDs von der ersten bis LED anzahl einfärben …
        for (size_t i = startLED; i < maxLEDs; i++)
        {
            leds[i].setHSV(color0, color1, color2);
        }

        // … und alle restlichen von 0 bis Rest einfärben
        for (size_t i = 0; i < endLED; i++)
        {
            leds[i].setHSV(color0, color1, color2);
        }

        if (printToConsole)
        {
            Serial.print(endLED);
            Serial.print("-");
            Serial.print(maxLEDs);
            Serial.print(" & ");
            Serial.print(0);
            Serial.print("-");
            Serial.print(endLED);
        }
    }
}