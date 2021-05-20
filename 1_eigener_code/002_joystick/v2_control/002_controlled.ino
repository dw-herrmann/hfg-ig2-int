// Joystick
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
};

int debug = 500;

Input input; // erstellt Objekt aus Klasse

// LEDs
#include <FastLED.h>

#define DATA_PIN 6
#define NUM_LEDS 32
#define BRIGHTNESS 32
#define LED_TYPE WS2811
#define COLOR_ORDER RBG
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

void setup()
{
    Serial.begin(9600);
    Serial.println("test");

// Joystick
    // Eingabe-Werte Ausgangszustand feststellen
    input.posDefault[0] = analogRead(joyX);
    input.posDefault[1] = analogRead(joyY);

// LEDs
    delay(3000); // power-up safety delay

    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
}

void loop()
{
// Joystick
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

    //print the values with to plot or view
    Serial.print("x:");
    Serial.print(input.posPercentage[0]);
    Serial.print("\t y:");
    Serial.print(input.posPercentage[1]);
    Serial.print("%\t a:");
    Serial.print(input.angle);
    Serial.print("°\t f:");
    Serial.print(input.force);
    Serial.println("");

// LEDs
    // Aktive LED aus Winkel herausfinden
    int activeLED = round(map(input.angle, -180, 180, 0, NUM_LEDS - 1));

    // Ab bestimmter Krafteinwirkung aktiviere LED
    if (input.force > 10)
    {
        for (size_t i = 0; i < NUM_LEDS; i++)
        {
            // nur aktive LED aktivieren, den rest deaktivieren
            if (i == activeLED)
            {
                leds[activeLED] = CRGB::Yellow;
            }
            else
            {
                leds[i] = CRGB::Black;
            }
        }
    }
    else
    {
        // Alle LEDs deaktivieren
        for (size_t i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB::Black;
        }
    }

    // LED aktualisieren
    FastLED.show();
    delay(30);
}