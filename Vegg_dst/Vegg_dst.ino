// Libraries
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <MemoryFree.h>

// Pins
#define COMPIN   4
#define DISTPIN  0
#define SPKRPIN  8
#define DIGPIN   6
#define PXLCNT   24
#define IRQ     (2)
#define RESET   (3)

// Config
const int NFC_TIMEOUT    = 750;
const int RESET_TAG      = 498;
const int STUDENT_COUNT  = 3;
const int BRIGHTNESS_L1  = 25;
const int BRIGHTNESS_L2  = 55;
const int BRIGHTNESS_L3  = 75;

// Intialize NFC shield
Adafruit_NFCShield_I2C nfc(IRQ, RESET);

// Intialize NeoPixel ring
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PXLCNT, DIGPIN, NEO_GRB + NEO_KHZ800);

uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
uint8_t uidLength; // Length of the UID

// Class: Student
class Student
{
  // Public
  public:
   // Variables
   int tagId;
   int shelfId;
   String name;
   boolean atSchool;
   
   // Contructor
   Student(int tagId, int shelfId, String name) {
     // Variables
     this->tagId = tagId;
     this->shelfId = shelfId;
     this->name = name;
     
     // Default: At school
     this->atSchool = true;
     this->updateShelf();
   }
   
   // Going home
   virtual boolean goingHome() {
     if (this->atSchool) {
       // Set status
       this->atSchool = false;
       this->updateShelf();
     }
     
     return this->atSchool;
   }
 
   // Update shelf status
   virtual void updateShelf() {
     Serial.print(" * Updating shelf: ");
     Serial.print(this->shelfId);
     Serial.print(", turn ");
     
     // Communicate
     Wire.beginTransmission(COMPIN);
     Wire.write(this->shelfId);
     
     if (this->atSchool) {
       Wire.write(1); // Turn on (green)
       Serial.println("ON");
     }
     else {
       Wire.write(0); // Turn off (red)
       Serial.println("OFF");
     }
     
     Wire.endTransmission();
   }  
};

// Class: Controller
class Controller 
{
  // Public
  public:
    // Constructor
    Controller() {
      Serial.println("Initializing controller..");
      
      // Join I2C bus
      Wire.begin();
    
      // NFC
      nfc.begin();
      nfc.SAMConfig();
      nfc.setPassiveActivationRetries(NFC_TIMEOUT);
  
      // NeoPixel
      strip.begin();
      strip.setBrightness(BRIGHTNESS_L1);
      strip.show();
      
      // Speaker
      pinMode(SPKRPIN, OUTPUT);
      
      // Distance sensor
      pinMode(DISTPIN, INPUT);
  
      // Set student slots to NULL
      for (int i = 0; i < STUDENT_COUNT; i++)
        this->students[i] = NULL;
    }
    
    // State: Ready
    virtual void ready() {
      // Feedback
      this->animateWipe(strip.Color(255, 0, 0), 10);
    }
    
    // Tag detected
    virtual void tagDetected() {
      // Read tag ID
      int tagId = 0;
      
      for (uint8_t i = 0; i < uidLength; i++) 
        tagId += uid[i];
      
       Serial.print(" * Tag detected, ID: ");
       Serial.println(tagId);
      
      if (tagId != RESET_TAG) {
        // Register student
        this->registerStudent(tagId);
      }
      else {
        // Reset all (master tag)
        this->reset();
      }
    }
    
    // Adjust light levels
    virtual void adjustLightLevels() {
      // Read and calculate (approximate) distance
      int sensorValue = analogRead(DISTPIN);
      delay(50);
      int cm = (sensorValue * 0.497) * 2.54;
      int lvl = 100;
      
      if (cm < 200) {
        lvl = (200 - (cm * 2));
      }
      
      Serial.print(cm);
      Serial.print("cm -- level: ");
      Serial.println(lvl);
      
      strip.setBrightness(lvl);
      strip.show();
    }
    
    // Add student
    virtual void addStudent(Student* s) {
       for (int i = 0; i < STUDENT_COUNT; i++) {
         if (this->students[i] == NULL) {
           this->students[i] = s;
           Serial.print(" * Student " + s->name + ", with tag ID ");
           Serial.print(s->tagId);
           Serial.print(", added at slot ");
           Serial.println(i);
           return;
         }
       }    
    }
    
