#include <Arduino.h>
#include <LiquidCrystal_PCF8574.h>
#include <Servo.h>
#include <SoftwareSerial.h>

#define ENTRY_INTERVAL 20000
#define LCD_INTERVAL 100

// declare global variables
char receivedAttendanceList[3] = {'1', '1', '1'};
byte showUpList = 0;
unsigned long currentMillis = 0;
int length_receivedAttendanceList;
int i_lcd = 0;                     // Index for scrolling the LCD
unsigned long previousMillis = 0;  // will store last time LED was updated
bool waitForAttendanceList = true;
bool requestForShowUpList = true;
String FirstLine = "WWelcome to NTUST LiFaUan";
String attendanceList = "SShowup List: ";
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

    // init servo
    servo.attach(servoPin);
    servo.write(0);
    delay(1000);
    servo.detach();
}

void loop() {
    currentMillis = millis();
    // getAttendanceList();   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // showAttendanceList();  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // getShowUpListBT();     //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (waitForAttendanceList) {
        getAttendanceList();
        showAttendanceList();
    } else if (!waitForAttendanceList && requestForShowUpList) {
        getShowUpListBT();
        sendShowUpList();
        requestForShowUpList = false;
    }

    // Show the showup list on the LCD
    // if (length_receivedAttendanceList == 3) {
    //     if (receivedAttendanceList[0] == 1) {
    //         attendanceList += P1.name + " ";
    //     } else if (receivedAttendanceList[1] == 1) {
    //         attendanceList += P2.name + " ";
    //     } else if (receivedAttendanceList[2] == 1) {
    //         attendanceList += P3.name + " ";
    //     }
    //     if (currentMillis - previousMillis >= LCD_INTERVAL) {
    //         previousMillis = currentMillis;
    //         // Move the start index of the substring
    //         i_lcd = 0;
    //         if (i_lcd <= max(FirstLine.length(), attendanceList.length())) {
    //             lcd.setCursor(0, 0);
    //             lcd.print(FirstLine.substring(i_lcd + 1, FirstLine.length()));
    //             lcd.print(" ");
    //             lcd.setCursor(0, 1);
    //             lcd.print(attendanceList.substring(i_lcd + 1, attendanceList.length()));
    //             lcd.print(" ");
    //             i_lcd++;
    //         } else {
    //             i_lcd = 0;  // Reset the index to start scrolling from the beginning
    //         }
    //     }
    // }

    delay(100);
}

void getShowUpList() {
    showUpList = 0;  // reset show up list
    int num = 0;

    // get show up list from camera
    while (true) {
        while (Serial.available() > 0) {
            byte inByte = Serial.read();
            Serial.println(inByte);
            num = (inByte - '0');
        }

        lcd.setCursor(0, 0);
        lcd.print(num);
        lcd.print(" ");

        if (num != 0) {
            showUpList = num;
            checkAttendance();  // open gate and turn on led
        }
        num = 0;
        lcd.clear();
    }
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

        lcd.setCursor(0, 0);
        lcd.print(num);
        lcd.print(" ");

        if (num != 0) {
            showUpList = num;
            checkAttendance();  // open gate and turn on led
        }
        num = 0;
        lcd.clear();
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
    char showUpListToSend[3];

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
            length_receivedAttendanceList = sizeof(receivedAttendanceList) / sizeof(receivedAttendanceList[0]);
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