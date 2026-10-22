const int pinPWMA = 25;
const int pinAIN2 = 34;
const int pinAIN1 = 35;
const int pinBIN1 = 32;
const int pinBIN2 = 33;
const int pinPWMB = 26;
const int pinSTBY = 14;
const int waitTime = 2000; // espera entre fases
const int speed = 200; // velocidad de giro
const int pinMotorA[3] = {pinPWMA, pinAIN2, pinAIN1};
const int pinMotorB[3] = {pinPWMB, pinBIN1, pinBIN2};

enum moveDirection {
  forward,
  backward
};

enum turnDirection {
  clockwise,
  counterClockwise
};

void setup() {
  pinMode(pinAIN2, OUTPUT);
  pinMode(pinAIN1, OUTPUT);
  pinMode(pinPWMA, OUTPUT);
  pinMode(pinBIN1, OUTPUT);
  pinMode(pinBIN2, OUTPUT);
  pinMode(pinPWMB, OUTPUT);
  pinMode(pinSTBY, OUTPUT);
  Serial.begin(9600); // Iniciar el puerto serie
}

void loop() {
  enableMotors();
  move(forward, 180);
  delay(waitTime);
  move(backward, 180);
  delay(waitTime);
  turn(clockwise, 180);
  delay(waitTime);
  turn(counterClockwise, 180);
  delay(waitTime);
  fullStop();
  delay(waitTime);
}

// Funciones que controlan el vehiculo
void move(int direction, int speed) {
  if (direction == forward) {
    moveMotorForward(pinMotorA, speed);
    moveMotorForward(pinMotorB, speed);
  } else {
    moveMotorBackward(pinMotorA, speed);
    moveMotorBackward(pinMotorB, speed);
  }
  printMotorState();
}

void turn(int direction, int speed) {
  if (direction == clockwise) {
    moveMotorForward(pinMotorA, speed);
    moveMotorBackward(pinMotorB, speed);
  } else {
    moveMotorBackward(pinMotorA, speed);
    moveMotorForward(pinMotorB, speed);
  }
  printMotorState();
}

void fullStop() {
  disableMotors();
  stopMotor(pinMotorA);
  stopMotor(pinMotorB);
  printMotorState();
}

// Funciones que controlan los motores
void moveMotorForward(const int pinMotor[3], int speed) {
  digitalWrite(pinMotor[1], HIGH);
  digitalWrite(pinMotor[2], LOW);
  analogWrite(pinMotor[0], speed);
}

void moveMotorBackward(const int pinMotor[3], int speed) {
  digitalWrite(pinMotor[1], LOW);
  digitalWrite(pinMotor[2], HIGH);
  analogWrite(pinMotor[0], speed);
}

void stopMotor(const int pinMotor[3]) {
  digitalWrite(pinMotor[1], LOW);
  digitalWrite(pinMotor[2], LOW);
  analogWrite(pinMotor[0], 0);
}

void enableMotors() {
  digitalWrite(pinSTBY, HIGH);
}

void disableMotors() {
  digitalWrite(pinSTBY, LOW);
}

void printMotorState() {
  Serial.print("Motor A: ");
  Serial.print(digitalRead(pinMotorA[1]));
  Serial.print(digitalRead(pinMotorA[2]));
  Serial.print(" | Motor B: ");
  Serial.print(digitalRead(pinMotorB[1]));
  Serial.println(digitalRead(pinMotorB[2]));
}
