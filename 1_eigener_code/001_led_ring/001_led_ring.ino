#include <FastLED.h>

#define DATA_PIN 6
#define NUM_LEDS 12
#define BRIGHTNESS 64
#define LED_TYPE WS2811
#define COLOR_ORDER RBG
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

void setup()
{
    delay(3000); // power-up safety delay

    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
    FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
}

void loop()
{
    leds[0] = CRGB::Yellow;
    FastLED.show();
    delay(30);
}
