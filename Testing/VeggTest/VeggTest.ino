#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define DIGPIN 6
#define PXLCNT 3

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PXLCNT, DIGPIN, NEO_GRB + NEO_KHZ800);

uint32_t red = strip.Color(255, 0, 0);
uint32_t green = strip.Color(0, 255, 0);

void setup() {
  strip.begin();
  strip.setBrightness(255);
  strip.setPixelColor(0, green);
  strip.setPixelColor(1, red);
  strip.setPixelColor(2, green);
  strip.show();
}

void loop() {
  
}
