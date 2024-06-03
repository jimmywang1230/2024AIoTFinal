#include <Arduino.h>
#include <LiquidCrystal_PCF8574.h>
#include <Servo.h>
#include <SoftwareSerial.h>

#define ENTRY_INTERVAL 10000
#define LCD_INTERVAL 100

// declare global variables
char receivedAttendanceList[3] = {'1', '1', '1'};
byte showUpList = 0;              // received camera via BT
unsigned long currentMillis = 0;  // Index for scrolling the LCD
bool waitForAttendanceList = true;
bool requestForShowUpList = true;
// String FirstLine = "Can Attendance";
char FirstLine[] = "Can Attendance";
String attendanceList = " ";
long entryMillis = 0;
byte servoPos = 0;

class Participant {
   public:
    String name;
    int AgreeBtnSt;
    int DisagreeBtnSt;
    bool Showup;
    int resistor;

    Participant() {
        name = "";
        Showup = false;
    }
};

Participant P1, P2, P3;

// declare the pins

byte englishPin = 12;
byte kpPin = 11;
byte fishPin = 10;
byte servoPin = 9;
byte entryAvailablePin = 7;

// declare modules

LiquidCrystal_PCF8574 lcd(0x27);
Servo servo;
SoftwareSerial BTserial(2, 3);  // SoftwareSerial(RX, TX) on arduino board

// declare functions

void checkAttendance();
void getAttendanceList();
void getShowUpListBT();
void showAttendanceList();
void sendShowUpList();
void openGate();
void closeGate();
void getShowUpList();
void test();

void setup() {
    Serial.begin(9600);
    BTserial.begin(38400);

    // init led
    pinMode(12, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(7, OUTPUT);
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
    digitalWrite(10, LOW);
    digitalWrite(7, LOW);

    // init LCD
    lcd.begin(16, 2);
    lcd.setBacklight(255);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("hello");

    // init servo
    servo.attach(servoPin);
    servo.write(0);
    delay(1000);
    servo.detach();

    // 設定參與者名字
    P1.name = "Eng";
    P2.name = "KP";
    P3.name = "Fish";
    P1.Showup = false, P2.Showup = false, P3.Showup = false;
}

void loop() {
    currentMillis = millis();
    if (waitForAttendanceList) {
        getAttendanceList();
        showAttendanceList();

        // Show the showup list on the LCD
        if (receivedAttendanceList[0] == '1') {
            attendanceList += P1.name + " ";
            receivedAttendanceList[0] == '2';
        }
        if (receivedAttendanceList[1] == '1') {
            attendanceList += P2.name + " ";
            receivedAttendanceList[1] == '2';
        }
        if (receivedAttendanceList[2] == '1') {
            attendanceList += P3.name + " ";
            receivedAttendanceList[2] == '2';
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(String(FirstLine));
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(attendanceList);
        lcd.print(" ");

    } else if (!waitForAttendanceList && requestForShowUpList) {
        getShowUpListBT();
        sendShowUpList();
        requestForShowUpList = false;
    }

    delay(1000);
}

void getShowUpListBT() {
    showUpList = 0;  // reset show up list
    int num = 0;

    // get show up list from camera
    while (millis() - currentMillis < ENTRY_INTERVAL) {
        while (BTserial.available() > 0) {
            byte inByte = BTserial.read();
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
 *
 */
void checkAttendance() {
    // english come in, condition 4
    if ((showUpList >> 2) % 2 == 1 && receivedAttendanceList[0] == '1') {
        P1.Showup = true;
        openGate();
        delay(1000);
        closeGate();
        digitalWrite(englishPin, LOW);  // turn off led
    }

    // kp come in, condition 2
    if ((showUpList >> 1) % 2 == 1 && receivedAttendanceList[1] == '1') {
        P2.Showup = true;
        openGate();
        delay(1000);
        closeGate();
        digitalWrite(kpPin, LOW);  // turn off led
    }

    // fish come in, condition 1
    if (showUpList % 2 == 1 && receivedAttendanceList[2] == '1') {
        P3.Showup = true;
        openGate();
        delay(1000);
        closeGate();
        digitalWrite(fishPin, LOW);  // turn off led
    }
}

/**
 * @brief send showup list to agent2
 */
void sendShowUpList() {
    char showUpListToSend[4] = "000";

    // english show up
    if (P1.Showup) {
        showUpListToSend[0] = '1';
    } else {
        showUpListToSend[0] = '0';
    }

    // kp show up
    if (P2.Showup) {
        showUpListToSend[1] = '1';
    } else {
        showUpListToSend[1] = '0';
    }

    // fish show up
    if (P3.Showup) {
        showUpListToSend[2] = '1';
    } else {
        showUpListToSend[2] = '0';
    }

    Serial.print(showUpListToSend);
    // lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.print("Show up list:");
    // lcd.setCursor(0, 1);
    // lcd.print(String(showUpListToSend));


    delay(1000);
}

/**
 * @brief get attendace list from agent2 to agent1
 *
 */
void getAttendanceList() {
    while (waitForAttendanceList) {
        int i = 0;  // Declare i
        if (Serial.available() && i < 3) {
            while (i < 3) {
                char x = Serial.read();

                // to avoid 13 and 10
                if (x == '1' || x == '0') {
                    receivedAttendanceList[i] = x;
                    i++;
                }
            }
            waitForAttendanceList = false;
        }
    }
}

/**
 * @brief show attendance with LED
 */
void showAttendanceList() {
    // english need attendance
    if (receivedAttendanceList[0] == '1') {
        digitalWrite(englishPin, HIGH);
    } else {
        digitalWrite(englishPin, LOW);
    }

    // kp need attendance
    if (receivedAttendanceList[1] == '1') {
        digitalWrite(kpPin, HIGH);
    } else {
        digitalWrite(kpPin, LOW);
    }

    // fish need attendance
    if (receivedAttendanceList[2] == '1') {
        digitalWrite(fishPin, HIGH);
    } else {
        digitalWrite(fishPin, LOW);
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