#include <Arduino.h>
#include <Servo.h>

char attendanceList[4] = "111";
byte showUpList = 0;
byte count = 0;
byte servoPos = 0;

byte servoPin = 9;
Servo servo;

void checkAttendance();
void openGate();
void closeGate();
void mainProcess();
void test();

void setup() {
    Serial.begin(9600);

    // init led
    pinMode(12, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(7, OUTPUT);
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
    digitalWrite(10, LOW);
    digitalWrite(7, LOW);

    // init servo
    servo.attach(servoPin);
    servo.write(0);
    delay(1000);
    servo.detach();
}

void loop() {
    mainProcess();
    // test();
    delay(100);
}

void test() {
    int num = 0;
    int idx = 0;

    // if "49" from python, it gets "4" then "9"
    while (true) {
        while (Serial.available() > 0) {
            byte inByte = Serial.read();
            Serial.println(inByte);
            if (idx == 0) {
                num = (inByte - '0') * 10;
                idx++;
            } else {
                num += (inByte - '0');
            }
            Serial.println(num);        // here num is 49 in INT
            Serial.println((char)num);  // here num is 1 in CHAR
        }
    }
}

void mainProcess() {
    showUpList = 0;  // reset show up list
    int num = 0;

    // get show up list from camera
    while (true) {
        while (Serial.available() > 0) {
            byte inByte = Serial.read();
            Serial.println(inByte);
            num = (inByte - '0');
        }
        if (num != 0) {
            showUpList = num;
            checkAttendance();  // open gate and turn on led
        }
        num = 0;
    }
}

/**
 * @brief compare showup list to attendance List, open the gate if they are the same
 * @details char attendanceList[4] = "101" is defined
 */
void checkAttendance() {
    // english come in, condition 4
    if ((showUpList >> 2) % 2 == 1 && attendanceList[0] == '1') {
        digitalWrite(12, HIGH);  // turn on led
        openGate();
        delay(1000);
        closeGate();
        digitalWrite(12, LOW);  // turn off led
    }

    // kp come in, condition 2
    if ((showUpList >> 1) % 2 == 1 && attendanceList[1] == '1') {
        digitalWrite(11, HIGH);  // turn on led
        openGate();
        delay(1000);
        closeGate();
        digitalWrite(11, LOW);  // turn off led
    }

    // fish come in, condition 1
    if (showUpList % 2 == 1 && attendanceList[2] == '1') {
        digitalWrite(10, HIGH);  // turn on led
        openGate();
        delay(1000);
        closeGate();
        digitalWrite(10, LOW);  // turn off led
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