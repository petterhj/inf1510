// Libraries
#include <Wire.h>

// Class: Student
class Student
{
  public:
   int id;
   int on_pin;
   int off_pin;
   virtual void config(int id, int on_pin, int off_pin) {
     // Variables
     this->id = id; 
     this->on_pin = on_pin; 
     this->off_pin = off_pin;
     
     // Pin mode
     pinMode(this->on_pin, OUTPUT);
     pinMode(this->off_pin, OUTPUT);
    
     // Default     
     digitalWrite(this->on_pin, HIGH);
     digitalWrite(this->off_pin, LOW);
   }
   virtual void setHome() { 
     //digitalWrite(this->on_pin, LOW);
     //digitalWrite(this->off_pin, HIGH);
     Serial.print("Set pin ");
     Serial.print(this->on_pin);
     Serial.println(" low");
     Serial.print("Set pin ");
     Serial.print(this->off_pin);
     Serial.println(" high");
     Serial.println("Student went home");
   }
};


// Students
const int STUDENT_COUNT = 3;
Student *stud1;
Student *stud2;
Student *stud3;

Student *students[STUDENT_COUNT];

// Setup
void setup() {
  // Debug
  Serial.begin(9600);
  Serial.println("Setup...");
    
  // Configure students
  stud1 = new Student();
  stud1->config(219, 3, 5);
  stud2 = new Student();
  stud2->config(242, 6, 9);
  stud3 = new Student();
  stud3->config(27, 10, 11);
  
  //students = {stud1, stud2, stud3};
  students[0] = stud1;
  students[1] = stud2;
  students[2] = stud3;
  
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  
  Serial.println("Listening...");
}


// Loop
void loop() {
  delay(100);
  //stud1->setHome();
}

void sendHome(int sid) 
{
  Serial.print("RECEIVED: ");
  Serial.println(sid);
  Serial.println();
  
  for (int i = 0; i < STUDENT_COUNT; i++) {
    Serial.println();
    
    if (sid == students[i]->id) {
       Serial.print("Sending student ");
       Serial.print(sid);
       Serial.println(" home!");
       
       Serial.println(students[i]->on_pin);
       Serial.println(students[i]->off_pin);
       //students[i]->setHome();
       digitalWrite(students[i]->on_pin, LOW);
       digitalWrite(students[i]->off_pin, HIGH);
    }
  }  
}

void receiveEvent(int howMany)
{
  sendHome(Wire.read());
}
