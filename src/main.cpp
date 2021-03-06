#include <Arduino.h>
#include <FastLED.h>

int timer = 0;

class Input
{
public:
  bool bottleActive = false; // Ist Flasche auf Untersetzer?
  int selectMode = 0;        // Farbe oder modus

  // Velo
  // - Mux control pins
  int s0 = 8;
  int s1 = 9;
  int s2 = 10;
  int s3 = 11;

  // - Mux in "SIG" pin
  int SIG_pin = 0;
  int controlPin[4] = {s0, s1, s2, s3};
  int muxChannel[16][4] = {
      {0, 0, 0, 0}, //channel 0
      {1, 0, 0, 0}, //channel 1
      {0, 1, 0, 0}, //channel 2
      {1, 1, 0, 0}, //channel 3
      {0, 0, 1, 0}, //channel 4
      {1, 0, 1, 0}, //channel 5
      {0, 1, 1, 0}, //channel 6
      {1, 1, 1, 0}, //channel 7
      {0, 0, 0, 1}, //channel 8
      {1, 0, 0, 1}, //channel 9
      {0, 1, 0, 1}, //channel 10
      {1, 1, 0, 1}, //channel 11
      {0, 0, 1, 1}, //channel 12
      {1, 0, 1, 1}, //channel 13
      {0, 1, 1, 1}, //channel 14
      {1, 1, 1, 1}  //channel 15
  };

  int veloCount = 8;
  const String veloCables[8] = {
      "orange",
      "  gelb",
      " gruen",
      "  blau",
      "violet",
      "  grau",
      " weiss",
      " schwarz"};

  // zum Flatten
  int analogValuesOriginal[8][5];

  // jetzt (flattened), min, max, percentage, direction
  int analogValues[8][5] = {
      {0, 1023, 0, 0, 0}, // direction 0
      {0, 1023, 0, 0, 0}, // direction 1
      {0, 1023, 0, 0, 0}, // direction 2
      {0, 1023, 0, 0, 0}, // direction 3
      {0, 1023, 0, 0, 0}, // direction 4
      {0, 1023, 0, 0, 0}, // direction 5
      {0, 1023, 0, 0, 0}, // direction 6
      {0, 1023, 0, 0, 0}, // direction 7
  };

  // V1: 817, 725, 579, 419, 424, 666, 584, 676

  // value, index
  int analogValuesSorted[8][2];

  int averagePercentage = 0;
  int analogPressedState = false;
  int veloPressed[3] = {
      0, // Flasche drauf gestellt
      0, // Flasche gedrückt
      0  // Flasche heruntergenommen
  };

  // Velo
  int active;     // Auf Winkel liegende Farbe
  int lastActive; // zuletzt ausgewählte Farbe
  bool bottleTilted = false;
  int bottleTiltCountDown = 0;
};

class Output
{
public:
  int partyMode = 0;
  int partyModeColor[4] = {180, 90, 0, 270};
  int mainColor = 0;
};

int debug = 500;

Input input;   // erstellt Objekt aus Klasse
Output output; // erstellt Objekt aus Klasse

// ##### LEDs
//
//
//
//

#define DATA_PIN 6    // LED Steuerung kommt in Pin 6
int ledInner = 20;    // 21 LEDs für den Inneren
int ledOuter = 32;    // 32 LEDs für den Äußeren
int ledStrip = 239;   // X LEDs für LED Kette
#define NUM_LEDS 291  // Gesamtanzahl an LEDs
#define BRIGHTNESS 16 // Helligkeit des Rings

#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100

