#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define DIGPIN 6
#define PXLCNT 24
#define IRQ   (2)
#define RESET (3)

// NFC
Adafruit_NFCShield_I2C nfc(IRQ, RESET);

// NP
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PXLCNT, DIGPIN, NEO_GRB + NEO_KHZ800);

uint32_t red = strip.Color(255, 0, 0);
uint32_t green = strip.Color(0, 255, 0);

// Setup
void setup() {
 // NFC
  nfc.begin();
  nfc.SAMConfig();
  
  // NP
  strip.begin();
  strip.setBrightness(20);
  strip.show();
}

// Loop
void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  // Default state
  colorWipe(red, 10);
  
  delay(2500);
  
  // Tag detected
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
   //colorWipe(green, 10);
   //theaterChase(green, 10);
   chase(green, 50);
   fullRing(0);
   delay(50);
   fullRing(green);
   //blinkFull(green, 500);
   delay(2000);
  }
}



void fade(uint8_t wait) {
  for(int i = 0; i < 100; i++) {
    strip.setBrightness(i);
    delay(5);
    strip.show();
  }
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c); 
      strip.show();
      delay(wait);
  }
}

void chase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void fullRing(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
   }
   strip.show();
}
void blinkFull(uint32_t c, uint8_t wait) {
    strip.setBrightness(0);
    strip.show();
    delay(wait);
    strip.setBrightness(20);
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
   }
   strip.show();
}
