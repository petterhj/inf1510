// Libraries
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>


// NP
#define DIGPIN 6
#define PXLCNT 24
#define IRQ   (2)
#define RESET (3)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PXLCNT, DIGPIN, NEO_GRB + NEO_KHZ800);

uint32_t red = strip.Color(255, 0, 0);


// Setup
void setup() {
  Serial.begin(9600);

  randomSeed(analogRead(0));
  
  // NP
  strip.begin();
  strip.setBrightness(20);
  solid(red);
  //strip.show();
}
 
 
// Loop
void loop() { 
  int distsensor, i;

  distsensor = 0;
   
  for (i=0; i<8; i++) {
    distsensor += analogRead(0);
    delay(50);
  }
   
  distsensor /= 8;
 
  Serial.println(distsensor);
  
  if (distsensor <= 100) {
    strip.setBrightness(100);
  }
  else {
    strip.setBrightness(10);
  }
  
  strip.show();
  
  /*
  if (distsensor <= 500) {
    Serial.println("<= 500");
  } 
  if (distsensor > 500) {
    Serial.println("> 500");
  } else if ((distsensor > 300) && (distsensor < 400)) {
    Serial.println("> 300 && < 400");
  } else if ((distsensor > 100) && (distsensor < 200)) {
    Serial.println("> 100 && < 200");
  } else if (distsensor < 50) {
    Serial.println("< 50");
  }*/
}


void solid(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
   }
   strip.show();
}
 
 