void setup()
{
  Serial.begin(9600);

  // Velo
  //
  //
  //
  //

  pinMode(input.s0, OUTPUT);
  pinMode(input.s1, OUTPUT);
  pinMode(input.s2, OUTPUT);
  pinMode(input.s3, OUTPUT);

  digitalWrite(input.s0, LOW);
  digitalWrite(input.s1, LOW);
  digitalWrite(input.s2, LOW);
  digitalWrite(input.s3, LOW);

  // ##### LEDs
  //
  //
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

// ##### input
//
//
//
//

void inputFunctionReadVelo()
{
  for (int i = 0; i < input.veloCount; i++)
  {
    // read out analog values
    //
    //
    //
    //

    //loop through the 4 sig
    for (int ii = 0; ii < 4; ii++)
    {
      digitalWrite(input.controlPin[ii], input.muxChannel[i][ii]);
    }

    // flatten analog value
    //
    //
    //
    //
    for (size_t ii = 2; ii > 0; ii--)

    {
      // push values one backwards
      input.analogValuesOriginal[i][ii] = input.analogValuesOriginal[i][ii - 1];
    }
    input.analogValuesOriginal[i][0] = analogRead(input.SIG_pin);

    input.analogValues[i][0] = (input.analogValuesOriginal[i][0] * 10 +
                                input.analogValuesOriginal[i][1] * 5) /
                               15;

    // get percentage value
    //
    //
    //
    //

    // niedrigsten Wert finden
    if (input.analogValues[i][0] < input.analogValues[i][1])
    {
      input.analogValues[i][1] = input.analogValues[i][0];
    }

    // höchsten Wert finden
    if (input.analogValues[i][0] > input.analogValues[i][2])
    {
      input.analogValues[i][2] = input.analogValues[i][0];
    }

    input.analogValues[i][3] = map(input.analogValues[i][0], input.analogValues[i][1], input.analogValues[i][2], 0, 100);
  }

  // flatten direction value
  //
  //
  // get average of both neighbours and this
  //

  for (int i = 0; i < input.veloCount; i++)
  {
    int newValue = 0;

    // Element selbst
    newValue += input.analogValues[i][3] * 10;

    // Nachbar davor
    if (i == 0)
    {
      newValue += input.analogValues[input.veloCount][3] * 7;
    }
    else
    {
      newValue += input.analogValues[i - 1][3] * 7; // 1 vor anfang
    }

    // Nachbar danach
    if (i == input.veloCount - 1) // ende von Kreis
    {
      newValue += input.analogValues[input.veloCount][3] * 7;
    }
    else
    {
      newValue += input.analogValues[i + 1][3] * 7;
    }

    input.analogValues[i][4] = newValue / 24;
  }

  // find highest value
  //
  //
  //
  //

  int highestValue = 0;
  int highestPos = 0;

  // Höchsten Wert finden
  for (size_t i = 0; i < input.veloCount; i++)
  {
    if (input.analogValues[i][3] > highestValue)
    {
      highestValue = input.analogValues[i][3];
    }
  }

  // Position von höchstem Wert finden
  for (size_t i = 0; i < input.veloCount; i++)
  {
    if (highestValue == input.analogValues[i][3])
    {
      highestPos = i;
    }
  }

  input.active = highestPos;

  input.averagePercentage = (input.analogValues[0][3] + input.analogValues[1][3] + input.analogValues[2][3] + input.analogValues[3][3] + input.analogValues[4][3] + input.analogValues[5][3] + input.analogValues[6][3] + input.analogValues[7][3]) / input.veloCount;

  // Druck interpretieren
  //
  //
  //
  //

  int arr[input.veloCount];
  int alreadyUsed[input.veloCount];

  for (size_t i = 0; i < input.veloCount; i++) // arrays kopieren
  {
    arr[i] = input.analogValues[i][4];
    alreadyUsed[i] = input.analogValues[i][4];
  }

  // tilt von Flasche erkennen
  for (int j = 0; j < input.veloCount - 1; j++) // Sortierfunktion
  {

    if (arr[j] > arr[j + 1])
    {
      int temp = arr[j];
      arr[j] = arr[j + 1];
      arr[j + 1] = temp;

      j = -1;
    }
  }

  for (size_t i = 0; i < input.veloCount; i++) // Werte und Positionen speichern
  {
    input.analogValuesSorted[i][0] = arr[i];

    for (size_t ii = 0; ii < input.veloCount; ii++) // Position von Werten finden
    {
      if (arr[i] == alreadyUsed[ii])
      {
        input.analogValuesSorted[i][1] = ii;
        alreadyUsed[ii] = -1;
        break;
      }
    }
  }

  if (input.analogValuesSorted[7][0] - input.analogValuesSorted[1][0] > 25) // wenn Unterschied zwischen neigerichtung und anderer Seite über 30
  {
    int sides[2] = {
        (input.analogValuesSorted[7][1] - input.analogValuesSorted[0][1]) % 4,
        (input.analogValuesSorted[7][1] - input.analogValuesSorted[1][1]) % 4,
    };

    if (fabs(sides[0]) <= 0 or fabs(sides[1]) <= 0) // Wenn einer von beiden Punkten genau gegenüber
    {
      input.bottleTilted = true;
      input.bottleTiltCountDown = 15;
      input.lastActive = input.active;
    }
    else
    {
      input.bottleTilted = false;
    }
  }
  else
  {
    input.bottleTilted = false;
  }

  // tilt countdown für unabsichtliches nicht tilten
  if (input.bottleTiltCountDown != 0 && input.averagePercentage < 35)
  {
    // solange nicht abgelaufen, behalte vorherigen State bei
    input.bottleTilted = true;
    input.bottleActive = true;
    input.active = input.lastActive;
    input.bottleTiltCountDown--;
  }
  else // nur nach flasche drauf und Press checken, wenn nicht getiltet
  {
    // draufstehen checken
    if (input.averagePercentage > 15) // über 12, Flasche drauf
    {
      if (!input.bottleActive && !input.veloPressed[0])
      {
        // nur beim frisch drauf stellen
        input.veloPressed[0] = 1;
        Serial.print("einmal ausgelöst");
      }
    }
    else
    {
      input.bottleActive = false;
    }

    // draufpressen checken
    if (input.averagePercentage > 35) // über 30, flasche gedrückt
    {
      if (input.bottleActive && input.veloPressed[0] == 0)
      {
        // nur wenn noch nicht ausgelöst
        input.veloPressed[1] = 1;
      }
    }
  }
}

// ##### output
//
//
//
//

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
      // Serial.print("mode 0");

      // Innenring
      setLED(
          0,
          0, ledInner,
          output.mainColor, 255, round(timer / 3 % 2) * 127 + 128,
          false);

      // Außenring
      setLED(
          1,
          0, ledOuter,
          output.mainColor, 255, round(timer / 3 % 2) * 127 + 128,
          false);

      // Strip
      setLED(
          3,
          0, ledStrip,
          output.mainColor, 255, round(timer / 3 % 2) * 127 + 128,
          false);

      return;
    }

    if (output.partyMode == 1)
    {
      // Innenring
      setLED(
          0,
          0, ledInner,
          output.mainColor, (timer + 16) / 16 % 2 * 255, 255,
          // output.mainColor, (timer + 16 ) / 16 % 2 * 255, timer + 16 % 16 * 12 + 64,
          false);

      // Außenring
      setLED(
          1,
          0, ledOuter,
          output.mainColor, timer / 16 % 2 * 255, timer % 16 * 12 + 64,
          false);

      // Strip
      setLED(
          3,
          0, ledStrip,
          output.mainColor, timer / 16 % 2 * 255, timer % 16 * 12 + 64,
          false);

      return;
    }

    if (output.partyMode == 2)
    {
      // Serial.print("mode 2");

      int m2colors[3] = {
          output.mainColor,
          (output.mainColor + 120) % 359,
          (output.mainColor + 240) % 359};

      // Innenring
      setLED(
          0,
          0, ledInner,
          // 3 Farbwechseln pro Umdrehung,
          m2colors[timer / 32 % 3], 255, (timer * ledOuter) % 255,
          false);

      Serial.print(" ");
      Serial.print(timer / 32 % 3);
      Serial.print(" ");

      // Außenring
      setLED(
          1,
          timer % ledOuter, ledOuter / 3,
          m2colors[0], 255, 255,
          false);

      setLED(
          1,
          (timer % ledOuter) + ledOuter / 3, ledOuter / 3,
          m2colors[1], 255, 255,
          false);

      setLED(
          1,
          (timer % ledOuter) + ledOuter / 3 * 2, ledOuter / 3,
          m2colors[2], 255, 255,
          false);

      return;
    }

    if (output.partyMode == 3)
    {
      // Serial.print("mode 3");

      int m2colors[3] = {
          output.mainColor,
          (output.mainColor + 120) % 359,
          (output.mainColor + 240) % 359};

      // Innenring
      setLED(
          0,
          0, ledInner,
          output.mainColor, partyModePlaceholder1.currentSaturation * 255, partyModePlaceholder1.currentBrightness,
          false);

      // Außenring
      setLED(
          1,
          timer % ledOuter, ledOuter / 3,
          m2colors[0], 255, 255,
          false);

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
    int maximum = 10;

    Serial.print(input.veloPressed[0]);
    // Abbrechen, wenn nichts los
    if (input.veloPressed[0] == 0)
    {
      return;
    }

    if (input.veloPressed[0] == 1)
    {
      Serial.print("f jetzt drauf");

      input.veloPressed[0]++;

      return;
    }

    // Unterm Maximum?
    if (input.veloPressed[0] <= maximum)
    {
      Serial.print("f drauf");

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

      // Überarbeitung Dominik: Hauptfarbe auf Ring und Modus auf Mitte
      setLED(
          0,
          0, ledInner,
          output.partyModeColor[output.partyMode], 255, 255,
          false);

      input.veloPressed[0]++;

      return;
    }

    // Überarbeitung Dominik: Hauptfarbe auf Ring und Modus auf Mitte
    setLED(
        0,
        0, ledInner,
        output.partyModeColor[output.partyMode], 255, 255,
        false);

    // Bei Maximum erreicht LEDs wieder zurücksetzen
    Serial.print("Fertig");

    input.veloPressed[0] = 0;
    input.bottleActive = true;

    return;
  }

  if (input.veloPressed[0])
  {
    input.bottleActive = false;
    input.veloPressed[0] = 0;
  }
}

