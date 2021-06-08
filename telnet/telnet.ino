/**
 * @author: Niels Temmerman
 */

#include <ESP8266WiFi.h>
#include <Arduino.h>

const char *APssid = "ESPap";
const char *APpassword = "esp123456789";
int port = 80;

String randomCode;

WiFiServer TelnetServer(port);
WiFiClient Telnet;

void handleTelnet(){
  if (TelnetServer.hasClient()){
    if (!Telnet || !Telnet.connected()){
      if(Telnet) Telnet.stop();          
      Telnet = TelnetServer.available(); 
    } else {
      TelnetServer.available().stop();  
    }
  }

  if (Telnet && Telnet.connected() && Telnet.available()){
    while(Telnet.available())
      Serial.write(Telnet.read());
  } 
}

void startAP(){
  WiFi.mode(WIFI_AP);
  WiFi.softAP(APssid, APpassword);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Starten AP");
  startAP();   
  delay(4000); 

  TelnetServer.begin();
  Serial.print("Port: " + (String)port);
  Serial.println();
  delay(100);

  randomCode = generateRandomCode();
}

void loop() {
  handleTelnet();
  Telnet.println(randomCode); //Heel de tijd de code doorsturen.
  delay(100);
}

String generateRandomCode(){ //Elke keer als deze functie wordt opgeroepen wordt er een nieuwe random code gegenereerd.
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
