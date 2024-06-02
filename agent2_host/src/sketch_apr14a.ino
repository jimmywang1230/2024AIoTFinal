// #include <Wire.h>               // I2C函式庫
#include <LiquidCrystal_PCF8574.h>
#include <SoftwareSerial.h>

SoftwareSerial btSerial(3, 4);  // TX, RX
LiquidCrystal_PCF8574 lcd(0x27);

const int P1AgreePin = 13;
const int P1DisagreePin = 12;
const int P2AgreePin = 11;
const int P2DisagreePin = 10;
const int P3AgreePin = 9;
const int P3DisagreePin = 8;
const int BuzzerPin = 7;
int i_lcd = 0;                     // Index for scrolling the LCD
unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 100;         // interval at which to scroll the LCD
char receivedShowupList[3];
int length_receivedShowupList;

class Participant {
   public:
    String name;
    int AgreeBtnSt;
    int DisagreeBtnSt;
    char Showup;
    int resistor;

    Participant() {
        name = "";
        AgreeBtnSt = 0;
        DisagreeBtnSt = 0;
        Showup = '0';
        resistor = 0;
    }
};

Participant P1, P2, P3;

boolean inVoteMode;    // 投票模式
boolean inSigninMode;  // 投票模式

String FirstLine = "WWelcome to NTUST LiFaUan";
String actualShowupList = "SShowup List: ";
String Alarm = "ALERT: ";
String Alarm2 = "No show but vote";

void setup() {
    pinMode(13, OUTPUT);
    pinMode(BuzzerPin, OUTPUT);  // digitalWrite(7,HIGH) or LOW;
    pinMode(P1AgreePin, INPUT);
    pinMode(P1DisagreePin, INPUT);
    pinMode(P2AgreePin, INPUT);
    pinMode(P2DisagreePin, INPUT);
    pinMode(P3AgreePin, INPUT);
    pinMode(P3DisagreePin, INPUT);
    // Wire.begin();           // 加入I2C總線

    // 設定參與者名字
    P1.name = "English Tsai";
    P2.name = "KP";
    P3.name = "Korea Fish";
    P1.Showup = '0', P2.Showup = '0', P3.Showup = '0';
    inSigninMode = true;

    Serial.begin(9600);
    btSerial.begin(9600);

    inVoteMode = false;
    lcd.begin(16, 2);  // 初始化LCD
    lcd.setBacklight(255);
    lcd.clear();
}

void loop() {
    unsigned long currentMillis = millis();

    // Serial.println(inSigninMode);

    if (inSigninMode) {
        setCanShowupList();
        waitTransmissionForShowupList();
        // Serial.println(actualShowupList);
    }

    if (!inSigninMode && actualShowupList != "SShowup List: ") {
        if (btSerial.available()) {   // 如果藍牙有傳資料過來
            int i = btSerial.read();  // 把讀到的資料丟給i
            // Serial.println(i);
            lcd.clear();
            if (i == 1) {
                inVoteMode = true;
                bool result = voteProcess();
                inVoteMode = result;
                // Serial.println(result);
                delay(2000);
            } else if (i == 2) {
                inVoteMode = false;
            }
        }

        if (!inVoteMode) {
            if (currentMillis - previousMillis >= interval) {
                previousMillis = currentMillis;
                // Move the start index of the substring
                // i_lcd = 0;
                lcd.begin(16, 2);
                lcd.clear();
                if (i_lcd <= max(FirstLine.length(), actualShowupList.length())) {
                    lcd.setCursor(0, 0);
                    lcd.print(FirstLine.substring(i_lcd + 1, FirstLine.length()));
                    lcd.print(" ");
                    lcd.setCursor(0, 1);
                    lcd.print(actualShowupList.substring(i_lcd + 1, actualShowupList.length()));
                    lcd.print(" ");
                    i_lcd++;
                } else {
                    i_lcd = 0;  // Reset the index to start scrolling from the beginning
                }
            }
        }
    }
    delay(1000);
}