void feedbackLEDRingSelect()
{
  // Wenn Flasche aktiv drauf
  if (input.bottleActive)
  {
    // Wird aktuell Farbe ausgewählt oder …
    if (input.selectMode == 0)
    {
      // Farbe von gewählter Richtung verstärken
      if (input.bottleTilted)
      { // Falls Flasche gekippt
        // ledOuter -> Farbkreis
        // ledInner -> gekippte Farbe

        for (size_t i = 0; i < input.veloCount; i++)
        {
          if (input.active == i)
          {
            setLED(
                1,
                i * 4, 4,
                i * (360 / input.veloCount), 255, 255,
                false);

            output.mainColor = i * (360 / input.veloCount);
          }
          else
          {
            setLED(
                1,
                i * 4, 4,
                i * (360 / input.veloCount), 255, 100,
                false);
          }
        }

        // Überarbeitung Dominik: Bei Farbe einstellen Innere LED in Main Color einfärben
        setLED(
            0,
            0, ledInner,
            output.mainColor, 255, 127,
            false);

        return;
      } // falls nicht:

      // ledInner -> grau
      // ledOuter -> Farbkreis
      setLED(
          0,
          0, ledInner,
          0, 0, 127,
          false);

      for (size_t i = 0; i < input.veloCount; i++)
      {
        setLED(
            1,
            i * 4, 4,
            i * (360 / input.veloCount), 255, 255,
            false);
      }

      return;
    }

    if (input.selectMode == 1)
    {

      // Abstandsfarbe
      setLED(
          1,
          0, ledOuter,
          0, 255, 255,
          false);

      // Füllfarbe
      for (size_t i = 0; i < 4; i++)
      {
        setLED(
            1,
            (i * input.veloCount) + 1, (ledOuter / 4) - 2,
            0, 0, 191,
            false);
      }

      // innerer Ring
      setLED(
          0,
          0, ledInner,
          0, 0, 127,
          false);

      // im Kipp-Modus
      if (input.bottleTilted)
      {
        Serial.print("\txyz ");
        Serial.print(float(floor(input.active / 2)));

        for (size_t i = 0; i < 4; i++)
        {
          // färbe entsprechende Richtung ein
          if (float(floor(input.active / 2)) == i)
          {
            setLED(
                1,
                (i * input.veloCount) + 1, (ledOuter / 4) - 2,
                output.mainColor, 255, 255,
                false);
          }
        }
      }

      return;

      // 4 Richtungen für Modi zeigen
      if (input.bottleTilted)
      { // Falls Flasche gekippt

        for (size_t i = 0; i < 4; i++)
        {
          if (input.active / (ledOuter / 4) == i)
          {
            setLED(
                1,
                (i * input.veloCount) + 1, (ledOuter / 4) - 2,
                output.mainColor, 255, 255,
                false);

            output.partyMode = i;
          }
          else
          {
            setLED(
                1,
                (i * input.veloCount) + 1, (ledOuter / 4) - 2,
                0, 0, 127,
                false);
          }
        }

        return;
      } // falls nicht:

      // ledInner -> grau
      // ledOuter -> Farbkreis
      setLED(
          0,
          0, ledInner,
          0, 0, 125,
          false);

      return;
    }
  }
}

