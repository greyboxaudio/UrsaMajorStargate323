int clockPin = 13;
int resetPin = 12;
int q1Pin = 1;
int q2Pin = 2;
int q3Pin = 3;
int q4Pin = 4;
int q5Pin = 5;
int q6Pin = 6;
int q7Pin = 7;
int q8Pin = 8;
int q1;
int q2;
int q3;
int q4;
int q5;
int q6;
int q7;
int q8;
int i;

void setup() {
  // put your setup code here, to run once:
pinMode(clockPin, OUTPUT);
pinMode(resetPin, OUTPUT);
pinMode(q1Pin, INPUT); 
pinMode(q2Pin, INPUT); 
pinMode(q3Pin, INPUT); 
pinMode(q4Pin, INPUT); 
}

void loop() {
digitalWrite(resetPin, 1);
digitalWrite(resetPin, 0);
Serial.println("reset");
for (i=0; i<256; i++){
  digitalWrite(clockPin, 1);
  q1 = digitalRead(q1Pin);
  q2 = digitalRead(q2Pin);
  q3 = digitalRead(q3Pin);
  q4 = digitalRead(q4Pin);
  q5 = digitalRead(q5Pin);
  q6 = digitalRead(q6Pin);
  q7 = digitalRead(q7Pin);
  q8 = digitalRead(q8Pin);
  digitalWrite(clockPin, 0);
  Serial.print(q8);
  Serial.print(q7);
  Serial.print(q6);
  Serial.print(q5);
  Serial.print(q4);
  Serial.print(q3);
  Serial.print(q2);
  Serial.println(q1);
}
}
