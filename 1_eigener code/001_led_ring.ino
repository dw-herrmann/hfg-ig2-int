#include <FastLED.h>

#define LED_PIN     3
#define NUM_LEDS    10
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100



void setup() {
    delay( 3000 ); // power-up safety delay
}