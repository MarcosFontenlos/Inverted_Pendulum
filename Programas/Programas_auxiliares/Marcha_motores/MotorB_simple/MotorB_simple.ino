const int pinBIN1 = 33; // Pin para controlar el sentido del motor B
const int pinBIN2 = 32; // Pin para controlar el sentido del motor B
const int pinPWMB = 26; // Pin para controlar la velocidad del motor B

void setup() {
    Serial.begin(57600);
    pinMode(pinBIN1, OUTPUT);
    Serial.println("pin1");
    pinMode(pinBIN2, OUTPUT);
    Serial.println("pin2");
    pinMode(pinPWMB, OUTPUT);
    Serial.println("pwm");
}

void loop() {
    // Mover hacia adelante
    Serial.println("delante");
    digitalWrite(pinBIN1, HIGH);
    digitalWrite(pinBIN2, LOW);
    analogWrite(pinPWMB, 200); // Velocidad moderada
    delay(2000); // Espera 2 segundos

    // Detener
    Serial.println("stop");
    digitalWrite(pinBIN1, LOW);
    digitalWrite(pinBIN2, LOW);
    analogWrite(pinPWMB, 0);
    delay(1000); // Espera 1 segundo

    // Mover hacia atrás
    Serial.println("atras");
    digitalWrite(pinBIN1, LOW);
    digitalWrite(pinBIN2, HIGH);
    analogWrite(pinPWMB, 200); // Velocidad moderada
    delay(2000); // Espera 2 segundos

    // Detener
    Serial.println("stop");
    digitalWrite(pinBIN1, LOW);
    digitalWrite(pinBIN2, LOW);
    analogWrite(pinPWMB, 0);
    delay(1000); // Espera 1 segundo
}
