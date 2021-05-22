void feedbackDirection()
{
    // Aktive LED aus Winkel herausfinden
    int activeLED = round(map(input.angle, -180, 180, 0, NUM_LEDS));

    // alle LEDs auf 0 stellen
    setLED(
        0, NUM_LEDS,
        180, 255, 128,
        false);

    // Ab bestimmter Krafteinwirkung aktiviere LED
    if (input.force > 0)
    {
        // nur aktive LED aktivieren, den rest deaktivieren
        setLED(
            activeLED - 2, 4,
            0, 255, 255,
            false);
    }
    else
    {
        // Alle LEDs deaktivieren
        setLED(
            0, NUM_LEDS,
            0, 0, 128,
            false);
    }
}

// 
void feedbackInteractionStart()
{

}