void feedbackLEDPressFeedback()
{
  // Wenn Flasche aktiv drauf
  if (input.bottleActive)
  {

    int maximum = 5;

    // Abbrechen, wenn nichts los
    if (input.veloPressed[1] == 0)
    {
      return;
    }

    if (input.veloPressed[1] == 1)
    {
      Serial.print("f jetzt gedrückt ");

      input.veloPressed[1]++;

      return;
    }

    // Unterm Maximum?
    if (input.veloPressed[1] <= maximum)
    {
      Serial.print("f gedrückt ");

      // ledOuter -> weiß
      // ledInner -> weiß

      // alle LEDs aufläuchten lassen
      setLED(
          0,
          0, ledInner,
          0, 0, 127,
          false);

      // Weißer außenring
      setLED(
          1,
          0, ledOuter,
          0, 0, 255,
          false);

      input.veloPressed[1]++;

      return;
    }

    // Bei Maximum erreicht LEDs wieder zurücksetzen
    Serial.print("Fertig");
    input.selectMode = !input.selectMode;

    input.veloPressed[1] = 0;
  }
}

void serialOutput()
{
  // Velo log
  for (int i = 0; i < input.veloCount; i++)
  {
    if (true) // cable and values
    {
      Serial.print(input.veloCables[i]);
      Serial.print("  ");

      // Serial.print(i);
      // Serial.print(": ");
      // Serial.print(input.analogValues[i][1]); // min
      // Serial.print(" ");
      // Serial.print(input.analogValues[i][2]); // max
      // Serial.print(" ");
      // Serial.print(input.analogValuesOriginal[i][0]); // original values pure
      // Serial.print(" ");
      // Serial.print(input.analogValues[i][0]); // original values flattened
      // Serial.print(input.analogValues[i][3]); // percentage
      Serial.print(" ");
      Serial.print(input.analogValues[i][4]); // percentage flattened (direction)

      Serial.print("\t");
    }

    if (false) // sorted
    {
      for (size_t i = 0; i < input.veloCount; i++)
      {
        Serial.print(input.analogValuesSorted[i][0]);
        Serial.print(" = ");
        Serial.print(input.analogValuesSorted[i][1]);
        Serial.print("\t");
      }
    }
  }

  if (false) // lowest values
  {
    Serial.print("\tlow: ");
    Serial.print(input.analogValuesSorted[0][0]);
    Serial.print("/");
    Serial.print(input.analogValuesSorted[0][1]);
    Serial.print(" & ");
    Serial.print(input.analogValuesSorted[1][0]);
    Serial.print("/");
    Serial.print(input.analogValuesSorted[1][1]);
    Serial.print("\t");
  }

  if (false) // highest values
  {
    Serial.print("hig: ");
    Serial.print(input.analogValuesSorted[7][0]);
    Serial.print("/");
    Serial.print(input.analogValuesSorted[7][1]);
    Serial.print("\t");
  }

  if (false) // dif
  {
    Serial.print("dif: ");
    Serial.print(input.analogValuesSorted[7][0] - input.analogValuesSorted[0][0]);
    Serial.print("/");
    Serial.print((input.analogValuesSorted[7][1] - input.analogValuesSorted[0][1]) % 4);
    Serial.print(" & ");
    Serial.print(input.analogValuesSorted[7][0] - input.analogValuesSorted[1][0]);
    Serial.print("/");
    Serial.print((input.analogValuesSorted[7][1] - input.analogValuesSorted[1][1]) % 4);
    Serial.print("   \t");
  }

  if (false) // pressure 
  {
    Serial.print(" avg: ");
    Serial.print(input.averagePercentage);
    Serial.print("   \t");
  }

  if (true) // state feedback
  {
    // Serial.print("act: ");
    Serial.print(input.bottleActive);
    Serial.print("\t");

    // Serial.print("til: ");
    Serial.print(input.bottleTilted);
    Serial.print("\t");

    // Serial.print("dir: ");
    Serial.print(input.active);
    Serial.print("\t");
  }

  if (false) // tilt values 
  {
    Serial.print("tcnt: ");
    Serial.print(input.bottleTiltCountDown);
    Serial.print("\t");
  }

  if (false) // flattening values
  {
    for (size_t i = 0; i < 5; i++)
    {
      Serial.print(input.analogValuesOriginal[0][i]);
      Serial.print("\t");
    }

    Serial.print("|||||\t");

    for (size_t i = 0; i < 5; i++)
    {
      Serial.print(input.analogValuesOriginal[3][i]);
      Serial.print("\t");
    }
  }
}

void loop()
{
  inputFunctionReadVelo();

  serialOutput();

  // ##### LEDs
  //
  //
  //
  //

  feedbackLEDIdle();
  feedbackLEDRingSelect();

  feedbackLEDInteractionStart();
  feedbackLEDPressFeedback();

  // LED aktualisieren
  FastLED.show();

  Serial.println("");
  // Serial.print(timer);
  timer++;
  delay(100);
}