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
uint32_t test = strip.Color(65, 85, 175);

// Setup
void setup() {
 // Serial
 Serial.begin(115200);
 
 // NFC
  nfc.begin();
  nfc.SAMConfig();
  
  // NP
  strip.begin();
  strip.setBrightness(20);
  strip.show();
}

// Loop
int was = 1;

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  // Default state
  //snake(test, 50, 5);
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, test);
  }
  delay(1000);
  strip.show();
  fadeDown(test, 1000);
  fadeUp(test, 1000);
}

void fadeDown(uint32_t c, uint8_t wait) {  
  for(uint16_t i=1; i<50; i++) { 
    strip.setBrightness(i);
    strip.show();
    delay(100);
  }
}
void fadeUp(uint32_t c, uint8_t wait) {  
  for(uint16_t i=1; i<50; i++) { 
    strip.setBrightness((50-i));
    strip.show();
    delay(100);
  }
}


// Fill the dots one after the other with a color
void snake(uint32_t c, uint8_t wait, uint16_t length) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      
      strip.setPixelColor(i-length, 0);
      
      if (i < length) {
        uint32_t off = (strip.numPixels() - (length-i));
        strip.setPixelColor(off, 0);
      }
      
      strip.setBrightness(50 / (i+1));
        
      strip.show();   
   
      delay(wait);

  }
}