bool voteProcess() {
    // checkParticipantSensor();
    String voteResult = "";
    String agreeVote = "";
    String disagreeVote = "";
    int agreeCount = 0;
    int disagreeCount = 0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bill 1 Vote");
    delay(1000);
    // Start the countdown
    unsigned long startTime = millis();
    int countdown = 10;  // 10 seconds
    while (countdown >= 0) {
        // Check if a second has passed
        checkParticipantSensor();
        if (millis() - startTime >= 1000) {
            // 如果有人不應該來在位置上，而且還投票 蜂鳴器就會響
            if (P1.Showup == '0' && P1.resistor < 600) {
                if (P1.AgreeBtnSt == 1 || P1.DisagreeBtnSt == 1) {
                    triggeredAlarm(P1.name);
                    return false;
                }
            }
            if (P2.Showup == '0' && P2.resistor < 600) {
                if (P2.AgreeBtnSt == 1 || P2.DisagreeBtnSt == 1) {
                    triggeredAlarm(P2.name);
                    return false;
                }
            }
            if (P3.Showup == '0' && P3.resistor < 600) {
                if (P3.AgreeBtnSt == 1 || P3.DisagreeBtnSt == 1) {
                    triggeredAlarm(P3.name);
                    return false;
                }
            }
            // 參與者投票紀錄
            if (P1.AgreeBtnSt == 1 && P1.DisagreeBtnSt == 0) {
                if (!checkAlreadyVote(P1.name, agreeVote, disagreeVote)) {
                    agreeVote += P1.name + " ";
                }
            } else if (P1.AgreeBtnSt == 0 && P1.DisagreeBtnSt == 1) {
                if (!checkAlreadyVote(P1.name, agreeVote, disagreeVote)) {
                    disagreeVote += P1.name + " ";
                }
            } else if (P2.AgreeBtnSt == 1 && P2.DisagreeBtnSt == 0) {
                if (!checkAlreadyVote(P2.name, agreeVote, disagreeVote)) {
                    agreeVote += P2.name + " ";
                }
            } else if (P2.AgreeBtnSt == 0 && P2.DisagreeBtnSt == 1) {
                if (!checkAlreadyVote(P2.name, agreeVote, disagreeVote)) {
                    disagreeVote += P2.name + " ";
                }
            } else if (P3.AgreeBtnSt == 1 && P3.DisagreeBtnSt == 0) {
                if (!checkAlreadyVote(P3.name, agreeVote, disagreeVote)) {
                    agreeVote += P3.name + " ";
                }
            } else if (P3.AgreeBtnSt == 0 && P3.DisagreeBtnSt == 1) {
                if (!checkAlreadyVote(P3.name, agreeVote, disagreeVote)) {
                    disagreeVote += P3.name + " ";
                }
            }

            lcd.setCursor(0, 1);
            lcd.print("Time left: ");
            lcd.setCursor(10, 1);  // Move cursor back to the start of the countdown
            if (countdown < 10) {
                lcd.print("0");  // Print a leading zero for single digit numbers
            }
            lcd.print(countdown);
            lcd.print("  ");  // Print extra spaces to clear any remaining characters
            countdown--;
            startTime = millis();
        }
    }

    if (P1.Showup == '1' || P2.Showup == '1' || P3.Showup == '1') {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Agree Vote: ");
        lcd.print(agreeVote);
        lcd.setCursor(0, 1);
        lcd.print("Disagree Vote: ");
        lcd.print(disagreeVote);
        voteResult = "Agree Vote: " + agreeVote + " Disagree Vote: " + disagreeVote;
        delay(3000);
    } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("No one show up");
        voteResult = "no show up";
    }
    // Serial.println(voteResult);
    lcd.begin(16, 2);
    inVoteMode = false;
    return inVoteMode;
}

bool checkAlreadyVote(String name, String agreeVote, String disagreeVote) {
    if (agreeVote.indexOf(name) != -1) {
        return true;  // The name is in the agreeVote list
    }

    if (disagreeVote.indexOf(name) != -1) {
        return true;  // The name is in the disagreeVote list
    }
    return false;
}

