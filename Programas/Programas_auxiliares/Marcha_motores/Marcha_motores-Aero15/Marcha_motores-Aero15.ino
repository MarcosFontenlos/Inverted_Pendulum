
const int pinPWMA = 25;
const int pinAIN2 = 2;
const int pinAIN1 = 15;
const int pinBIN1 = 33;
const int pinBIN2 = 32;
const int pinPWMB = 26;
const int pinSTBY = 14;
const int waitTime = 2000; // espera entre fases
const int speed = 200;     // velocidad de giro
const int pinMotorA[3] = {pinPWMA, pinAIN2, pinAIN1};
const int pinMotorB[3] = {pinPWMB, pinBIN1, pinBIN2};

enum moveDirection
{
    forward,
    backward
};

enum turnDirection
{
    clockwise,
    counterClockwise
};

void setup()
{
    Serial.begin(57600);
    
    pinMode(pinAIN2, OUTPUT);
    pinMode(pinAIN1, OUTPUT);
    pinMode(pinPWMA, OUTPUT);
    pinMode(pinBIN1, OUTPUT);
    pinMode(pinBIN2, OUTPUT);
    pinMode(pinPWMB, OUTPUT);
    pinMode(pinSTBY, OUTPUT);
}

void loop()
{
    enableMotors();
    Serial.println("Moviendo hacia adelante...");
    move(forward, 180);
    delay(waitTime);
    Serial.println("Moviendo hacia atrás...");
    move(backward, 180);
    delay(waitTime);
    Serial.println("Girando en sentido horario...");
    turn(clockwise, 180);
    delay(waitTime);
    Serial.println("Girando en sentido antihorario...");
    turn(counterClockwise, 180);
    delay(waitTime);
    Serial.println("Deteniendo...");
    fullStop();
    delay(waitTime);
}

// Funciones que controlan el vehículo
void move(int direction, int speed)
{
    if (direction == forward)
    {
        moveMotorForward(pinMotorA, speed);
        moveMotorForward(pinMotorB, speed);
    }
    else
    {
        moveMotorBackward(pinMotorA, speed);
        moveMotorBackward(pinMotorB, speed);
    }
}

void turn(int direction, int speed)
{
    if (direction == clockwise)
    {
        moveMotorForward(pinMotorA, speed);
        moveMotorBackward(pinMotorB, speed);
    }
    else
    {
        moveMotorBackward(pinMotorA, speed);
        moveMotorForward(pinMotorB, speed);
    }
}

void fullStop()
{
    disableMotors();
    stopMotor(pinMotorA);
    stopMotor(pinMotorB);
}

// Funciones que controlan los motores
void moveMotorForward(const int pinMotor[3], int speed)
{
    digitalWrite(pinMotor[1], HIGH);
    digitalWrite(pinMotor[2], LOW);
    analogWrite(pinMotor[0], speed);
}

void moveMotorBackward(const int pinMotor[3], int speed)
{
    digitalWrite(pinMotor[1], LOW);
    digitalWrite(pinMotor[2], HIGH);
    analogWrite(pinMotor[0], speed);
}

void stopMotor(const int pinMotor[3])
{
    digitalWrite(pinMotor[1], LOW);
    digitalWrite(pinMotor[2], LOW);
    analogWrite(pinMotor[0], 0);
}

void enableMotors()
{
    digitalWrite(pinSTBY, HIGH);
}

void disableMotors()
{
    digitalWrite(pinSTBY, LOW);
}
