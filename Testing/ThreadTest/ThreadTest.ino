// Libraries
#include <Wire.h>
#include <Thread.h>
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

Thread distanceThread = Thread();

void test1() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
   // Tag detected
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.println("NFC TAG DETECTED");
  } 
}

// Setup
void setup() {
  Serial.begin(9600);
  Serial.println("Setup...");
  
  // Join I2C bus
  Wire.begin();
    
  // NFC
  nfc.begin();
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();
  
  distanceThread.onRun(test1);
  distanceThread.setInterval(1000);
}

// Loop
void loop(void) {  
  if(distanceThread.shouldRun())
    distanceThread.run();

  // Distance
  Serial.println(analogRead(0));
}

