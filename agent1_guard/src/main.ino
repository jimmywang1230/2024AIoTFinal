#include <Wire.h>
#include <Arduino.h>

char receivedShowupList[3];

void setup() {
  Wire.begin(8);                // 加入I2C總線，設定地址為8
  Wire.onReceive(receiveEvent); // 註冊接收事件
  Wire.onRequest(requestEvent); // 註冊請求事件
  Serial.begin(9600);           // 開始序列通信
}

void loop() {
  delay(100);
}


void receiveEvent(int howMany) {
  for (int i = 0; i < howMany; i++) {
    char x = Wire.read();
    receivedShowupList[i] = x;
  }
  Serial.println("Received showup list: ");
  for (int i = 0; i < howMany; i++) {
    Serial.println(receivedShowupList[i]);
  }
}


// 請求事件處理函數
void requestEvent() {
  Wire.print(receivedShowupList);            // 當主設備請求數據時發送"World"字串
}