    // Get student
    virtual Student* getStudent(int tagId) {
       for (int i = 0; i < STUDENT_COUNT; i++) {
         if (this->students[i]->tagId == tagId) {
           return this->students[i];
         }
       }
       return NULL;
    }
    
  // Private
  private:
    // Variables
    Student* students[STUDENT_COUNT];
    
    // Register student going home
    virtual void registerStudent(int tagId) {
      // Feedback
      this->playTone(594, 75);
      
      // Tag owner
      Student* student = this->getStudent(tagId);
      
      if (student != NULL) {
        if (student->atSchool) {
          Serial.println(" * Success: Student " + student->name + " went home!");
          
          // Feedback
          this->working();
          
          // Send home
          student->goingHome();
          
          // Feedback
          this->success();
        }
        else {
          Serial.println(" * Warning: Student " + student->name + " already registered, maybe just checking!");
          // Feedback
          this->check();
        }
      }
      else {
        Serial.println(" * Error: Unknown tag!");
        // Feedback
        this->error();
      }
      
      // Ready
      this->ready();
    }
    
    // State: Working
    virtual void working() {
      this->animateChase(strip.Color(255, 102, 0)); // Orange
    }
    
    // Result: Success
    virtual void success() {
      this->playTone(1001, 75);
      animateSolid(strip.Color(0, 255, 0));
      this->playTone(749, 200);
      delay(100);
      animateSolid(0);
      delay(100);
      animateSolid(strip.Color(0, 255, 0));
      delay(200);
      animateSolid(0);
      delay(200);
      animateSolid(strip.Color(0, 255, 0));
      delay(2000); 
    }
    
    // Result: Check
    virtual void check() {
      this->playTone(749, 150);
      animateSolid(strip.Color(0, 255, 0));
      delay(1000);
    }
    
    // Result: Error
    virtual void error() {
      this->playTone(1898, 75);
      animateSolid(strip.Color(255, 0, 0));
      this->playTone(1126, 150);
      delay(100);
      animateSolid(0);
      delay(100);
      animateSolid(strip.Color(255, 0, 0));
      delay(200);
      animateSolid(0);
      delay(200);
      animateSolid(strip.Color(255, 0, 0));
      delay(2000); 
    }
    
    // Animation: Chase
    virtual void animateChase(uint32_t color, uint8_t wait = 50) {
      for (int j = 0; j < 10; j++) {
        for (int q = 0; q < 3; q++) {
          for (int i = 0; i < strip.numPixels(); i = i + 3) {
            strip.setPixelColor(i + q, color);
          }
          strip.show();
         
          delay(wait);
         
          for (int i = 0; i < strip.numPixels(); i = i + 3) {
            strip.setPixelColor(i + q, 0);
          }
        }
      }
    }
    
    // Animation: Wipe
    void animateWipe(uint32_t color, uint8_t wait) {
      for(uint16_t i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, color); 
          strip.show();
          delay(wait);
      }
    }
    
    // Animation: Solid
    void animateSolid(uint32_t color) {
      for(uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, color);
      }
      strip.show();
    }
    
    // Sound: Tone
    void playTone(int tone, int duration) {
      for (long i = 0; i < duration * 1000L; i += tone * 2) {
        digitalWrite(SPKRPIN, HIGH);
        delayMicroseconds(tone);
        digitalWrite(SPKRPIN, LOW);
        delayMicroseconds(tone);
      }
    }
    
    // Reset function
    void reset() {
      Serial.println("Resetting...");
      
      this->animateChase(strip.Color(0, 0, 255), 25);
      
      // Call everybody back to school
      for (int i = 0; i < STUDENT_COUNT; i++) {
          Serial.println(" * Student: " + this->students[i]->name);
          this->students[i]->atSchool = true;
          this->students[i]->updateShelf();
      }
      
      delay(300);
      this->animateSolid(strip.Color(0, 0, 255));
      delay(1000);
       
      // Ready
      this->ready();
    }
};
Controller *ctrl;
// Setup
void setup() {
  Serial.begin(9600);
  
  // Controller
  ctrl = new Controller();
  
  // Students
  ctrl->addStudent(new Student(475, 0, "Ida"));
  ctrl->addStudent(new Student(283, 1, "Per"));
  ctrl->addStudent(new Student(202, 2, "Ola"));
  
  // Set ready status
  ctrl->ready();
}

// Loop: Listener  
void loop() {
  // Detect NFC tag
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    ctrl->tagDetected();
  }
  // Read distance sensor
  else {
    ctrl->adjustLightLevels();
  }
}

