#include <Adafruit_NeoPixel.h>
#define PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(180, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show();
  Serial.begin(9600);
}

void loop() {
  solid();
}

void solid() {
  float time = millis() / 200.0;
  float step = TWO_PI * 16 / strip.numPixels();

  for (uint16_t i = 0; i < strip.numPixels(); i++) {

    float sinVal = sin(time + step * i);
    float sinNormalized = sinVal / 2 + 0.5;
    
    //if (!i) Serial.println(sinVal);

    float r = sinNormalized * 0;
    float g = sinNormalized * 25;
    float b = sinNormalized * 25;

    strip.setPixelColor(i, r, g, b);

  }
  strip.show();
  delay(50);
}
