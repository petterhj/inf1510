// Libraries
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

// Config
const int STUDENT_COUNT = 3;

// Class: Student
class Student
{
  // Public
  public:
   // Variables
   int studentId;
   int shelfId;
   boolean atSchool;
   
   // Contructor
   Student(int studentId, int shelfId) {
     // Variables
     this->studentId = studentId;
     this->shelfId = shelfId;
     this->atSchool = true; 
   }
   
   virtual void setHome() {
     
     if (this->atSchool) {
       
       // Transmit to hall lights controller
       Wire.beginTransmission(4);
       Wire.write(this->shelfId);
       Wire.endTransmission(); 
      
       // Registered 
       this->atSchool = false;
     }
     else {
       
     }
   }
};

// Class: Controller
class Controller 
{
  // Public
  public:
    Controller(int studentCount) {
      
    }
    
    virtual void addStudent(Student* s) {
       Serial.println("added student");
       for (int i = 0; i < STUDENT_COUNT; i++) {
         if (this->students[i] == NULL) {
         }

       }    
    }
    
  // Private
  private:
    Student* students[STUDENT_COUNT];
    // Animations
    virtual void animateWorking() {
      
    }
};

// Pins
#define DISTPIN  0
#define SPKRPIN  8
#define DIGPIN   6
#define PXLCNT   24
#define IRQ     (2)
#define RESET   (3)

// Intialize NFC shield
Adafruit_NFCShield_I2C nfc(IRQ, RESET);

// Intialize NeoPixel ring
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PXLCNT, DIGPIN, NEO_GRB + NEO_KHZ800);

// Color definitions
uint32_t red = strip.Color(255, 0, 0);
uint32_t green = strip.Color(0, 255, 0);
uint32_t orange = strip.Color(255, 102, 0);

// Students
Student *stud1;
Student *stud2;
Student *stud3;
//Student *students[STUDENT_COUNT];

// Setup
void setup() {
  Serial.begin(9600);
  
  // Configure students (hardcoded)
  Controller *ctrl = new Controller(3);
  
  ctrl->addStudent(new Student(219, 0));
  //stud2 = new Student(242, 1);
  //stud3 = new Student(27, 2);
  
  /*
  students[0] = stud1;
  students[1] = stud2;
  students[2] = stud3;
  */

  // Distance sensor
  randomSeed(analogRead(DISTPIN));
  
  // Speaker
  pinMode(SPKRPIN, OUTPUT);
  
  // Join I2C bus
  Wire.begin();
    
  // NFC
  nfc.begin();
  nfc.SAMConfig();
  
  // NeoPixel
  strip.begin();
  strip.setBrightness(25);
  strip.show();
}

// Beep
void beep(int msdelay, int misdelay = 700){
  for (int i = 0; i < msdelay; i++) {
    digitalWrite(SPKRPIN, HIGH);
    delayMicroseconds(misdelay);
    digitalWrite(SPKRPIN, LOW);
    delayMicroseconds(misdelay);
  }
}

// Loop
void loop(void) {
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  // Default state
  wipe(red, 10);
  //delay(500);
  
  // Distance
  Serial.println(analogRead(0));
  
  // Tag detected
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    // Beep when tag first detected
    beep(75);
    
    // Indicate working status
    chase(orange, 50);
    
    // Get student ID
    int studentId = 0;
    
    for (uint8_t i = 0; i < uidLength; i++) 
      studentId += uid[i];
    
    // Transmit to hall lights  
    Wire.beginTransmission(4);
    Wire.write(studentId);
    Wire.endTransmission();
   
    // Registration feedback
    registrationSuccessful();
  }
}

// Going home
void registrationSuccessful() {
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

// Already registered
void animateRegistered() {
  
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


