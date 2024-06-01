#include <Arduino.h>
#include <Servo.h>

char memberList[4] = "101";
char memberDetection[4] = "000";
byte count = 0;
byte servoPos = 0;

byte servoPin = 9;
Servo servo;

void turnOffLED();
void checkAttendance();
void openGate();
void closeGate();

void setup() {
    Serial.begin(9600);

    pinMode(12, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(10, OUTPUT);

    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
    digitalWrite(10, LOW);

    servo.attach(servoPin);
    servo.write(0);
    delay(1000);
    servo.detach();
}

void loop() {
    turnOffLED();

    count = 0;
    while (Serial.available() > 0) {
        int inByte = Serial.read();
        Serial.println(inByte);
        if (inByte == '1' || inByte == '0') {
            memberDetection[count] = (char)inByte;
            count++;
            Serial.print("char: ");
            Serial.println((char)inByte);
            Serial.print("memberDetection: ");
            Serial.println(memberDetection);
        }
    }

    checkAttendance();

    delay(1000);
}

void turnOffLED() {
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
}

void checkAttendance() {
    // english come in
    if (memberDetection[0] == '1' && memberList[0] == '1') {
        digitalWrite(10, HIGH);
        openGate();
        delay(1000);
        closeGate();
    } else {
        digitalWrite(10, LOW);
        memberDetection[0] = '0';
    }

    // fish come in
    if (memberDetection[1] == '1' && memberList[1] == '1') {
        digitalWrite(11, HIGH);
        openGate();
        delay(1000);
        closeGate();
    } else {
        digitalWrite(11, LOW);
        memberDetection[1] = '0';
    }

    // kp come in
    if (memberDetection[2] == '1' && memberList[2] == '1') {
        digitalWrite(12, HIGH);
        openGate();
        delay(1000);
        closeGate();
    } else {
        digitalWrite(12, LOW);
        memberDetection[2] = '0';
    }
}

void openGate() {
    servo.attach(servoPin);
    for (servoPos = 0; servoPos < 90; servoPos += 1) {
        servo.write(servoPos);
        delay(15);
    }
    servo.detach();
}

void closeGate() {
    servo.attach(servoPin);
    for (servoPos = 90; servoPos > 0; servoPos -= 1) {
        servo.write(servoPos);
        delay(15);
    }
    servo.detach();
}