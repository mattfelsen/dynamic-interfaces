#include <Adafruit_NeoPixel.h>

#define PIN 7
#define NUM_SPOKES 12
#define SPOKES_LIT 0

Adafruit_NeoPixel strip = Adafruit_NeoPixel(103, PIN, NEO_GRB + NEO_KHZ800);

int spokes[NUM_SPOKES][2] = {
  { 0, 4 },
  { 7,  12 },
  { 17, 22 },
  { 25, 30},
  { 36, 42 },
  { 45, 50 },
  { 55, 61 },
  { 64, 68 },
  { 73, 77 },
  { 81, 85 },
  { 90, 94 },
  { 97, 102 }
};

void setup() {
  strip.begin();
  strip.show();
}

void loop() {
  float time = millis() / 100.0;
  float step = TWO_PI / (NUM_SPOKES - SPOKES_LIT);

  int startSpoke = SPOKES_LIT;

  for (uint16_t i = SPOKES_LIT; i < NUM_SPOKES; i++) {

    float sinVal = sin(time + step * i);
    float sinNormalized = sinVal / 2 + 0.5;

    float r = sinNormalized * 0;
    float g = sinNormalized * 100;
    float b = sinNormalized * 100;

    for (uint16_t j = spokes[i][0]; j <= spokes[i][1]; j++) {
      strip.setPixelColor(j, r, g, b);
    }

  }

  for (uint16_t i = 0; i < SPOKES_LIT; i++) {
    for (uint16_t j = spokes[i][0]; j <= spokes[i][1]; j++) {
      strip.setPixelColor(j, 0, 100, 0);
    }

  }

  strip.show();
  delay(5);
}


