int clockPin = 11;
int resetPin = 12;
int A12 = 10;
int q1Pin = 2;
int q2Pin = 3;
int q3Pin = 4;
int q4Pin = 5;
int q5Pin = 6;
int q6Pin = 7;
int q7Pin = 8;
int q8Pin = 9;
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
  Serial.begin(9600);
  pinMode(clockPin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  pinMode(A12, OUTPUT);
  pinMode(q1Pin, INPUT_PULLUP);
  pinMode(q2Pin, INPUT_PULLUP);
  pinMode(q3Pin, INPUT_PULLUP);
  pinMode(q4Pin, INPUT_PULLUP);
  pinMode(q5Pin, INPUT_PULLUP);
  pinMode(q6Pin, INPUT_PULLUP);
  pinMode(q7Pin, INPUT_PULLUP);
  pinMode(q8Pin, INPUT_PULLUP);
}

void loop() {
  //toggle counter reset
  digitalWrite(resetPin, 1);
  digitalWrite(resetPin, 0);
  //set highest address pin to 0
  digitalWrite(A12, 0);
  Serial.println("reset");
  //step through the first 4096 registers and read the binary data
  for (i = 0; i < 4096; i++) {
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
  //toggle counter reset
  digitalWrite(resetPin, 1);
  digitalWrite(resetPin, 0);
  //set highest address pin to 1
  digitalWrite(A12, 1);
  //step through the remaining 4096 registers and read the binary data
  for (i = 4096; i < 8192; i++) {
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
