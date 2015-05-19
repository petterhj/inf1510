#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>

#define IRQ   (2)
#define RESET (3)

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uidLength;
Adafruit_NFCShield_I2C nfc(IRQ, RESET);

void onInterrupt(){
    Serial.println("DETECT");
}

void setup() {
  Serial.begin(9600);
  delay(50);
  nfc.begin();
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();
  attachInterrupt(0, onInterrupt, RISING);
}

void loop() {
  Serial.println("loop");
  delay(1000);
  nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);
}
