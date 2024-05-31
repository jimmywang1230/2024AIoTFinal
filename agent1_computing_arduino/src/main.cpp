#include <Arduino.h>

char memberList[4] = "000";
byte count = 0;

void turnOffLED() {
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    for(int i = 0; i < 4; i++) {
        memberList[i] = '0';
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(12, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(10, OUTPUT);
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
    digitalWrite(10, LOW);
}

void loop() {
  
    count = 0;
    turnOffLED();

    while (Serial.available() > 0) {
        int inByte = Serial.read();
        Serial.println(inByte);
        if (inByte == '1' || inByte == '0') {
          memberList[count] = (char)inByte;
          count++;
          Serial.print("char: ");
          Serial.println((char)inByte);
          Serial.print("memberList: ");
          Serial.println(memberList);
        }
    }
    

    // if (strcmp(memberList[0], '1') == 0)
    //     digitalWrite(12, HIGH);
    // if (strcmp(memberList, "110") == 0)
    //     digitalWrite(12, HIGH);
    if(memberList[0] == '1')
      digitalWrite(10, HIGH);
    if(memberList[1] == '1')
      digitalWrite(11, HIGH);
    if(memberList[2] == '1')
      digitalWrite(12, HIGH);

    delay(1000);
}