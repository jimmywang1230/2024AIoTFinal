#include <Wire.h>
#include <Arduino.h>
#include <LiquidCrystal_PCF8574.h>
#include <SoftwareSerial.h>

LiquidCrystal_PCF8574 lcd(0x27);


char receivedShowupList[3];
int length_receivedShowupList;
int i_lcd = 0; // Index for scrolling the LCD
unsigned long previousMillis = 0; // will store last time LED was updated
const long interval = 100; // interval at which to scroll the LCD
bool waitForCanShowUpList, requestForActualShowList;
String FirstLine = "WWelcome to NTUST LiFaUan";
String canShowupList = "SShowup List: ";
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


void setup() {
  // Wire.begin(8);                // 加入I2C總線，設定地址為8
  // Wire.onReceive(receiveEvent); // 註冊接收事件
  // Wire.onRequest(requestEvent); // 註冊請求事件
  Serial.begin(9600);           // 開始序列通信
  lcd.begin(16, 2); // 初始化LCD
  lcd.setBacklight(255);
  lcd.clear();
  waitForCanShowUpList = true;
  requestForActualShowList = true;    // FOR TESTING！！！！
}

void loop() {
  delay(100);
  unsigned long currentMillis = millis();
  if (waitForCanShowUpList) {
    serial_blink();
    // Serial.print("waitForCanShowUpList:");
    // Serial.println(waitForCanShowUpList);
  } else if(!waitForCanShowUpList && requestForActualShowList)  {
    requestEvent();
    requestForActualShowList = false;
  }
  // Show the showup list on the LCD
  if(length_receivedShowupList == 3) {
    if (receivedShowupList[0] == 1)
    {
      canShowupList += P1.name + " ";
    } else if (receivedShowupList[1] == 1)
    {
      canShowupList += P2.name + " ";
    } else if (receivedShowupList[2] == 1)
    {
      canShowupList += P3.name + " ";
    }
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      // Move the start index of the substring
      i_lcd = 0;
      if (i_lcd <= max(FirstLine.length(), canShowupList.length())) {
        lcd.setCursor(0, 0);
        lcd.print(FirstLine.substring(i_lcd + 1, FirstLine.length()));
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(canShowupList.substring(i_lcd + 1, canShowupList.length()));
        lcd.print(" ");
        i_lcd++;
      } else {
        i_lcd = 0; // Reset the index to start scrolling from the beginning
      }
    }
    
  }
}

// 請求事件處理函數
void requestEvent() {
  Serial.print(receivedShowupList);            // 當主設備請求數據時發送"World"字串
}

void serial_blink() {
  int i = 0; // Declare i
  if(Serial.available() && i < 3) {
    while (i < 3) {
      char x = Serial.read();
      receivedShowupList[i] = x;
      i++;
    }
    length_receivedShowupList = sizeof(receivedShowupList) / sizeof(receivedShowupList[0]);
    waitForCanShowUpList = false;
  }
}
// void receiveEvent(int howMany) {
//   for (int i = 0; i < howMany; i++) {
//     char x = Wire.read();
//     receivedShowupList[i] = x;
//   }
//   Serial.println("Received showup list: ");
//   for (int i = 0; i < howMany; i++) {
//     Serial.println(receivedShowupList[i]);
//   }
//   length_receivedShowupList = sizeof(receivedShowupList) / sizeof(receivedShowupList[0]);
// }
