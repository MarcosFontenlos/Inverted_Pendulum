#include <Wire.h>
#include <MPU6050.h>
#include <I2Cdev.h>

MPU6050 sensor;

const int pinPWMA = 25;
const int pinAIN2 = 2;
const int pinAIN1 = 15;
const int pinBIN1 = 33;
const int pinBIN2 = 32;
const int pinPWMB = 26;
const int pinSTBY = 14;

float ang_x = 0, ang_y = 0;
float ang_x_prev = 0, ang_y_prev = 0;
float dt = 0;
long tiempo_prev = 0;

float Kp = 10; // Ajusta estos 3 valores según sea necesario
float Ki = 0.5;
float Kd = 0;

// Coeficientes del controlador PID
float A0, A1, A2;

// Array de errores
float error[3] = {0, 0, 0};

bool pidEnabled = true;

void setup() {
    Serial.begin(115200);
    pinMode(pinPWMA, OUTPUT);
    pinMode(pinAIN2, OUTPUT);
    pinMode(pinAIN1, OUTPUT);
    pinMode(pinPWMB, OUTPUT);
    pinMode(pinBIN1, OUTPUT);
    pinMode(pinBIN2, OUTPUT);
    pinMode(pinSTBY, OUTPUT);

    Wire.begin(21, 22);
    sensor.initialize();

    // Calcula los coeficientes del PID
    A0 = Kp + Ki * dt + Kd / dt;
    A1 = -Kp - 2 * Kd / dt;
    A2 = Kd / dt;
}

void loop() {
    if (pidEnabled) {
        int16_t ax, ay, az;
        int16_t gx, gy, gz;
        sensor.getAcceleration(&ax, &ay, &az);
        sensor.getRotation(&gx, &gy, &gz);
        
        float accel_ang_x = atan(ay / sqrt(pow(ax, 2) + pow(az, 2))) * (180.0 / PI);
        float accel_ang_y = atan(-ax / sqrt(pow(ay, 2) + pow(az, 2))) * (180.0 / PI);
        
        dt = (millis() - tiempo_prev) / 1000.0;
        tiempo_prev = millis();
        ang_x = 0.98 * (ang_x_prev + (gx / 131.0) * dt) + 0.02 * accel_ang_x;
        ang_y = 0.98 * (ang_y_prev + (gy / 131.0) * dt) + 0.02 * accel_ang_y;
        
        ang_x_prev = ang_x;
        ang_y_prev = ang_y;
        
        float desired_angle = 0.0;

        // Actualiza el array de errores
        error[2] = error[1];
        error[1] = error[0];
        error[0] = desired_angle - ang_x;
        
        // Calcula la salida del PID usando la forma IIR
        float motor_speed = A0 * error[0] + A1 * error[1] + A2 * error[2];
        
        enableMotors();
        moveMotor(pinPWMA, pinAIN1, pinAIN2, motor_speed);
        moveMotor(pinPWMB, pinBIN1, pinBIN2, motor_speed);
        
        Serial.print("Rotation in X: ");
        Serial.print(ang_x);
        Serial.print(", Error: ");
        Serial.print(error[0]);
        Serial.print(", Motor Speed: ");
        Serial.println(motor_speed);
    }

    delay(10);
}

void moveMotor(int pinPWM, int pinIN1, int pinIN2, float speed) {
    if (speed > 255) speed = 255;
    if (speed < -255) speed = -255;

    if (speed > 0) {
        digitalWrite(pinIN1, HIGH);
        digitalWrite(pinIN2, LOW);
        analogWrite(pinPWM, speed);
    } else {
        digitalWrite(pinIN1, LOW);
        digitalWrite(pinIN2, HIGH);
        analogWrite(pinPWM, -speed);
    }
}

void enableMotors() {
    digitalWrite(pinSTBY, HIGH);
}

void shutdown() {
    pidEnabled = false;
    digitalWrite(pinSTBY, LOW);
    moveMotor(pinPWMA, pinAIN1, pinAIN2, 0);
    moveMotor(pinPWMB, pinBIN1, pinBIN2, 0);
}

void start() {
    pidEnabled = true;
    digitalWrite(pinSTBY, HIGH);
}
