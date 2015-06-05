/*======================================================
   SFOuino - INF1510 - v15
  ======================================================*/

// Libraries
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <MemoryFree.h>

// Pins
//#define COMPIN   4  // Shelf unit
#define DISTPIN  0    // Distance sensor
#define SPKRPIN  8    // Speaker
#define NPRPIN   6    // NeoPixel-ring
#define PXLCNTR 24        // Pixel count
#define NPSPIN   5    // NeoPixel-strip
#define PXLCNTS  3        // Pixel count
#define IRQ      2    // NFC
#define RESET    3    // NFC

// Config
const int NFC_TIMEOUT      = 250;   // Prevent NFC blocking distance sensor
const int RESET_TAG        = 498;   // Master NFC-tag ID
const int STUDENT_COUNT    = 3;     // Student count
const int MAX_BRIGHTNESS   = 255;   // NeoPixel maximum brightness
const int BASE_BRIGHTNESS  = 55;    // NeoPixel base brightness

// Intialize NFC shield
Adafruit_NFCShield_I2C nfc(IRQ, RESET);

uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
uint8_t uidLength; // Length of the UID

// Intialize NeoPixel
Adafruit_NeoPixel ring = Adafruit_NeoPixel(PXLCNTR, NPRPIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PXLCNTS, NPSPIN, NEO_GRB + NEO_KHZ800);


/*======================================================
   Class: Student
  ======================================================*/
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
    
    
    /* //Communicate with shelf-unit
    Wire.beginTransmission(COMPIN);
    Wire.write(this->shelfId);
     
    if (this->atSchool) {
      Wire.write(1); // Turn on (green)
    }
    else {
      Wire.write(0); // Turn off (red)
    }
     
    Wire.endTransmission();*/
    
    // Control strip directly
    if (this->atSchool) {
      strip.setPixelColor(this->shelfId, strip.Color(0, 255, 0)); // Green
      Serial.println("ON");
    }
    else {
      strip.setPixelColor(this->shelfId, strip.Color(255, 0, 0)); // Red
    }
    strip.show();
  }  
};


/*======================================================
   Class: Controller
  ======================================================*/
class Controller 
{
  // Public
  public:
    // Constructor
    Controller() {
      Serial.println("Initializing controller..");
      
      // Join I2C bus
      //Wire.begin(); // Master-slave communication betwen Arduinos
    
      // NFC
      nfc.begin();
      nfc.SAMConfig();
      nfc.setPassiveActivationRetries(NFC_TIMEOUT); // Set timeout to prevent blocking,
                                                    // allowing distance sensor reads
  
      // Wall unit LED-ring
      ring.begin();
      ring.setBrightness(BASE_BRIGHTNESS);
      ring.show();

      // Shelf LED-strip
      strip.begin();
      strip.setBrightness(MAX_BRIGHTNESS);
      
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
      this->animateWipe(ring.Color(255, 155, 0), 10); // Orange
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
      int lvl = BASE_BRIGHTNESS;
      
      // Dynamically change light intensity 
      // when within 200cm
      if (cm < 200) {
        lvl = (MAX_BRIGHTNESS - cm);
      }
      
      Serial.print(cm);
      Serial.print("cm -- level: ");
      Serial.println(lvl);
      
      ring.setBrightness(lvl);
      ring.show();
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
      this->animateChase(ring.Color(255, 155, 0));  // Orange
    }
    
    // Result: Success
    virtual void success() {
      this->playTone(1001, 75);
      animateSolid(ring.Color(0, 255, 0));  // Green
      this->playTone(749, 200);
      delay(100);
      animateSolid(0);
      delay(100);
      animateSolid(ring.Color(0, 255, 0));  // Green
      delay(200);
      animateSolid(0);
      delay(200);
      animateSolid(ring.Color(0, 255, 0));  // Green
      delay(2000); 
    }
    
    // Result: Check
    virtual void check() {
      this->playTone(749, 150);
      animateSolid(ring.Color(0, 255, 0));  // Green
      delay(1000);
    }
    
    // Result: Error
    virtual void error() {
      this->playTone(1898, 75);
      animateSolid(ring.Color(255, 0, 0));
      this->playTone(1126, 150);
      delay(100);
      animateSolid(0);
      delay(100);
      animateSolid(ring.Color(255, 0, 0));
      delay(200);
      animateSolid(0);
      delay(200);
      animateSolid(ring.Color(255, 0, 0));
      delay(2000); 
    }
    
    // Animation: Chase (indicate "working" state)
    virtual void animateChase(uint32_t color, uint8_t wait = 50) {
      for (int j = 0; j < 10; j++) {
        for (int q = 0; q < 3; q++) {
          for (int i = 0; i < ring.numPixels(); i = i + 3) {
            ring.setPixelColor(i + q, color);
          }
          ring.show();
         
          delay(wait);
         
          for (int i = 0; i < ring.numPixels(); i = i + 3) {
            ring.setPixelColor(i + q, 0);
          }
        }
      }
    }
    
    // Animation: Wipe (indicate change of state, e.g. from success to ready)
    void animateWipe(uint32_t color, uint8_t wait) {
      for(uint16_t i = 0; i < ring.numPixels(); i++) {
          ring.setPixelColor(i, color); 
          ring.show();
          delay(wait);
      }
    }
    
    // Animation: Solid
    void animateSolid(uint32_t color) {
      for(uint16_t i = 0; i < ring.numPixels(); i++) {
        ring.setPixelColor(i, color);
      }
      ring.show();
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
      
      this->animateChase(ring.Color(0, 0, 255), 25);
      
      // Call everybody back to school
      for (int i = 0; i < STUDENT_COUNT; i++) {
          Serial.println(" * Student: " + this->students[i]->name);
          this->students[i]->atSchool = true;
          this->students[i]->updateShelf();
      }
      
      delay(300);
      this->playTone(1898, 75);
      this->animateSolid(ring.Color(0, 0, 255));  // Blue
      this->playTone(1126, 150);
      delay(1000);
       
      // Ready
      this->ready();
    }
};

// Controller (wall unit)
Controller *ctrl;

// Setup
void setup() {
  Serial.begin(9600);
  
  // Controller
  ctrl = new Controller();
  
  // Add dummy students
  ctrl->addStudent(new Student(475, 0, "Ida"));
  ctrl->addStudent(new Student(283, 1, "Per"));
  ctrl->addStudent(new Student(202, 2, "Ola"));
  
  // Set ready status
  ctrl->ready();
}

// Loop: Listener  
void loop() {
  // Wait for NFC tag (until timeout)
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    ctrl->tagDetected();
  }
  // Read distance sensor
  else {
    ctrl->adjustLightLevels();
  }
}

