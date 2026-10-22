const int pinPWMA = 25;
const int pinAIN2 = 2;
const int pinAIN1 = 15;
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
  Serial.begin(9600); // Iniciar el puerto serie
  Serial.println("Setup");
  pinMode(pinAIN2, OUTPUT);
  Serial.println("PinA2");
  pinMode(pinAIN1, OUTPUT);
  Serial.println("PinA1");
  pinMode(pinPWMA, OUTPUT);
  Serial.println("PWMA1");
  pinMode(pinBIN1, OUTPUT);
  Serial.println("PinB1");
  pinMode(pinBIN2, OUTPUT);
  Serial.println("PinB2");
  pinMode(pinPWMB, OUTPUT);
  Serial.println("PWMB1");
  pinMode(pinSTBY, OUTPUT);
  Serial.println("sTBY");
  
}

void loop() {
  enableMotors();
  Serial.println("Adelante");
  move(forward, 180);
  delay(waitTime);
  
  Serial.println("Atras");
  move(backward, 180);
  delay(waitTime);
  
  Serial.println("Derecha (Horario)");
  turn(clockwise, 180);
  delay(waitTime);
  
  Serial.println("Izquierda (antihorario)");
  turn(counterClockwise, 180);
  delay(waitTime);
  
  Serial.println("Full stop.");
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
