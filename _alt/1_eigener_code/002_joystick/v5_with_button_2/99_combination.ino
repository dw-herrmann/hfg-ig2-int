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
    int buttonSensor;
    int buttonState;
};

int debug = 500;

Input input; // erstellt Objekt aus Klasse

//
//
// ##### Button
//
//

#define button 2

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
// Farben einstellen
void setLED(int firstLED, int amount, int color0, int color1, int color2, bool printToConsole)
{

    int maxLEDs = NUM_LEDS;

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

void inputFunctionJoystick()
{
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
}

void inputFunctionButton()
{
    // Button gepresst oder nicht
    input.buttonSensor = digitalRead(button);
    Serial.print("sensorVal: ");
    Serial.println(input.buttonSensor);
    Serial.print("ledState: ");
    Serial.println(input.buttonState);

    // falls button gedrückt und state aus
    if (input.buttonSensor == 1 && input.buttonState == 0)
    {
        input.buttonState = 1;
        Serial.print("button pressed");
    }

    // falls button nicht gedrückt und state an
    else if (input.buttonSensor == 0 && input.buttonState == 1)
    {
        input.buttonState = 0;
    }
}

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
void loop()
{
    //
    //
    // ##### Joystick
    //
    //

    inputFunctionJoystick();

    inputFunctionButton();

    //
    //
    // ##### LEDs
    //
    //

    feedbackDirection();

    feedbackInteractionStart();

    // LED aktualisieren
    FastLED.show();

    //print the values with to plot or view
    // Serial.print("x:");
    // Serial.print(input.posPercentage[0]);
    // Serial.print("\t y:");
    // Serial.print(input.posPercentage[1]);
    // Serial.print("%\t a:");
    // Serial.print(input.angle);
    // Serial.print("°\t f:");
    // Serial.print(input.force);

    Serial.println("");
    delay(100);
}