#include <Arduino.h>
#include <FastLED.h>

//
//
// ##### Joystick
//
//

#define joyX A1
#define joyY A2

int timer = 0;


class Input
{
public:
    bool bottleActive = false; // Ist Flasche auf Untersetzer?

    // Joystick
    int posDefault[2];    // startpunkt bei laden, um ungenaue Startwerte auszugleichen
    int pos[2];           // positionsdaten - arrays müssen direkt mit Inhaltsmenge angegeben werden
    int posPercentage[2]; // position als %
    int angle;            // Winkel
    int active;           // Auf Winkel liegende Farbe
    int force;            // Kraft
    bool bottleTilted = false;
    bool bottleTilteState = false;

    // Buttons
    int buttonSensor[2];
    bool buttonVal[2];
    bool buttonState[2];
    int buttonOnOff[2];
};

class Output
{
public:
    int partyMode = 0;
    int partyModeColor[2];
    int mainColor = 0;
};

int debug = 500;

Input input;   // erstellt Objekt aus Klasse
Output output; // erstellt Objekt aus Klasse

//
//
// ##### Button
//
//

//
//
// ##### LEDs
//
//

#define DATA_PIN 6
#define NUM_LEDS 53
int ledInner = 21;
int ledOuter = 32;
#define BRIGHTNESS 255
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
// CHSV leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

void setup()
{
    Serial.begin(9600);

    // ##### Joystick
    //
    //
    //
    //

    // Eingabe-Werte Ausgangszustand feststellen
    input.posDefault[0] = analogRead(joyX);
    input.posDefault[1] = analogRead(joyY);

    // ##### Buttons
    //
    //
    //
    //

    input.buttonSensor[0] = 4;
    input.buttonSensor[1] = 5;

    // ##### LEDs
    //
    //
    //
    //

    output.partyModeColor[0] = 0;
    output.partyModeColor[1] = 180;

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
    input.active = round(map(input.angle, 180, -180, 0, ledOuter));

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

    // falls button gedrückt und state aus
    if (input.bottleTilted && !input.bottleTilteState)
    {
        input.bottleTilteState = true;
    }

    // falls button nicht gedrückt und state an
    else if (!input.bottleTilted && input.bottleTilteState)
    {
        input.bottleTilteState = false;
    }
}

void inputFunctionButton()
{
    for (size_t i = 0; i < 2; i++)
    {
        // read the state of the pushbutton value:
        input.buttonVal[i] = digitalRead(input.buttonSensor[i]);

        // falls button gedrückt und state aus
        if (input.buttonVal[i] && !input.buttonState[i])
        {
            input.buttonState[i] = true;
            input.buttonOnOff[i] = !input.buttonOnOff[i];
        }

        // falls button nicht gedrückt und state an
        else if (!input.buttonVal[i] && input.buttonState[i])
        {
            input.buttonState[i] = false;

            Serial.print("button ");
            Serial.print(i);
            Serial.print(" pressed");
        }
    }
}

// ##### output
//
//
//
//

class PartyModePlaceholder0
{
public:
    int currentHue = 0;
    bool currentBrightness = 0;
};

PartyModePlaceholder0 partyModePlaceholder0; // erstellt Objekt aus Klasse

class PartyModePlaceholder1
{
public:
    int currentBrightness = 0;
    bool currentSaturation = true;
};

PartyModePlaceholder1 partyModePlaceholder1; // erstellt Objekt aus Klasse

void feedbackLEDIdle()
{
    if (!input.bottleActive)
    {

        if (output.partyMode == 0)
        {
            Serial.print("mode 0");

            // Wenn Kreis durchgelaufen ist
            if (partyModePlaceholder0.currentHue >= 359)
            {
                partyModePlaceholder0.currentHue = 0;
            }

            // Stroboskop
            if (partyModePlaceholder0.currentHue / 16)
            {
                partyModePlaceholder0.currentBrightness = !partyModePlaceholder0.currentBrightness;
            }

            // Außenring
            setLED(
                1,
                0, ledOuter,
                output.mainColor, 255, partyModePlaceholder0.currentBrightness * 127,
                false);

            // Innenring
            setLED(
                0,
                0, ledInner,
                output.mainColor, 255, partyModePlaceholder0.currentBrightness * 255,
                false);

            partyModePlaceholder0.currentHue++;
            return;
        }

        if (output.partyMode == 1)
        {
            Serial.print("mode 1");

            if (partyModePlaceholder1.currentBrightness >= 255)
            {
                // Brightness Fade in
                partyModePlaceholder1.currentBrightness = 63;

                // Saturation on off
                partyModePlaceholder1.currentSaturation = !partyModePlaceholder1.currentSaturation;
            }
            partyModePlaceholder1.currentBrightness = partyModePlaceholder1.currentBrightness + 32;

            // Außenring
            setLED(
                1,
                0, ledOuter,
                output.mainColor, partyModePlaceholder1.currentSaturation * 255, partyModePlaceholder1.currentBrightness / 2,
                false);

            // Innenring
            setLED(
                0,
                0, ledInner,
                output.mainColor, partyModePlaceholder1.currentSaturation * 255, partyModePlaceholder1.currentBrightness,
                false);

            Serial.print(" B: ");
            Serial.print(partyModePlaceholder1.currentBrightness);
            Serial.print(" S: ");
            Serial.print(partyModePlaceholder1.currentSaturation);

            return;
        }
    }
}

