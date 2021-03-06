#include <Adafruit_NeoPixel.h>

#define PIN 7
#define NUM_SPOKES 12
#define SPOKES_LIT 6

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

  setCompletedSpokes(SPOKES_LIT);
  setLoadingSpokes(SPOKES_LIT);

  strip.show();
  delay(5);
}

void setCompletedSpokes(uint16_t number) {
  // loop through number of completed spokes and set them to solid
  // we actually start from spoke 12 and go down so that they fill in
  // clockwise, the same direction of the spinner  for (int i = 0; i < number; i++) {
  for (int i = 0; i < number; i++) {
    int currentSpoke = NUM_SPOKES - 1 - i;
    for (uint16_t j = spokes[currentSpoke][0]; j <= spokes[currentSpoke][1]; j++) {
      strip.setPixelColor(j, 0, 255, 255);
    }
  }
}

void setLoadingSpokes(uint16_t number) {

  // loop through spokes from number-end and set them to a color
  // with brightness determined by a sine wave

  // update time & step variables
  float time = millis() / 100.0;
  float step = TWO_PI / (NUM_SPOKES - number);

  // make 'em pulse
  for (int i = NUM_SPOKES - number - 1; i >= 0; i--) {

    float sinVal = sin(time + step * i);
    float sinNormalized = sinVal / 2 + 0.5;

    // this line "compressess" the sine wave so there's a bigger
    // difference between first/brightest spoke and the last/dimmest one
    sinNormalized = sinNormalized * sinNormalized * sinNormalized;

    float r = sinNormalized * 225;
    float g = sinNormalized * 255;
    float b = sinNormalized * 255;

    for (uint16_t j = spokes[i][0]; j <= spokes[i][1]; j++) {
      strip.setPixelColor(j, r, g, b);
    }

  }
}


