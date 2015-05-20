
int SonarPin=A0;

void setup(){
  pinMode(SonarPin,INPUT);
  Serial.begin(9600);
}
void loop(){
  int sensorValue = analogRead(SonarPin);
  delay(50);
  int cm = (sensorValue * 0.497) * 2.54;

  Serial.print(sensorValue);
  Serial.print(" | ");
  Serial.print(cm);
  Serial.println("cm");

  delay(200);
}