int innerBrightness = 200;
int innerBrightnessDirection = 1;

void feedbackLEDInteractionStart()
{
    if (!input.bottleActive)
    {
        int maximum = 5;

        // Abbrechen, wenn nichts los
        if (input.buttonOnOff[0] == 0)
        {
            return;
        }

        if (input.buttonOnOff[0] == 1)
        {
            Serial.print("oh wow, feedbackLEDInteractionStart wurde aktiviert ");

            input.buttonOnOff[0]++;

            return;
        }

        // Unterm Maximum?
        if (input.buttonOnOff[0] <= maximum)
        {
            Serial.print("Die linie geht hoch! - Florentin");

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

            input.buttonOnOff[0]++;

            return;
        }

        // Bei Maximum erreicht LEDs wieder zurücksetzen
        Serial.print("Fertig");

        input.buttonOnOff[0] = 0;
        input.bottleActive = true;

        return;
    }

    if (input.buttonOnOff[0])
    {
        input.bottleActive = false;
        input.buttonOnOff[0] = 0;
    }
}

void feedbackLEDRingColor()
{
    // Wenn Flasche aktiv drauf
    if (input.bottleActive)
    {
        // Pulsieren innerer Kreis
        if (innerBrightness <= 63 or innerBrightness >= 255)
        {
            innerBrightnessDirection = innerBrightnessDirection * -1;
        }
        innerBrightness = innerBrightness + innerBrightnessDirection * 16;

        setLED(
            0,
            0, ledInner,
            output.partyModeColor[output.partyMode], 255, innerBrightness,
            false);
        
        // Farbrichtungen anzeigen
        if (!input.bottleTilted)
        {
            // Farbkreis auf LED Ring
            for (size_t i = 0; i < 8; i++)
            {
                setLED(
                    1,
                    i * 4, 4,
                    i * (360 / 8), 255, 255,
                    false);
            }

            // Innenren Ring weiß färben
            setLED(
                0,
                0, ledInner,
                0, 255, 255,
                false);

            return;
        }

        // Farbe von gewählter Richtung verstärken
        if (input.bottleTilted)
        {
            Serial.print("oh wow, die Flasche ist geneigt ");
            Serial.print(input.active);

            for (size_t i = 0; i < 8; i++)
            {
                if (round(input.active / 4) == i)
                {
                    setLED(
                        1,
                        i * 4, 4,
                        i * (360 / 8), 255, 255,
                        false);

                    output.mainColor = i * (360 / 8);
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

            // Innere LED weiß einfärben
            setLED(
                0,
                0, ledInner,
                0, 0, 64,
                false);

            // // Innere LED in Main Color einfärben
            // setLED(
            //     0,
            //     0, ledInner,
            //     output.mainColor, 255, 255,
            //     false);
        }
    }
}

void feedbackLEDPressFeedback()
{
    // Wenn Flasche aktiv drauf
    if (input.bottleActive)
    {

        int maximum = 15;

        // Abbrechen, wenn nichts los
        if (input.buttonOnOff[1] == 0)
        {
            return;
        }

        if (input.buttonOnOff[1] == 1)
        {
            Serial.print("oh wow, LEDPressFeedback wurde aktiviert ");

            input.buttonOnOff[1]++;

            return;
        }

        // Unterm Maximum?
        if (input.buttonOnOff[1] <= maximum)
        {
            Serial.print("Die linie geht hoch! - Florentin");

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

            input.buttonOnOff[1]++;

            return;
        }

        // Partymode wechseln
        if (output.partyMode >= 1)
        {
            output.partyMode = 0;
        }
        else
        {
            output.partyMode++;
        }

        // Bei Maximum erreicht LEDs wieder zurücksetzen
        Serial.print("Fertig");

        input.buttonOnOff[1] = 0;
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

    feedbackLEDIdle();
    feedbackLEDInteractionStart();
    feedbackLEDRingColor();
    feedbackLEDPressFeedback();

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

    Serial.println(timer);
    Serial.print(" ");
    timer++;
    delay(100);
}