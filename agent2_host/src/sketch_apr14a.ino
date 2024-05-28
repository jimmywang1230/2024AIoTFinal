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

class Participant {
public:
  String name;
  int AgreeBtnSt;
  int DisagreeBtnSt;
  int Showup;
  int resistor;

  Participant() {
    name = "";
    AgreeBtnSt = 0;
    DisagreeBtnSt = 0;
    Showup = 0;
    resistor = 0;
  }
};

Participant P1, P2, P3;

boolean inVoteMode; // 投票模式


String FirstLine = "WWelcome to NTUST LiFaUan";
String ShowupList = "SShowup List: ";
String Alarm = "ALERT: ";
String Alarm2 = "No show but vote";


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

  P1.name = "English Tsai";
  P2.name = "KP";
  P3.name = "Korea Fish";
  P1.Showup=0, P2.Showup=0, P3.Showup=0;

  
  Serial.begin(9600);
  btSerial.begin(9600);
  
  inVoteMode = false;

  lcd.begin(16, 2); // 初始化LCD
  lcd.setBacklight(255);
  lcd.clear();
}
int prev=0;
void loop()
{
  unsigned long currentMillis = millis();
  P1.resistor=analogRead(A0); 
  P2.resistor=analogRead(A1); 
  P3.resistor=analogRead(A2);
  P1.AgreeBtnSt= digitalRead(P1AgreePin);
  P1.DisagreeBtnSt= digitalRead(P1DisagreePin);
  P2.AgreeBtnSt= digitalRead(P2AgreePin);
  P2.DisagreeBtnSt= digitalRead(P2DisagreePin);
  P3.AgreeBtnSt= digitalRead(P3AgreePin);
  P3.DisagreeBtnSt= digitalRead(P3DisagreePin);

  ShowupList = setShowupList(P1, P2, P3);
  if (btSerial.available()){  //如果藍牙有傳資料過來
    int i = btSerial.read();  //把讀到的資料丟給i
    Serial.println(i);
    if (i == 1) {
      inVoteMode = true;
      voteProcess(P1, P2, P3);

    } else if (i == 2) {
      inVoteMode = false;
    }

  }
  // 如果有人不應該來在位置上，而且還投票 蜂鳴器就會響
  if (P1.Showup == 0 && P1.resistor < 600) {
    if (P1.AgreeBtnSt==1 || P1.DisagreeBtnSt ==1) {
      triggeredAlarm(P1.name);
    }
  }
  if (P2.Showup == 0 && P2.resistor < 600) {
    if (P2.AgreeBtnSt==1 || P2.DisagreeBtnSt ==1) {
      triggeredAlarm(P2.name);
    }
  }
  if (P3.Showup == 0 && P3.resistor < 600) {
    if (P3.AgreeBtnSt==1 || P3.DisagreeBtnSt ==1) {
      triggeredAlarm(P3.name);
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

String voteProcess(Participant P1, Participant P2, Participant P3) {
  String voteResult = "";
  String agreeVote = "";
  String disagreeVote = "";
  int agreeCount = 0;
  int disagreeCount = 0;
  if (P1.Showup == 1 || P2.Showup == 1 || P3.Showup == 1) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Bill 1 Vote");
    delay(1000);

    // Start the countdown
    unsigned long startTime = millis();
    int countdown = 10; // 10 seconds
    while (countdown >= 0) {
      // Check if a second has passed
      if (millis() - startTime >= 1000) {
        if (P1.AgreeBtnSt == 1 && P1.DisagreeBtnSt == 0) {
          agreeVote += P1.name + " ";
          lcd.setCursor(0, 1);

        } else if (P1.AgreeBtnSt == 0 && P1.DisagreeBtnSt == 1) {
          disagreeVote += P1.name + " ";
          lcd.setCursor(0, 1);
        } else if (P2.AgreeBtnSt ==1 && P2.DisagreeBtnSt ==0) {
          agreeVote += P2.name + " ";
          lcd.print(agreeVote);
        } else if (P2.AgreeBtnSt ==0 && P2.DisagreeBtnSt ==1) {
          disagreeVote += P2.name + " ";
          lcd.print(disagreeVote);
        } else if (P3.AgreeBtnSt ==1 && P3.DisagreeBtnSt ==0) {
          agreeVote += P3.name + " ";
          lcd.print(agreeVote);
        } else if (P3.AgreeBtnSt ==0 && P3.DisagreeBtnSt ==1) {
          disagreeVote += P3.name + " ";
          lcd.print(disagreeVote);
        }
        lcd.setCursor(0, 2);
        lcd.print("Agree Vote: ");
        lcd.print(agreeVote);
        lcd.setCursor(0, 1);
        lcd.print("Time left: ");
        lcd.print(countdown);
        countdown--;
        startTime = millis();
      }
    }
    voteResult = agreeVote + " " + disagreeVote;
    return voteResult;
  } else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Not all show up");
    lcd.setCursor(0,1);
    lcd.print("Can't vote");
    return "no show up";
  }

}


// 顯示出席名單
String setShowupList(Participant P1, Participant P2, Participant P3) {

  if (P1.Showup==0 && P1.AgreeBtnSt == 1 && P1.DisagreeBtnSt == 1){
    P1.Showup=1;
  } else if (P2.Showup==0 && P2.AgreeBtnSt == 1 && P2.DisagreeBtnSt ==1) {
    P2.Showup=1;
  } else if (P3.Showup==0 && P3.AgreeBtnSt == 1 && P3.DisagreeBtnSt == 1) {
    P3.Showup=1;
  }

  if (P1.Showup==1) {
    ShowupList += P1.name + " ";
    P1.Showup=2;
  } else if (P2.Showup==1) {
    ShowupList += P2.name + " ";
    P2.Showup=2;
  } else if (P3.Showup==1) {
    ShowupList += P3.name + " ";
    P3.Showup=2;
  } 
  delay(200);
  return ShowupList;
}

// 有人不在位卻投票時，觸發警報
void triggeredAlarm(String Person) {
  lcd.clear();
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
  lcd.begin(16, 2);
}