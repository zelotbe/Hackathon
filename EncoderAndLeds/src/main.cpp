#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>

#define ADDRESS 0x20
#define DEVIATION 30
#define MAXVALUE 1023

const char* APssid = "ESPap";
const char* APpassword = "esp123456789";
int port = 80;

WiFiServer TelnetServer(port);
WiFiClient Telnet;

String randomCode;

byte prevEncoderValue = B00000000;

const byte settings[6][2] = {
  //{address, setting}
  {0x00, 0xFF}, // Set portA as input for encoders
  {0x01, 0xFC}, // Set portB as input for buttons, output for LEDs
  {0x04, 0x3F}, // Set interrupts for encoders
  {0x05, 0xE0}, // Set interrupts for buttons
  {0x0C, 0xFF}, // Enable pull-up on portA
  {0x0D, 0xFC} // Enable pull-up on upper portB
};

const byte targetLedPins[] = {
  0, // Target R
  13, // Target G
  12, // Target B
};

const byte currentLedPins[] = {
  14, // Current R
  5, // Current G
  3 // Current B
};

int targetLedValues[] = {0, 0, 0};
int currentLedValues[] = {0, 0, 0};

bool codeCorrect = false;


void initExpander() {
  // Encoder: portA (0-5)
  // Buttons: portB (7-5)
  // Leds: portB (0-1)
  //Wire.pins(4, 2); //sda : 4, scl:2
  Wire.begin(4, 2); // Join bus as master 

  Wire.beginTransmission(ADDRESS);
  Wire.write(0x0A); Wire.write(0x22); // Disable auto-increment address
  Wire.endTransmission();

  delay(100);

  for(byte i = 0; i < sizeof(settings)/sizeof(settings[0]); i++) {
    Wire.beginTransmission(ADDRESS);
    Wire.write(settings[i][0]); Wire.write(settings[i][1]);
    Wire.endTransmission();
    delay(100);
  }

}

void writeGreenLed(bool state) {
  Wire.beginTransmission(ADDRESS);
  Wire.write(0x15); Wire.write(state << 1);
  Wire.endTransmission();
}

void writeRedLed(bool state) {
  Wire.beginTransmission(ADDRESS);
  Wire.write(0x15); Wire.write(state);
  Wire.endTransmission();
}

int fetchFromAddress(byte addr) {
  byte result = 0;
  Wire.beginTransmission(ADDRESS);
  Wire.write(addr);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS, 1);
  while (Wire.available()) {
    result = Wire.read();
  }
  return result;
}

boolean checkForCodeMatch() {
  for(byte i = 0; i < 3; i++) {
    // 200 - 30 > 200 
    // 200 + 30 < 200
    if(targetLedValues[i] - DEVIATION > currentLedValues[i]) return false;
    if(targetLedValues[i] + DEVIATION < currentLedValues[i]) return false;
  }
  return true;
}

void updateCurrentLed() {
  for(byte i = 0; i < 3; i++) analogWrite(currentLedPins[i], currentLedValues[i]*4);
}

void handleTelnet() {
  if (TelnetServer.hasClient()) {
    if (!Telnet || !Telnet.connected()) {
      if (Telnet) Telnet.stop();
      Telnet = TelnetServer.available();
    }
    else {
      TelnetServer.available().stop();
    }
  }

  if (Telnet && Telnet.connected() && Telnet.available()) {
    while (Telnet.available())
      Serial.write(Telnet.read());
  }
}

void startAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(APssid, APpassword);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

