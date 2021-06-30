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
      " weiss",
      "  grau",
      " braun"};

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

  // zum Flatten
  int analogValuesOriginal[8][5] = {
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0}};

  int averagePercentage = 0;
  int analogPressedState = false;
  int veloPressed[2] = {
      0, // Flasche drauf gestellt
      0, // Flasche gedrückt
      0  // Flasche heruntergenommen
  };

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
  int buttonSensor[2] = {4, 5};
  bool buttonVal[2];
  bool buttonState[2];
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

// ##### Button
//
//
//
//

// ##### LEDs
//
//
//
//

#define DATA_PIN 6    // LED Steuerung kommt in Pin 6
int ledInner = 21;    // 21 LEDs für den Inneren
int ledOuter = 32;    // 32 LEDs für den Äußeren
int ledStrip = 10;    // X LEDs für LED Kette
#define NUM_LEDS 63   // Gesamtanzahl an LEDs ausrechnen
#define BRIGHTNESS 32 // Helligkeit des Rings

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
      input.veloPressed[i] = !input.veloPressed[i];
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

    for (size_t ii = 5; ii > 0; ii--)
    {
      // push values one backwards
      input.analogValuesOriginal[i][ii] = input.analogValuesOriginal[i][ii - 1];
    }
    input.analogValuesOriginal[i][0] = analogRead(input.SIG_pin);

    input.analogValues[i][0] = (input.analogValuesOriginal[i][0] * 10 +
                                input.analogValuesOriginal[i][1] * 4 +
                                input.analogValuesOriginal[i][2] * 3 +
                                input.analogValuesOriginal[i][3] * 2 +
                                input.analogValuesOriginal[i][4] * 1) /
                               20;

    // get percentage value
    //
    //
    //
    //

    // niedrigsten Wert finden
    if (input.analogValues[i][0] < input.analogValues[i][1] && input.analogValues[i][0] != 0)
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

  if (input.averagePercentage > 12) // über 12, Flasche drauf
  {
    // Serial.print("i.vP[0] = ");
    // Serial.print(input.veloPressed[0]);
    // Serial.print("\t");

    if (!input.bottleActive && !input.veloPressed[0])
    {
      // nur beim frisch drauf stellen
      input.veloPressed[0] = 1;
      Serial.print("einmal ausgeläst");
    }
  }
  else
  {
    input.bottleActive = false;
  }

  if (input.averagePercentage > 35) // über 30, flasche gedrückt
  {
    if (input.bottleActive && input.veloPressed[0] == 0)
    {
      // nur wenn noch nicht ausgelöst
      input.veloPressed[1] = 1;
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

    if (output.partyMode == 1)
    {
      // Serial.print("mode 0");

      // Außenring
      setLED(
          1,
          0, ledOuter,
          output.mainColor, 255, round(timer / 3 % 2) * 127 + 128,
          // output.mainColor, 255, 255,
          false);

      // Innenring
      setLED(
          0,
          0, ledInner,
          output.mainColor, 255, round(timer / 3 % 2) * 127 + 128,
          // output.mainColor, 255, 255,
          false);

      return;
    }

    if (output.partyMode == 0)
    {
      // Serial.print("mode 1");

      // if (partyModePlaceholder1.currentBrightness >= 255)
      // {
      //     // Brightness Fade in
      //     partyModePlaceholder1.currentBrightness = 63;

      //     // Saturation on off
      //     partyModePlaceholder1.currentSaturation = !partyModePlaceholder1.currentSaturation;
      // }
      // partyModePlaceholder1.currentBrightness = partyModePlaceholder1.currentBrightness + 32;

      // Außenring
      setLED(
          1,
          0, ledOuter,
          output.mainColor, timer / 16 % 2 * 255, timer % 16 * 12 + 64,
          false);

      // Innenring
      setLED(
          0,
          0, ledInner,
          output.mainColor, (timer + 16) / 16 % 2 * 255, 255,
          // output.mainColor, (timer + 16 ) / 16 % 2 * 255, timer + 16 % 16 * 12 + 64,
          false);

      // Serial.print(" B: ");
      // Serial.print(partyModePlaceholder1.currentBrightness);
      // Serial.print(" S: ");
      // Serial.print(partyModePlaceholder1.currentSaturation);

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

      // setLED(
      //     1,
      //     0, ledOuter,
      //     output.mainColor, 255, 127,
      //     false);

      input.veloPressed[0]++;

      return;
    }

    // Überarbeitung Dominik: Hauptfarbe auf Ring und Modus auf Mitte
    setLED(
        0,
        0, ledInner,
        output.partyModeColor[output.partyMode], 255, 255,
        false);

    // setLED(
    //     1,
    //     0, ledOuter,
    //     output.mainColor, 255, 127,
    //     false);

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
        Serial.print("oh wow, die Flasche ist geneigt ");
        Serial.print(input.active);

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
          0, 0, 50,
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
      Serial.print("input.selectMode "); //debug
      Serial.print(input.selectMode);    //debug
      Serial.print(" ");                 //debug

      // Abstandsfarbe
      setLED(
          1,
          0, ledOuter,
          0, 255, 127,
          false);

      // 4 Richtungen für Modi zeigen
      if (input.bottleTilted)
      { // Falls Flasche gekippt
        Serial.print("geneigt ");
        Serial.print(input.active);

        // ledOuter -> 1 schwarz, 6 weiß, 1 schwarz
        // ledInner -> modus

        setLED(
            0,
            0, ledInner,
            0, 0, 127,
            false);

        for (size_t i = 0; i < 4; i++)
        {
          if (input.active / (ledOuter / 4) == i)
          {
            setLED(
                1,
                (i * input.veloCount) + 1, (ledOuter / 4) - 2,
                0, 0, 255,
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

      for (size_t i = 0; i < 4; i++)
      {
        setLED(
            1,
            (i * input.veloCount) + 1, (ledOuter / 4) - 2,
            0, 0, 255,
            false);
      }

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

void loop()
{
  inputFunctionReadVelo();

  // Velo
  // Serial.print("before: ");

  // for (int i = 0; i < input.veloCount; i++)
  // {
  //   // Serial.print(input.veloCables[i]);
  //   // Serial.print("  ");
  //   Serial.print(input.analogValues[i][3]);
  //   // Serial.print(" / ");
  //   // Serial.print(input.analogValues[i][4]);
  //   // Serial.print("% / ");
  //   // Serial.print(input.analogValuesOriginal[i][0]);
  //   Serial.print(" – ");
  //   // Serial.print(input.analogValues[i][0]);
  //   // Serial.print("-");
  //   // Serial.print(input.analogValues[i][1]);
  //   // Serial.print("-");
  //   // Serial.print(input.analogValues[i][2]);
  // }

  // Serial.print("\t\t after: ");
  for (int i = 0; i < input.veloCount; i++)
  {
    Serial.print(input.analogValues[i][4]);
    Serial.print("\t");
  }

  // Serial.print(" avg: ");
  // Serial.print(input.averagePercentage);
  // Serial.print("\t");

  // for (size_t i = 0; i < 5; i++)
  // {
  //   Serial.print("\tV: ");
  //   Serial.print(input.analogValuesOriginal[1][i]);
  // }

  // ##### Joystick
  //
  //
  //
  //

  // inputFunctionJoystick();
  // inputFunctionButton();

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