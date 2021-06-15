// Library einbinden
#include <FastLED.h>

#define DATA_PIN 6    // LED Steuerung kommt in Pin 6
int ledInner = 21;    // 21 LEDs für den Inneren
int ledOuter = 32;    // 32 LEDs für den Äußeren
int ledStrip = 10;    // X LEDs für LED Kette
#define NUM_LEDS 63   // Gesamtanzahl an LEDs ausrechnen
#define BRIGHTNESS 32 // Helligkeit des Rings

// >> ignorieren <<
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100

// Durchlauf des loops, wird pro loop eins hochgezählt
// Wichtig für Animationen und Co
int timer = 0;

// Funktion, um auf LED-Ringen und Strip Lichter einzustellen.
// !!!Nicht ändern!! Nur verwenden
void setLED(int ledRange, int firstLED, int amount, int color0, int color1, int color2, bool printToConsole)
{

    int maxLEDs;    // Gesamtanzahl der LEDs von ausgewählter ledRange
    int addToI = 0; // erste LED von ausgewählter ledRange

    if (ledRange == 0) // innerer LED Ring
    {
        maxLEDs = ledInner;
    }
    else if (ledRange == 1) // äußerer LED Ring
    {
        addToI = ledInner;
        maxLEDs = ledOuter;
    }
    else if (ledRange == 2) // beide LED Ringe
    {
        maxLEDs = ledInner + ledOuter;
    }
    else if (ledRange == 3) // LED Strip
    {
        addToI = ledInner + ledOuter;
        maxLEDs = ledInner + ledOuter + ledStrip;
    }

    // Start und Ende festlegen
    int startLED = firstLED % maxLEDs;
    int endLED = (firstLED + amount) % maxLEDs;

    // Falls negativ, mach Zahl korrekt
    if (startLED < 0)
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
            leds[i + addToI].setHSV(color0, color1, color2);
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
            leds[i + addToI].setHSV(color0, color1, color2);
        }

        // … und alle restlichen von 0 bis Rest einfärben
        for (size_t i = 0; i < endLED; i++)
        {
            leds[i + addToI].setHSV(color0, color1, color2);
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

void setup()
{
    Serial.begin(9600); // Serial Monitor für Logs starten

    // Setup der LED-Ringe und Ketten
    delay(3000); // power-up safety delay
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);

    Serial.println("––––––––––––––––––––––"); // Feedback im Log, dass Arduino jetzt Modi anzeigt
}

//
//
//
//
//
//
//
//
//
//
//
//
//

/*
@Team, hier könnt ihr eure Modi programmieren.

Ein Modus besteht aus 
- einer Klasse    // erstellt dort alle Variablen, Arrays, Counter usw und ruft sie dann über mode0.beispiel1 oder mode0.beispiel2[0]
- einer Funktion  // Dort könnt ihr hineinschreiben, wie sich die LEDs verhalten sollen.

!!! Wichtig !!!
    Der Loop hat einen Takt von 100ms, also 10 wechseln pro Sekunde. 
    Viele Rythmen lassen sich bereits mit dem Taktschlag (variable namens "timer") und mit Modulo (%) lösen. Beispielsweise ein Rythmus von an/aus alle 500 ms würde so funktionieren:


        if(timer % 5 == 0) {                            // Wenn der Takt durch 5 Teilbar ist, also alle 5 Takte, …
            mode0.lightToggle = !mode0.lightToggle;     // … dann wechsel Lichtstatus zwischen 1 und 0
        }

        setLED(
            0,
            0, ledInner,
            0, 0, mode0.lightToggle * 255,              // mode0.lightToggle * 255 bedeutet entweder "0 * 255"(aus) oder "1 * 255"(an)
            false);


!!! Wichtig !!!
*/

int mainHue = 60; // @Team, hier könnt ihr zum ausprobieren ein Hue eintragen. Also aus HSL zwischen 0 und 359

class Mode0
{
public:
    int beispiel1 = 5;
    int beispiel2[4] = {30, 90, 70, 40};
    int lightToggle = 0;
    int fadeToggle = 1;
    int brightness = 1;
};

Mode0 mode0; // erstellt Objekt aus Klasse

void showMode0()
{
    //
    //
    // An/Aus Blinken auf innerem Ring
    if (timer % 5 == 0)
    {                                           // Wenn der Takt durch 5 Teilbar ist, also alle 5 Takte, …
        mode0.lightToggle = !mode0.lightToggle; // … dann wechsel Lichtstatus zwischen 1 und 0
    }

    setLED(
        0,
        0, ledInner,
        0, 0, mode0.lightToggle * 255, // mode0.lightToggle * 255 bedeutet entweder "0 * 255"(aus) oder "1 * 255"(an)
        false);

    //
    //
    // abwechselndes Blinken zwischen mainHue und weiß
    setLED(
        1,
        0, ledOuter,
        mainHue, mode0.lightToggle * 255, 255, // mainHue ist die vom Nutzer ausgewählte Farbe
        false);

    //
    //
    // Ein- und Ausfade-Animation
    mode0.brightness = (timer % (255 / 16)) * 16; // Zählt brightness in 16 stufen hoch

    setLED(
        3,
        5, 3,
        0, 0, mode0.brightness,
        false);
}

void showModes(int activeMode = 0)
{
    if (activeMode == 0)
    {
        showMode0();
        Serial.print(" ");
        Serial.print(activeMode);
        Serial.print(" ");
    }
}

//
//
//
//
//
//
//
//
//
//
//
//
//

void loop()
{
    // Funktion zeigt deinen Modus an
    showModes(0); // @Team, hier einfach die 0 gegen euren Modus wechseln

    // LED aktualisieren
    FastLED.show();

    Serial.println("");
    Serial.print(timer);
    Serial.print(" ");
    timer++;
    delay(100);
}