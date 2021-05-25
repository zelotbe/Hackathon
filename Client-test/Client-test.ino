#include <SoftwareSerial.h>
SoftwareSerial ESP01(2, 3); // RX, TX


const String ssid = "ESPap";
const String password = "esp123456789";
String randomCode;   

void setup() {
  Serial.begin(9600);
  ESP01.begin(9600);
  delay(3000);
  connectToNetwork();
}

void loop() {
  if (ESP01.available()) {
    Serial.write(ESP01.read());
  }
  if (Serial.available()){
    ESP01.write(Serial.read());
  }
}

void connectToNetwork(){
  ESP01.println("AT+CIPMUX=0");
  delay(1000);
  printResponse();
  
  ESP01.println("AT+CWMODE=1");
  Serial.println("AT+CWMODE=1");
  if(ESP01.find("OK")){
    Serial.println("Response: OK");
  }
  delay(100);
  ESP01.println("AT+CWJAP=\"ESPap\",\"esp123456789\"");
  Serial.println("AT+CWJAP= "+ ssid + "," + password);
  delay(100);
  if(ESP01.find("OK")){
    Serial.println("Response: OK");
  }
  delay(5000);
  
  ESP01.println("AT+CIPSTART=\"TCP\",\"192.168.4.1\",80");
  delay(1000);
  printResponse();
}

void printResponse() {
  while (ESP01.available()) {
    Serial.println(ESP01.readStringUntil('\n')); 
  }
}

String generateRandomCode(){
  int randomCode[4];
  String stringCode;
  
  for(int i = 0; i < 4; i++){
    randomCode[i] = random(0,10);
  }

  for(int i = 0; i < 4; i++){
    stringCode += String(randomCode[i]);
  }
  return stringCode;
}
