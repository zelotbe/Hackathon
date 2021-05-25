/**
 * Arduino ESP8266 telnet server with some ansi experiments
 * @author: shawn_a
 */

#include <ESP8266WiFi.h>
#include <Arduino.h>

/* Set these to your desired AP credentials. */
const char *APssid = "ESPap";
const char *APpassword = "esp123456789";
int port = 80;

String randomCode;

// declare telnet server (do NOT put in setup())
WiFiServer TelnetServer(port);
WiFiClient Telnet;

void handleTelnet(){
  if (TelnetServer.hasClient()){
    // client is connected
    if (!Telnet || !Telnet.connected()){
      if(Telnet) Telnet.stop();          // client disconnected
      Telnet = TelnetServer.available(); // ready for new client
    } else {
      TelnetServer.available().stop();  // have client, block new conections
    }
  }

  if (Telnet && Telnet.connected() && Telnet.available()){
    // client input processing
    while(Telnet.available())
      Serial.write(Telnet.read()); // pass through
      // do other stuff with client input here
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
  // Serial.setDebugOutput(true);
  delay(1000); // serial delay

  Serial.println();
  Serial.println("Staring AP");
  startAP();   // startup is async ?
  delay(4000); // ap delay

  TelnetServer.begin();
  Serial.print("Starting telnet server on port " + (String)port);
  // TelnetServer.setNoDelay(true); // ESP BUG ?
  Serial.println();
  delay(100);

  randomCode = generateRandomCode();
}

void loop() {
  handleTelnet();
  Telnet.println(1234);
  delay(100);
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
