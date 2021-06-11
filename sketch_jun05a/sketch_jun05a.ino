#include "LedControl.h"
#include <SoftwareSerial.h>

const String ssid = "ESPap";
const String password = "esp123456789";
char code[4] = { ' ', ' ', ' ', ' ' };
bool beginMetLezen = false;
bool codeNietGevonden = true;
int i = 0;
int IR_TX = 1;

int timer = 5909;

// Initialiseer de LedControl bibliotheek met
// pin 7 op DIN, 
// pin 6 op Clk, 
// pin 5 op LOAD
LedControl lc = LedControl(11, 13, 10, 1);
SoftwareSerial ESP01(2, 3); // RX, TX naar ESP-01


void connectToNetwork() { //Configuratie om te verbinden met de wifikit.
  ESP01.println("AT+CIPMUX=0");
  delay(1000);
  printResponse();

  ESP01.println("AT+CWMODE=1");
  Serial.println("AT+CWMODE=1");
  if (ESP01.find("OK")) {
    Serial.println("Response: OK");
  }
  delay(100);
  ESP01.println("AT+CWJAP=\"ESPap\",\"esp123456789\"");
  Serial.println("AT+CWJAP= " + ssid + "," + password);
  delay(100);
  if (ESP01.find("OK")) {
    Serial.println("Response: OK");
  }
  delay(5000);

  ESP01.println("AT+CIPSTART=\"TCP\",\"192.168.4.1\",80");
  delay(1000);
  printResponse();
}

void printResponse() { //Om te kijken of de reactie juist is van de ESP-01
  while (ESP01.available()) {
    Serial.println(ESP01.readStringUntil('\n'));
  }
}

void setup() {

  Serial.begin(9600);
  ESP01.begin(9600);
  delay(3000);
  connectToNetwork();

  // De MAX72XX chip staat in power-safe mode bij het opstarten
  // deze maken we wakker met shutdown van apparaat 0
  lc.shutdown(0, false);

  // Stel de helderheid van het scherm op apparaat 0 in met een waarde van 0 tot 15
  lc.setIntensity(0, 5);
  
  // Maak scherm schoon van apparaat 0
  lc.clearDisplay(0);
}

// Toont een nummer van maximaal 4 getallen, dus 0...9999
void printNumber(int number) {  
    int ones;      // eenheden
    int tens;      // tientallen
    int hundreds;  // honderdtallen
    int thousands; // duizendtallen

    // Bepaal uit hoeveel enen het getal bestaat
    // door de rest te nemen als het getal door 10 gedeeld wordt
    ones = number % 10;
    // Sla het aantal volledig passende delingen op
    number = number / 10;

    // Bepaal uit hoeveel tientallen het getal bestaat
    // door de rest te nemen als het getal door 10 gedeeld wordt
    tens = number % 10;  
    number = number / 10;

    // Bepaal uit hoeveel honderdtallen het getal bestaat
    // door de rest te nemen als het getal door 10 gedeeld wordt
    hundreds = number % 10;
    number = number / 10;
    
    thousands = number;
    if(tens==9){
      tens=tens-4;
      timer=timer-40;
    }
    // Toon de getallen op het display
    lc.setDigit(0, 0, thousands, false);
    lc.setDigit(0, 1, hundreds, false);
    lc.setDigit(0, 2, tens, false); 
    lc.setDigit(0, 3, ones, false); 
}

void loop() {

  if (ESP01.available() && codeNietGevonden) {
    char temp = ESP01.read(); //Uitlezen van chars op de softserial van de ESP-01

    if (code[3] != ' ') {
      beginMetLezen = false;
    }

    if (beginMetLezen) {
      code[i] = temp;
      i++;
    }

    if (temp == ':') {
      beginMetLezen = true;
    }
  }
  if (Serial.available()) {
    ESP01.write(Serial.read());
  }

  Serial.print(code[0]);
  Serial.print(code[1]);
  Serial.print(code[2]);
  Serial.println(code[3]);

  if (code[3] != ' ') { //Blijft doorgaan tot de hele code is gevonden.
    codeNietGevonden == false;
    Serial.println("CODE GEVONDEN");
  }

  delay(500);

  lc.clearDisplay(0);
  printNumber(timer);
  timer--;
  delay(1000);
}
