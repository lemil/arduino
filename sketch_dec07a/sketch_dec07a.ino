/*
   Board pin | NodeMCU GPIO |  Arduino IDE
      A-           1             5 or D1
      A+           3             0 or D3
      B-           2             4 or D2
      B+           4             2 or D4
*/



const int pwmMotorA = 5; //D1;
const int dirMotorA = 0; //D3;
const int pwmMotorB = 6; //D2;
const int dirMotorB = 4; //D4;

int delayBtwnStep = 1;

void setup() {
  
  Serial.begin ( 115200 );
  Serial.println("...");
  Serial.println("tic");

  pinMode(pwmMotorA, OUTPUT);
  pinMode(pwmMotorB, OUTPUT);
  pinMode(dirMotorA, OUTPUT);
  pinMode(dirMotorB, OUTPUT);

  Serial.println("tic");
  
  Serial.println("Motor SHield 12E Initialized");
  
}

void loop() {
  stepperRotate(500, 0);
  delay(500);
}


void stepperRotate(int nbStep, int dirStep) {
  for (int i = 0; i <= nbStep; i++) {
      if (dirStep == 0) {   // sens horaire
          nextStep(i % 4);
      }
      if (dirStep == 1) {   // sens antihoraire
          nextStep(3 - (i % 4));
      }
      delay(delayBtwnStep);
      Serial.println(i % 4);
  }
}


void nextStep(int index) {
  if (index == 0) {
      digitalWrite(pwmMotorA, true);
      digitalWrite(pwmMotorB, false);
      digitalWrite(dirMotorA, true);
      digitalWrite(dirMotorB, false);
  }
  if (index == 1) {
      digitalWrite(pwmMotorA, false);
      digitalWrite(pwmMotorB, true);
      digitalWrite(dirMotorA, true);
      digitalWrite(dirMotorB, false);
  }
  if (index == 2) {
      digitalWrite(pwmMotorA, false);
      digitalWrite(pwmMotorB, true);
      digitalWrite(dirMotorA, false);
      digitalWrite(dirMotorB, true);
  }
  if (index == 3) {
      digitalWrite(pwmMotorA, true);
      digitalWrite(pwmMotorB, false);
      digitalWrite(dirMotorA, false);
      digitalWrite(dirMotorB, true);
  }
}
