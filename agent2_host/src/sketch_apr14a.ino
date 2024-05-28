#include <Wire.h>               // I2C函式庫
#include <LiquidCrystal_PCF8574.h>
#include <SoftwareSerial.h>

SoftwareSerial btSerial(3, 4); // TX, RX
LiquidCrystal_PCF8574 lcd(0x27);

const int P1AgreePin = 13; 
const int P1DisagreePin = 12; 
const int P2AgreePin = 11; 
const int P2DisagreePin = 10; 
const int P3AgreePin = 9; 
const int P3DisagreePin = 8; 
const int BuzzerPin = 7; 
int i_lcd = 0; // Index for scrolling the LCD
unsigned long previousMillis = 0; // will store last time LED was updated
const long interval = 100; // interval at which to scroll the LCD

int resistor1, resistor2, resistor3;
int P1AgreeBtnSt, P1DisagreeBtnSt, P2AgreeBtnSt, P2DisagreeBtnSt, P3AgreeBtnSt, P3DisagreeBtnSt;
int P1Showup, P2Showup, P3Showup;   // 可以出現在議場的立委0代表不行1可以
boolean inVoteMode; // 投票模式


String FirstLine = "WWelcome to NTUST LiFaUan";
String ShowupList = "SShowup List: ";
String Alarm = "ALERT: ";
String Alarm2 = "No show but vote";

String P1 = "English Tsai";
String P2 = "KP";
String P3 = "Korea Han";

void setup()
{
  pinMode(13,OUTPUT);
  pinMode(BuzzerPin,OUTPUT); //digitalWrite(7,HIGH) or LOW;
  pinMode(P1AgreePin, INPUT);
  pinMode(P1DisagreePin, INPUT);
  pinMode(P2AgreePin, INPUT);
  pinMode(P2DisagreePin, INPUT);
  pinMode(P3AgreePin, INPUT);
  pinMode(P3DisagreePin, INPUT);
  Serial.begin(9600);
  btSerial.begin(9600);
  P1Showup=0, P2Showup=0, P3Showup=0;
  inVoteMode = false;

  lcd.begin(16, 2); // 初始化LCD
  lcd.setBacklight(255);
  lcd.clear();
}
int prev=0;
void loop()
{
  unsigned long currentMillis = millis();
  resistor1=analogRead(A0); 
  resistor2=analogRead(A1); 
  resistor3=analogRead(A2);
  P1AgreeBtnSt= digitalRead(P1AgreePin);
  P1DisagreeBtnSt= digitalRead(P1DisagreePin);
  P2AgreeBtnSt= digitalRead(P2AgreePin);
  P2DisagreeBtnSt= digitalRead(P2DisagreePin);
  P3AgreeBtnSt= digitalRead(P3AgreePin);
  P3DisagreeBtnSt= digitalRead(P3DisagreePin);

  ShowupList = setShowupList(P1, P2, P3, P1AgreeBtnSt, P1DisagreeBtnSt, P2AgreeBtnSt, P2DisagreeBtnSt, P3AgreeBtnSt, P3DisagreeBtnSt);
  if (btSerial.available()){  //如果藍牙有傳資料過來
    int i = btSerial.read();  //把讀到的資料丟給i
    Serial.println(i);
    if (i == 1) {
      inVoteMode = true;
    } else if (i == 2) {
      inVoteMode = false;
    }

  }
  // 如果有人不應該來在位置上，而且還投票 蜂鳴器就會響
  if (P1Showup == 0 && resistor1 < 600) {
    if (P1AgreeBtnSt==1 || P1DisagreeBtnSt ==1) {
      triggeredAlarm(P1);
      lcd.begin(16, 2);

    }
  }
  if (P2Showup == 0 && resistor2 < 600) {
    if (P2AgreeBtnSt==1 || P2DisagreeBtnSt ==1) {
      triggeredAlarm(P2);
      lcd.begin(16, 2);
    }
  }
  if (P3Showup == 0 && resistor3 < 600) {
    if (P3AgreeBtnSt==1 || P3DisagreeBtnSt ==1) {
      triggeredAlarm(P3);
      lcd.begin(16, 2);
    }
  }
  

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // Move the start index of the substring
    // i_lcd = 0;
    if (i_lcd <= max(FirstLine.length(), ShowupList.length())) {
      lcd.setCursor(0, 0);
      lcd.print(FirstLine.substring(i_lcd + 1, FirstLine.length()));
      lcd.print(" ");
      lcd.setCursor(0, 1);
      lcd.print(ShowupList.substring(i_lcd + 1, ShowupList.length()));
      lcd.print(" ");
      i_lcd++;
    } else {
      i_lcd = 0; // Reset the index to start scrolling from the beginning
    }
  }

  delay(200);
}

String setShowupList(String P1, String P2, String P3, int P1AgreeBtnSt, int P1DisagreeBtnSt, int P2AgreeBtnSt, int P2DisagreeBtnSt, int P3AgreeBtnSt, int P3DisagreeBtnSt) {

  if (P1Showup==0 && P1AgreeBtnSt == 1 && P1DisagreeBtnSt == 1){
    P1Showup=1;
  } else if (P2Showup==0 && P2AgreeBtnSt == 1 && P2DisagreeBtnSt ==1) {
    P2Showup=1;
  } else if (P3Showup==0 && P3AgreeBtnSt == 1 && P3DisagreeBtnSt == 1) {
    P3Showup=1;
  }

  if (P1Showup==1) {
    ShowupList += P1 + " ";
    P1Showup=2;
  } else if (P2Showup==1) {
    ShowupList += P2 + " ";
    P2Showup=2;
  } else if (P3Showup==1) {
    ShowupList += P3 + " ";
    P3Showup=2;
  } 
  delay(200);
  return ShowupList;
}

void triggeredAlarm(String Person) {
  digitalWrite(BuzzerPin,HIGH);
  delay(1000);
  digitalWrite(BuzzerPin,LOW);

  // lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(Alarm + Person);
  lcd.setCursor(0,1);
  lcd.print(Alarm2);

  for (int i=0; i<=5; i++) {
    lcd.display();
    delay(500);
    lcd.noDisplay();
    delay(500);
  }
}