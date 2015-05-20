
int speakerPin = 8;


#define B1  2010
#define C1  1898
#define D1  1690
#define E1  1500
#define F1  1420
#define G1  1265
#define X1  1194
#define A1  1126

//char names[] = { 'c', 'd', 'e', 'f', 'g', 'x', 'a', 'z', 'b', 'C', 'y', 'D', 'w', 'E', 'F', 'q', 'G', 'i' };
//int tones[] = { 1898, 1690, 1500, 1420, 1265, 1194, 1126, 1063, 1001, 947, 893, 843, 795, 749, 710, 668, 630, 594 };
   





void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNotes(int count, int notes[], int beats[], int tempo = 75) {
  for (int i = 0; i < count; i++) {
     playTone(notes[i], beats[i] * tempo);
  }
}


void setup() {
  Serial.begin(9600);
  pinMode(speakerPin, OUTPUT);

  int notes[] = {A1, B1, D1, E1, F1, G1, X1, A1};
  int beats[] = { 3,  1,  2,  2,  1,  2,  3};
  playNotes(7, notes, beats);
}

void loop() {}
