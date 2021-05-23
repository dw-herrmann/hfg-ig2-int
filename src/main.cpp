#include <Arduino.h>

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
    int active;           // Auf Winkel liegende Farbe
    int force;            // Kraft
    bool bottleTilted;
    bool bottleActive = true; // debug
    bool buttonSensor;
};

int debug = 500;

Input input; // erstellt Objekt aus Klasse

//
//
// ##### Button
//
//

#define button 12

//
//
// ##### LEDs
//
//

#include <FastLED.h>

#define DATA_PIN 6
#define NUM_LEDS 40
int ledInner = 8;
int ledOuter = 32;
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
void setLED(int ledRing, int firstLED, int amount, int color0, int color1, int color2, bool printToConsole)
{
    if (ledRing == 0)
    {
        /* code */

        int maxLEDs = ledInner;

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
    else
    {
        int maxLEDs = ledOuter;

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
                leds[i + ledInner].setHSV(color0, color1, color2);
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
                leds[i + ledInner].setHSV(color0, color1, color2);
            }

            // … und alle restlichen von 0 bis Rest einfärben
            for (size_t i = 0; i < endLED; i++)
            {
                leds[i + ledInner].setHSV(color0, color1, color2);
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
}

// ##### input
//
//
//
//

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

    // Aktive LED aus Winkel herausfinden
    input.active = round(map(input.angle, -180, 180, 0, ledOuter));

    // Krafteinwirkung aus Joystick herausfinden
    input.force = (fabs(input.posPercentage[0]) + fabs(input.posPercentage[1])) / 2;
    if (input.force > 20)
    {
        input.bottleTilted = true;
    }
    else
    {
        input.bottleTilted = false;
    }
}

void inputFunctionButton()
{
    // Button gepresst oder nicht
    input.buttonSensor = digitalRead(button);
    Serial.print("sensorVal: ");
    Serial.println(input.buttonSensor);
    Serial.print("ledState: ");
    Serial.println(input.buttonState);

    Serial.print(input.buttonSensor);

    // falls button nicht gedrückt und state an
    else if (input.buttonSensor == 0 && input.buttonState == 1)
    {
        input.buttonState = 0;
    }

//
//
// ##### output
//
//

int feedbackLEDPressFeedbackVar = 0;
CRGB feedbackLEDPressFeedbackTemp[40];
void feedbackLEDPressFeedback()
{
    int maximum = 15;

    // Abbrechen, wenn nichts los
    if (feedbackLEDPressFeedbackVar == 0)
    {
        return;
    }

    if (feedbackLEDPressFeedbackVar == 1)
    {
        Serial.print("oh wow, LEDPressFeedback wurde aktiviert ");

        // Zwischenstand speichern
        for (size_t i = 0; i < sizeof(leds); i++)
        {
            feedbackLEDPressFeedbackTemp[i] = leds[i];
        }

        // alle LEDs aufläuchten lassen
        setLED(
            0,
            0, ledInner,
            0, 0, 255,
            false);

        setLED(
            1,
            0, ledOuter,
            0, 0, 255,
            false);

        feedbackLEDPressFeedbackVar++;

        return;
    }

    // Unterm Maximum?
    if (feedbackLEDPressFeedbackVar <= maximum)
    {
        Serial.print("Die linie geht hoch! - Florentin");
        feedbackLEDPressFeedbackVar++;

        return;
    }

    // Bei Maximum erreicht LEDs wieder zurücksetzen
    Serial.print("Fertig");
    setLED(
        0,
        0, ledInner,
        0, 0, 0,
        false);

    setLED(
        1,
        0, ledOuter,
        0, 0, 0,
        false);

    feedbackLEDPressFeedbackVar = 0;

    // // Zustand zurücksetzen
    // for (size_t i = 0; i < sizeof(leds); i++)
    // {
    //     leds[i] = feedbackLEDPressFeedbackTemp[i];
    // }
}

CRGB feedbackLEDRingColorTemp[40];
void feedbackLEDRingColor()
{
    // Abbrechen, wenn nichts los
    if (!input.bottleTilted)
    {
        return;
    }

    if (input.bottleTilted)
    {
        Serial.print("oh wow, der FarbKreis aktiviert ");
        Serial.print(input.active);

        setLED(
            0,
            0, ledInner,
            0, 0, 255,
            false);

        for (size_t i = 0; i < 8; i++)
        {
            if (round(input.active / 4) == i)
            {
                setLED(
                    1,
                    i * 4, 4,
                    i * (360 / 8), 255, 255,
                    false);
            }
            else
            {
                setLED(
                    1,
                    i * 4, 4,
                    i * (360 / 8), 255, 100,
                    false);
            }
        }
    }
}

void loop()
{
    // ##### Joystick
    //
    //
    //
    //

    inputFunctionJoystick();

    inputFunctionButton();

    // ##### LEDs
    //
    //
    //
    //

    feedbackLEDPressFeedback();
    feedbackLEDRingColor();

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