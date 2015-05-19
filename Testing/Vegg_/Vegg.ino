// Libraries
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define DISTPIN  0
#define SPKRPIN  8
#define DIGPIN   6
#define PXLCNT   24
#define IRQ     (2)
#define RESET   (3)

// NFC
Adafruit_NFCShield_I2C nfc(IRQ, RESET);

// NP
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PXLCNT, DIGPIN, NEO_GRB + NEO_KHZ800);

uint32_t red = strip.Color(255, 0, 0);
uint32_t green = strip.Color(0, 255, 0);
uint32_t orange = strip.Color(255, 102, 0);

// Beep
void beep(int msdelay, int misdelay = 700){
  for (int i = 0; i < msdelay; i++) {
    digitalWrite(SPKRPIN, HIGH);
    delayMicroseconds(misdelay);
    digitalWrite(SPKRPIN, LOW);
    delayMicroseconds(misdelay);
  }
}

// Default state
void defaultState() {
  delay(1000);
  wipe(red, 10);
}

// Going home
void animateHome() {
  solid(green);
  delay(100);
  solid(0);
  delay(100);
  solid(green);
  delay(200);
  solid(0);
  delay(200);
  solid(green);
  delay(2000);
}

// Animations
void wipe(uint32_t c, uint8_t wait) {
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

void solid(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
   }
   strip.show();
}


// NFC Detected
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
void onNFCRed() {
  //beep(75);
    
  chase(orange, 50);
  
  int studentId = 0;
  
  for (uint8_t i = 0; i < uidLength; i++) 
    studentId += uid[i];
    
  Wire.beginTransmission(4);
  Wire.write(studentId);
  Wire.endTransmission();
 
  animateHome();
  
  defaultState();
}

// Setup
void setup() {
  Serial.begin(9600);
  
  // Distance sensor
  randomSeed(analogRead(DISTPIN));
  
  // Speaker
  pinMode(SPKRPIN, OUTPUT);
  
  // Join I2C bus
  Wire.begin();
    
  // NFC
  nfc.begin();
  nfc.setPassiveActivationRetries(0x01);
  nfc.SAMConfig();
  attachInterrupt(0, onNFCRead, RISING);
  
  // NP
  strip.begin();
  strip.setBrightness(25);
  strip.show();
  
  // Ready
  defaultState();
}

// Loop
void loop(void) {
  // Distance
  Serial.println(analogRead(0));
  
  // Detect tag
  nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);
}

