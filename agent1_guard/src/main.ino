#include <Arduino.h>
#include <LiquidCrystal_PCF8574.h>
#include <Servo.h>
#include <SoftwareSerial.h>

LiquidCrystal_PCF8574 lcd(0x27);

char receivedAttendanceList[3];
char showUpList[4] = "000";
int length_receivedAttendanceList;
int i_lcd = 0;                     // Index for scrolling the LCD
unsigned long previousMillis = 0;  // will store last time LED was updated
const long LCD_INTERVAL = 100;     // interval at which to scroll the LCD
bool waitForAttendanceList = true;
bool requestForShowUpList = true;
String FirstLine = "WWelcome to NTUST LiFaUan";
String attendanceList = "SShowup List: ";

byte servoPos = 0;
byte servoPin = 9;
Servo servo;

class Participant {
   public:
    String name;
    int AgreeBtnSt;
    int DisagreeBtnSt;
    int Showup;
    int resistor;

    Participant() {
        name = "";
        Showup = 0;
    }
};

Participant P1, P2, P3;

void checkAttendance();
void openGate();
void closeGate();

void setup() {
    Serial.begin(9600);

    lcd.begin(16, 2);  // init LCD
    lcd.setBacklight(255);
    lcd.clear();

    servo.attach(servoPin);
    servo.write(0);
    delay(1000);
    servo.detach();
}

void loop() {
    delay(100);
    unsigned long currentMillis = millis();

    if (waitForAttendanceList) {
        getAttendanceList();
        // Serial.print("waitForAttendanceList:");
        // Serial.println(waitForAttendanceList);
    } else if (!waitForAttendanceList && requestForShowUpList) {
        getShowUpList();
        requestForShowUpList = false;
    }

    // Show the showup list on the LCD
    if (length_receivedAttendanceList == 3) {
        if (receivedAttendanceList[0] == 1) {
            attendanceList += P1.name + " ";
        } else if (receivedAttendanceList[1] == 1) {
            attendanceList += P2.name + " ";
        } else if (receivedAttendanceList[2] == 1) {
            attendanceList += P3.name + " ";
        }
        if (currentMillis - previousMillis >= LCD_INTERVAL) {
            previousMillis = currentMillis;
            // Move the start index of the substring
            i_lcd = 0;
            if (i_lcd <= max(FirstLine.length(), attendanceList.length())) {
                lcd.setCursor(0, 0);
                lcd.print(FirstLine.substring(i_lcd + 1, FirstLine.length()));
                lcd.print(" ");
                lcd.setCursor(0, 1);
                lcd.print(attendanceList.substring(i_lcd + 1, attendanceList.length()));
                lcd.print(" ");
                i_lcd++;
            } else {
                i_lcd = 0;  // Reset the index to start scrolling from the beginning
            }
        }
    }
}

/**
 * @brief get showup list from camera
 *
 */
void getShowUpList() {
    int index = 0;
    while (Serial.available() > 0) {
        int inByte = Serial.read();
        Serial.println(inByte);
        if (inByte == '1' || inByte == '0') {
            showUpList[index] = (char)inByte;
            index++;
        }
    }

    checkAttendance();
    Serial.print(receivedAttendanceList);
}

/**
 * @brief get attendace list from agent2 to agent1
 *
 */
void getAttendanceList() {
    int i = 0;  // Declare i
    if (Serial.available() && i < 3) {
        while (i < 3) {
            char x = Serial.read();
            receivedAttendanceList[i] = x;
            i++;
        }
        length_receivedAttendanceList = sizeof(receivedAttendanceList) / sizeof(receivedAttendanceList[0]);
        waitForAttendanceList = false;
    }
}

// void receiveEvent(int howMany) {
//   for (int i = 0; i < howMany; i++) {
//     char x = Wire.read();
//     receivedAttendanceList[i] = x;
//   }
//   Serial.println("Received showup list: ");
//   for (int i = 0; i < howMany; i++) {
//     Serial.println(receivedAttendanceList[i]);
//   }
//   length_receivedAttendanceList = sizeof(receivedAttendanceList) / sizeof(receivedAttendanceList[0]);
// }

/**
 * @brief compare showup list to attendance List, open the gate if they are the same
 */
void checkAttendance() {
    // english come in
    if (showUpList[0] == '1' && receivedAttendanceList[0] == '1') {
        openGate();
        delay(1000);
        closeGate();
    } else {
        showUpList[0] = '0';
    }

    // kp come in
    if (showUpList[1] == '1' && receivedAttendanceList[1] == '1') {
        openGate();
        delay(1000);
        closeGate();
    } else {
        showUpList[2] = '0';
    }

    // fish come in
    if (showUpList[2] == '1' && receivedAttendanceList[2] == '1') {
        openGate();
        delay(1000);
        closeGate();
    } else {
        showUpList[1] = '0';
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