// 顯示出席名單
void setCanShowupList() {
    checkParticipantSensor();
    char sendCanShowupList[] = "000";

    // 10秒輸入可以出席的人
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("set valid Att");
    delay(1000);
    // Start the countdown
    unsigned long startTime = millis();
    int countdown = 10;  // 10 seconds
    while (countdown >= 0) {
        // Check if a second has passed
        checkParticipantSensor();
        if (millis() - startTime >= 1000) {
            if (P1.Showup == '0' && P1.AgreeBtnSt == 1 && P1.DisagreeBtnSt == 1) {
                P1.Showup = '1';
            } else if (P2.Showup == '0' && P2.AgreeBtnSt == 1 && P2.DisagreeBtnSt == 1) {
                P2.Showup = '1';
            } else if (P3.Showup == '0' && P3.AgreeBtnSt == 1 && P3.DisagreeBtnSt == 1) {
                P3.Showup = '1';
            }

            if (P1.Showup == '1') {
                sendCanShowupList[0] = '1';
                P1.Showup = '2';
                delay(500);
            } else if (P2.Showup == '1') {
                sendCanShowupList[1] = '1';
                P2.Showup = '2';
                delay(500);
            } else if (P3.Showup == '1') {
                sendCanShowupList[2] = '1';
                P3.Showup = '2';
                delay(500);
            }
            lcd.setCursor(0, 1);
            lcd.print("Time left: ");
            lcd.setCursor(10, 1);  // Move cursor back to the start of the countdown
            if (countdown < 10) {
                lcd.print("0");  // Print a leading zero for single digit numbers
            }
            lcd.print(countdown);
            lcd.print("  ");  // Print extra spaces to clear any remaining characters
            countdown--;
            startTime = millis();
        }

        // if (countdown== -1 && sendCanShowupList != "000") {
        //   // 透過I2C傳送資料
        //   Wire.beginTransmission(8); // 開始與地址為8的裝置通信
        //   Wire.print(sendCanShowupList);
        //   Wire.endTransmission();    // 結束傳輸
        //   delay(500);
        // }
        if (countdown == -1 && sendCanShowupList != "000") {
            // 透過I2C傳送資料
            Serial.print(sendCanShowupList);
            delay(500);
        }
    }
    delay(200);
}

void waitTransmissionForShowupList() {
    int i = 0;  // Declare i
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wait for Att");
    lcd.setCursor(0, 1);
    lcd.print("Showup List");
    while (!Serial.available() && i > 3) {
      lcd.noDisplay();
      delay(500);
      lcd.display();
      delay(500);
    }
    while (Serial.available() && i < 3) {
        char x = Serial.read();
        receivedShowupList[i] = x;
        i++;
    }
    length_receivedShowupList = sizeof(receivedShowupList) / sizeof(receivedShowupList[0]);
    if (length_receivedShowupList == 3) {
        P1.Showup = receivedShowupList[0];
        P2.Showup = receivedShowupList[1];
        P3.Showup = receivedShowupList[2];

        if (P1.Showup == '1') {
            actualShowupList += P1.name + " ";
        }
        if (P2.Showup == '1') {
            actualShowupList += P2.name + " ";
        }
        if (P3.Showup == '1') {
            actualShowupList += P3.name + " ";
        }

        inSigninMode = false;
    }
}

// 有人不在位卻投票時，觸發警報
void triggeredAlarm(String Person) {
    lcd.clear();
    digitalWrite(BuzzerPin, HIGH);
    delay(1000);
    digitalWrite(BuzzerPin, LOW);

    // lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(Alarm + Person);
    lcd.setCursor(0, 1);
    lcd.print(Alarm2);

    for (int i = 0; i <= 5; i++) {
        lcd.display();
        delay(500);
        lcd.noDisplay();
        delay(500);
    }
    lcd.display();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VOTE END");
    delay(2000);
    lcd.begin(16, 2);
}

void checkParticipantSensor() {
    P1.resistor = analogRead(A0);
    P2.resistor = analogRead(A1);
    P3.resistor = analogRead(A2);
    P1.AgreeBtnSt = digitalRead(P1AgreePin);
    P1.DisagreeBtnSt = digitalRead(P1DisagreePin);
    P2.AgreeBtnSt = digitalRead(P2AgreePin);
    P2.DisagreeBtnSt = digitalRead(P2DisagreePin);
    P3.AgreeBtnSt = digitalRead(P3AgreePin);
    P3.DisagreeBtnSt = digitalRead(P3DisagreePin);
}