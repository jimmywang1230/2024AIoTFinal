#include <Arduino.h>
#include <Servo.h>

char attendanceList[4] = "101";
char showUpList[4] = "000";
byte count = 0;
byte servoPos = 0;

byte servoPin = 9;
Servo servo;

void checkAttendance();
void openGate();
void closeGate();

void setup() {
    Serial.begin(9600);

    servo.attach(servoPin);
    servo.write(0);
    delay(1000);
    servo.detach();
}

void loop() {

    count = 0;
    while (Serial.available() > 0) {
        int inByte = Serial.read();
        Serial.println(inByte);
        if (inByte == '1' || inByte == '0') {
            showUpList[count] = (char)inByte;
            count++;
            Serial.print("char: ");
            Serial.println((char)inByte);
            Serial.print("memberDetection: ");
            Serial.println(showUpList);
        }
    }

    checkAttendance();

    delay(1000);
}

void checkAttendance() {
    // english come in
    if (showUpList[0] == '1' && attendanceList[0] == '1') {
        digitalWrite(10, HIGH);
        openGate();
        delay(1000);
        closeGate();
    } else {
        digitalWrite(10, LOW);
        showUpList[0] = '0';
    }

    // fish come in
    if (showUpList[1] == '1' && attendanceList[1] == '1') {
        digitalWrite(11, HIGH);
        openGate();
        delay(1000);
        closeGate();
    } else {
        digitalWrite(11, LOW);
        showUpList[1] = '0';
    }

    // kp come in
    if (showUpList[2] == '1' && attendanceList[2] == '1') {
        digitalWrite(12, HIGH);
        openGate();
        delay(1000);
        closeGate();
    } else {
        digitalWrite(12, LOW);
        showUpList[2] = '0';
    }
}

void openGate() {
    servo.attach(servoPin);
    for (servoPos = 0; servoPos < 90; servoPos += 1) {
        servo.write(servoPos);
        delay(10);
    }
    servo.detach();
}

void closeGate() {
    servo.attach(servoPin);
    for (servoPos = 90; servoPos > 0; servoPos -= 1) {
        servo.write(servoPos);
        delay(10);
    }
    servo.detach();
}