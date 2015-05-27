// Libraries
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

// Pins
#define COMPIN   4
#define DIGPIN   6
#define PXLCNT   3

// Config
const int STUDENT_COUNT = 3;
const int BRIGHTNESS = 255;

// Intialize NeoPixel strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PXLCNT, DIGPIN, NEO_GRB + NEO_KHZ800);

// Setup
void setup() {
  // Debug
  Serial.begin(9600);
  Serial.println("Setup...");

  // NeoPixel
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  
  //for(uint16_t i = 0; i < strip.numPixels(); i++) // While waiting for wall controller
    //strip.setPixelColor(i, 0); // Off
  
  //strip.show();

  // Join I2C bus
  Wire.begin(COMPIN);
  Wire.onReceive(receiveEvent);
  
  Serial.println("Listening...");
}


// Loop
void loop() {
  delay(100);
}

// Set shelf light
void setShelfLight(int shelfId, int onoroff) 
{
  Serial.println("Update shelf:");
  Serial.print(" * ID: ");
  Serial.print(shelfId);
  Serial.print(", set: ");
  Serial.println(onoroff);
  
  // Status: School
  if (onoroff == 1) {
    strip.setPixelColor(shelfId, strip.Color(0, 255, 0)); // Green
  }
  // Status: Home
  else {
    strip.setPixelColor(shelfId, strip.Color(255, 0, 0)); // Red
  }
  
  // Update
  strip.show();
}

void receiveEvent(int howMany)
{
  if (howMany == 2)
    setShelfLight(Wire.read(), Wire.read());
}
