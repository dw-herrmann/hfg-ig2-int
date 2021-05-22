//
//
// ##### Joystick
//
//

#define joyX A0
#define joyY A1

class Input
{
public:
    int posDefault[2];    // startpunkt bei laden, um ungenaue Startwerte auszugleichen
    int pos[2];           // positionsdaten - arrays müssen direkt mit Inhaltsmenge angegeben werden
    int posPercentage[2]; // position als %
    int angle;            // Winkel
    int force;            // Winkel
    bool button;
};

int debug = 500;

Input input; // erstellt Objekt aus Klasse


//
//
// ##### Button
//
//

#define button D2



//
//
// ##### LEDs
//
//

#include <FastLED.h>

#define DATA_PIN 6
#define NUM_LEDS 32
#define BRIGHTNESS 32
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
// CHSV leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

void setup()
{
    Serial.begin(9600);

    //
    //
    // ##### Joystick
    //
    //

    // Eingabe-Werte Ausgangszustand feststellen
    input.posDefault[0] = analogRead(joyX);
    input.posDefault[1] = analogRead(joyY);

    //
    //
    // ##### LEDs
    //
    //

    delay(3000); // power-up safety delay

    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);

    Serial.println("––––––––––––––––––––––");
}

// Farben einstellen
void setLED(int firstLED, int amount, int color0, int color1, int color2, bool printToConsole)
{

    // Start und Ende festlegen
    int startLED = firstLED % NUM_LEDS;
    int endLED = (firstLED + amount) % NUM_LEDS;

    // Falls negativ, mach Zahl korrekt
    if (startLED <= 0)
    {
        startLED = startLED + NUM_LEDS;
    }

    if (endLED <= 0)
    {
        endLED = endLED + NUM_LEDS;
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
        for (size_t i = startLED; i < NUM_LEDS; i++)
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
            Serial.print(NUM_LEDS);
            Serial.print(" & ");
            Serial.print(0);
            Serial.print("-");
            Serial.print(endLED);
        }
    }
}

void feedbackAnimation(char feedbackType)
{
    // if (feedbackType == "buttonPress")
    // {
    //     setLED(
    //         0, NUM_LEDS,
    //         180, 255, 128,
    //         false);
    // }
}

void loop()
{
    //
    //
    // ##### Joystick
    //
    //

    // Eingabe-Werte empfangen
    input.pos[0] = analogRead(joyX);
    input.pos[1] = analogRead(joyY);

    // Werte in % umrechnen
    input.posPercentage[0] = map(input.pos[0], input.posDefault[0], 1023, 0, 100);
    input.posPercentage[1] = map(input.pos[1], input.posDefault[1], 1023, 0, 100);

    // Winkel herausfinden
    input.angle = -atan2(
                      input.posPercentage[0],
                      input.posPercentage[1]) *
                  180 / PI;

    // Krafteinwirkung aus Joystick herausfinden
    input.force = (fabs(input.posPercentage[0]) + fabs(input.posPercentage[0])) / 2;

    //
    //
    // ##### LEDs
    //
    //

    // Aktive LED aus Winkel herausfinden
    int activeLED = round(map(input.angle, -180, 180, 0, NUM_LEDS));

    // alle LEDs auf 0 stellen
    setLED(
        0, NUM_LEDS,
        180, 255, 128,
        false);

    // Ab bestimmter Krafteinwirkung aktiviere LED
    if (input.force > 2)
    {
        // nur aktive LED aktivieren, den rest deaktivieren
        setLED(
            4, 6,
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

    // LED aktualisieren
    FastLED.show();
    delay(100);

    Serial.println("");
}