/*
Encoder for Red:
Clockwise           xxxx xx00 -> xxxx xx01 -> xxxx xx11
                    xxxx xx11 -> xxxx xx10 -> xxxx xx00
Counter-clockwise   xxxx xx00 -> xxxx xx10 -> xxxx xx11
                    xxxx xx11 -> xxxx xx01 -> xxxx xx00

Encoder for Green:
Clockwise           xxxx 00xx -> xxxx 01xx -> xxxx 11xx
Counter-clockwise   xxxx 00xx -> xxxx 10xx -> xxxx 11xx

Encoder for blue:
Clockwise           xx00 xxxx -> xx01 xxxx -> xx11 xxxx
Counter-clockwise   xx00 xxxx -> xx01 xxxx -> xx11 xxxx
*/
void handleEncoder() {
  byte result = fetchFromAddress(0x10);

  byte currentResultRed = result & 0x03;
  byte currentResultGreen = (result & 0x0C) >> 2;
  byte currentResultBlue = (result & 0x30) >> 4;

  byte prevResultRed = prevEncoderValue & 0x03;
  byte prevResultGreen = (prevEncoderValue & 0x0C) >> 2;
  byte prevResultBlue = (prevEncoderValue & 0x30) >> 4;

  // Detect change in red

  // Clockwise
  if(((prevResultRed == 0x00 && currentResultRed == 0x01) || (prevResultRed == 0x01 && currentResultRed == 0x03)) || ((prevResultRed == 0x03 && currentResultRed == 0x02) || (prevResultRed == 0x02 && currentResultRed == 0x00))) {
    currentLedValues[0] += 5;
    if (currentLedValues[0] > 255) currentLedValues[0] = 255;
  }

  //Counter-Clockwise
  if(((prevResultRed == 0x00 && currentResultRed == 0x02) || (prevResultRed == 0x02 && currentResultRed == 0x03)) || ((prevResultRed == 0x03 && currentResultRed == 0x01) || (prevResultRed == 0x01 && currentResultRed == 0x00))) {
    currentLedValues[0] -= 5;
    if (currentLedValues[0] < 0) currentLedValues[0] = 0;
  }


  // Detect change in green
  // Clockwise
  if(((prevResultGreen == 0x00 && currentResultGreen == 0x01) || (prevResultGreen == 0x01 && currentResultGreen == 0x03)) || ((prevResultGreen == 0x03 && currentResultGreen == 0x02) || (prevResultGreen == 0x02 && currentResultGreen == 0x00))) {
    currentLedValues[1] += 5;
    if (currentLedValues[1] > 255) currentLedValues[1] = 255;
  }

  //Counter-Clockwise
  if(((prevResultGreen == 0x00 && currentResultGreen == 0x02) || (prevResultGreen == 0x02 && currentResultGreen == 0x03)) || ((prevResultGreen == 0x03 && currentResultGreen == 0x01) || (prevResultGreen == 0x01 && currentResultGreen == 0x00))) {
    currentLedValues[1] -= 5;
    if (currentLedValues[1] < 0) currentLedValues[1] = 0;
  }


  // Detect change in blue
  // Clockwise
  if (((prevResultBlue == 0x00 && currentResultBlue == 0x01) || (prevResultBlue == 0x01 && currentResultBlue == 0x03)) || ((prevResultBlue == 0x03 && currentResultBlue == 0x02) || (prevResultBlue == 0x02 && currentResultBlue == 0x00))) {
    currentLedValues[2] += 5;
    if (currentLedValues[2] > 255) currentLedValues[2] = 255;
  }

  //Counter-Clockwise
  if(((prevResultBlue == 0x00 && currentResultBlue == 0x02) || (prevResultBlue == 0x02 && currentResultBlue == 0x03)) || ((prevResultBlue == 0x03 && currentResultBlue == 0x01) || (prevResultBlue == 0x01 && currentResultBlue == 0x00))) {
    currentLedValues[2] -= 5;
    if (currentLedValues[2] < 0) currentLedValues[2] = 0;
  }

  updateCurrentLed();
  codeCorrect = checkForCodeMatch();
  if(codeCorrect) {
    writeRedLed(LOW);
    writeGreenLed(HIGH);
  }
  prevEncoderValue = result;

}

void newPWMValues() {
  for(byte i = 0; i < 3; i++) {
    targetLedValues[i] = 255;//random(0, 255);
    analogWrite(targetLedPins[i], targetLedValues[i]*4);
  }
}

String generateRandomCode() { //Elke keer als deze functie wordt opgeroepen wordt er een nieuwe random code gegenereerd.
  int randomCode[4];
  String stringCode;

  for (int i = 0; i < 4; i++) {
    randomCode[i] = random(0, 10);
  }

  for (int i = 0; i < 4; i++) {
    stringCode += String(randomCode[i]);
  }
  return stringCode;
}

void reset() {
  // Reset the code and the colorvalues
  newPWMValues();
}

void previousNumber() {
  // Reset the colorvalues and get previous digit of code
}

void nextNumber() {
  // Reset the colorvalues and get next digit of code
  newPWMValues();
}

/*
R-button (reset):
1xxx xxxx -> not pressed
0xxx xxxx -> pressed

G-button (prev):
x1xx xxxx -> not pressed
x0xx xxxx -> pressed

B-button (next):
xx1x xxxx -> not pressed
xx0x xxxx -> pressed
*/
void handleButton() {
  byte result = ~fetchFromAddress(0x11); // Watch the flip

  // Handle reset (left)
  if(result & 0x80) reset();

  // Handle previous (middle)
  if(result & 0x40) previousNumber();

  // Handle next (right)
  if(result & 0x20) nextNumber();

}

void setup() {
  // put your setup code here, to run once:

  initExpander();

  for(byte pin: targetLedPins) pinMode(pin, OUTPUT);
  for (byte pin : currentLedPins) pinMode(pin, OUTPUT);

  startAP();
  delay(1000);

  TelnetServer.begin();

  randomCode = generateRandomCode();

  newPWMValues();

  writeRedLed(HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  handleTelnet();
  Telnet.println(randomCode);
  if(digitalRead(16)) {
    handleEncoder();
  }
  if(analogRead(A0) > 1000 ? true: false) { // HACKY
    handleButton();
  }
  while(codeCorrect) {
    // Send morse

    // Handle next, previous, reset
    if(analogRead(A0) > 1000 ? true: false) { // HACKY
      handleButton();
    }
  }
}