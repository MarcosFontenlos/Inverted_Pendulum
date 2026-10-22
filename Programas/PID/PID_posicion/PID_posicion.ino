#include <WiFi.h>
#include <Wire.h>
#include <MPU6050.h>
#include <I2Cdev.h>

MPU6050 sensor;

const char* ssid = "Control Pendulo invertido";
const char* password = "buseropesetero";

WiFiServer server(80);

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

float Kp = 7; // Ajusta estos 3 valores según sea necesario
float Ki = 0.5;
float Kd = 0;
float integral = 0.0;
float derivative = 0.0;
float previous_error = 0.0;

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

    // Configura el ESP32 en modo AP
    WiFi.softAP(ssid, password);

    Serial.println("Access Point started.");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    server.begin();
}

void loop() {
    WiFiClient client = server.available();
    if (client) {
        Serial.println("New Client.");
        String currentLine = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.write(c);
                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><title>ESP32 Car Control</title>");
                        client.println("<style>");
                        client.println("body { font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }");
                        client.println(".container { text-align: center; }");
                        client.println("button { font-size: 24px; margin: 10px; padding: 20px; border: none; border-radius: 10px; background-color: #4CAF50; color: white; cursor: pointer; }");
                        client.println("button:active { background-color: #45a049; }");
                        client.println("</style>");
                        client.println("<script>");
                        client.println("var intervalID;");
                        client.println("function sendCommand(cmd) {");
                        client.println("var xhttp = new XMLHttpRequest();");
                        client.println("xhttp.open('GET', '/' + cmd, true);");
                        client.println("xhttp.send();}");
                        client.println("function startSending(cmd) {");
                        client.println("intervalID = setInterval(function() { sendCommand(cmd); }, 100); }");
                        client.println("function stopSending() { clearInterval(intervalID); sendCommand('stop'); }");
                        client.println("</script></head><body>");
                        client.println("<div class='container'>");
                        client.println("<h1>ESP32 Car Control</h1>");
                        client.println("<div><button onmousedown=\"startSending('forward')\" onmouseup=\"stopSending()\" ontouchstart=\"startSending('forward')\" ontouchend=\"stopSending()\">&#9650;</button></div>");
                        client.println("<div>");
                        client.println("<button onmousedown=\"startSending('left')\" onmouseup=\"stopSending()\" ontouchstart=\"startSending('left')\" ontouchend=\"stopSending()\">&#9664;</button>");
                        client.println("<button onclick=\"sendCommand('stop')\">&#10074;&#10074;</button>");
                        client.println("<button onmousedown=\"startSending('right')\" onmouseup=\"stopSending()\" ontouchstart=\"startSending('right')\" ontouchend=\"stopSending()\">&#9654;</button>");
                        client.println("</div>");
                        client.println("<div><button onmousedown=\"startSending('backward')\" onmouseup=\"stopSending()\" ontouchstart=\"startSending('backward')\" ontouchend=\"stopSending()\">&#9660;</button></div>");
                        client.println("<div><button onclick=\"sendCommand('shutdown')\">Apagar</button></div>");
                        client.println("<div><button onclick=\"sendCommand('start')\">Encender</button></div>");
                        client.println("</div></body></html>");
                        break;
                    } else {
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }

                if (currentLine.endsWith("GET /forward")) {
                    forward();
                }
                if (currentLine.endsWith("GET /backward")) {
                    backward();
                }
                if (currentLine.endsWith("GET /left")) {
                    left();
                }
                if (currentLine.endsWith("GET /right")) {
                    right();
                }
                if (currentLine.endsWith("GET /stop")) {
                    stop();
                }
                if (currentLine.endsWith("GET /shutdown")) {
                    shutdown();
                }
                if (currentLine.endsWith("GET /start")) {
                    start();
                }
            }
        }
        client.stop();
        Serial.println("Client Disconnected.");
    }

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
        float error = desired_angle - ang_x;
        
        integral += error * dt;
        derivative = (error - previous_error) / dt;
        
        float motor_speed = Kp * error + Ki * integral + Kd * derivative;
        
        enableMotors();
        moveMotor(pinPWMA, pinAIN1, pinAIN2, motor_speed);
        moveMotor(pinPWMB, pinBIN1, pinBIN2, motor_speed);
        
        Serial.print("Rotation in X: ");
        Serial.print(ang_x);
        Serial.print(", Error: ");
        Serial.print(error);
        Serial.print(", Motor Speed: ");
        Serial.println(motor_speed);
        
        previous_error = error;
    }

    delay(10);
}

void forward() {
    digitalWrite(pinAIN1, HIGH);
    digitalWrite(pinAIN2, LOW);
    digitalWrite(pinBIN1, HIGH);
    digitalWrite(pinBIN2, LOW);
    analogWrite(pinPWMA, 30);  // Reducir la velocidad
    analogWrite(pinPWMB, 30);  // Reducir la velocidad
}

void backward() {
    digitalWrite(pinAIN1, LOW);
    digitalWrite(pinAIN2, HIGH);
    digitalWrite(pinBIN1, LOW);
    digitalWrite(pinBIN2, HIGH);
    analogWrite(pinPWMA, 30);  // Reducir la velocidad
    analogWrite(pinPWMB, 30);  // Reducir la velocidad
}

void left() {
    digitalWrite(pinAIN1, HIGH);
    digitalWrite(pinAIN2, LOW);
    digitalWrite(pinBIN1, LOW);
    digitalWrite(pinBIN2, HIGH);
    analogWrite(pinPWMA, 20);  // Reducir la velocidad
    analogWrite(pinPWMB, 20);  // Reducir la velocidad
}

void right() {
    digitalWrite(pinAIN1, LOW);
    digitalWrite(pinAIN2, HIGH);
    digitalWrite(pinBIN1, HIGH);
    digitalWrite(pinBIN2, LOW);
    analogWrite(pinPWMA, 20);  // Reducir la velocidad
    analogWrite(pinPWMB, 20);  // Reducir la velocidad
}

void stop() {
    digitalWrite(pinAIN1, LOW);
    digitalWrite(pinAIN2, LOW);
    digitalWrite(pinBIN1, LOW);
    digitalWrite(pinBIN2, LOW);
    analogWrite(pinPWMA, 0);
    analogWrite(pinPWMB, 0);
}

void shutdown() {
    pidEnabled = false;
    digitalWrite(pinSTBY, LOW);
    stop();
}

void start() {
    pidEnabled = true;
    digitalWrite(pinSTBY, HIGH);
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
