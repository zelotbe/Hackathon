#include <LedControl.h>
#include "IRremote.h"           //Library used: https://github.com/Arduino-IRremote/Arduino-IRremote
#include <SoftwareSerial.h>


#define Button_0 0xFF6897       //Gekeken welk signaal de remote geeft als u op een bepaald knopje drukt,
#define Button_1 0xFF30CF       //en zo de drukknoppen gedefinieerd die in dit project gebruikt worden
#define Button_2 0xFF18E7
#define Button_3 0xFF7A85
#define Button_4 0xFF10EF
#define Button_5 0xFF38C7
#define Button_6 0xFF5AA5
#define Button_7 0xFF42BD
#define Button_8 0xFF4AB5
#define Button_9 0xFF52AD

const String ssid = "ESPap"; //SSID Acces Point
const String password = "esp123456789"; //Wachtwoord Acces Point
char code[4] = { ' ', ' ', ' ', ' ' }; //Char array voor de random code
bool beginMetLezen = false; //Check flag 1
bool codeNietGevonden = true; //Check flag 2

const int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;

int cijfers[4];
uint32_t previous;
const unsigned long time = 1000;
unsigned long previousTime = 0;

int i = 5909;
int j = 0;


// Initialiseer de LedControl bibliotheek met
// pin 7 op DIN, 
// pin 6 op Clk, 
// pin 5 op LOAD
LedControl lc = LedControl(11, 13, 10, 1);
SoftwareSerial ESP01(6, 7); // RX, TX naar ESP-01


void blinking() {
  lc.setDigit(0, 0, 0, false);
  lc.setDigit(0, 1, 0, false);
  lc.setDigit(0, 2, 0, false);
  lc.setDigit(0, 3, 0, false);
  i = 0;
}

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
  if (tens == 9) {
    tens = tens - 4;
    i = i - 40;
  }
  // Toon de getallen op het display
  lc.setDigit(0, 0, thousands, false);
  lc.setDigit(0, 1, hundreds, false);
  lc.setDigit(0, 2, tens, false);
  lc.setDigit(0, 3, ones, false);
}

void printResponse() { //Om te kijken of de reactie juist is van de ESP-01
  while (ESP01.available()) {
    Serial.println(ESP01.readStringUntil('\n'));
  }
}

//Bron voor de AT-Commands: https://www.instructables.com/Getting-Started-With-the-ESP8266-ESP-01/
void connectToNetwork() { //Configuratie om te verbinden met de wifikit door het sturen van AT-Commands
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

boolean codeIsCorrect() {
  for (byte i = 0; i < 4; i++) {
    Serial.print(code[i], DEC);
    Serial.print(" : ");
    Serial.println(cijfers[i], DEC);
  }

  for (byte i = 0; i < 4; i++) {
    if (code[i] - '0' != cijfers[i]) return false;
  }
  return true;
}

int getNumber(long unsigned int r) {
  switch (r) {
  case Button_1: return 1;           //geef nummer 1 weer
  case Button_2: return 2;           //geef nummer 2 weer
  case Button_3: return 3;           //geef nummer 3 weer
  case Button_4: return 4;           //geef nummer 4 weer
  case Button_5: return 5;           //geef nummer 5 weer
  case Button_6: return 6;           //geef nummer 6 weer
  case Button_7: return 7;           //geef nummer 7 weer
  case Button_8: return 8;           //geef nummer 8 weer
  case Button_9: return 9;           //geef nummer 9 weer
  case Button_0: return 0;           //geef nummer 0 weer
  default: return -1;
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

  irrecv.enableIRIn();
}

void loop() {

  if (irrecv.decode(&results)) {

    int result = getNumber(results.value);
    if (result != -1) {
      cijfers[j] = result;
      lc.setDigit(0, j + 4, cijfers[j], false);
      j++;
      if (j > 3) {
        if (codeIsCorrect()) {
          lc.clearDisplay(0);
          lc.setChar(0, 0, 'H', false);
          lc.setChar(0, 1, 'E', false);
          lc.setChar(0, 2, 'L', false);
          lc.setChar(0, 3, 'L', false);
          lc.setDigit(0, 4, 0, false);
          lc.setChar(0, 5, '.', false);
          lc.setChar(0, 6, '.', false);
          lc.setChar(0, 7, '.', false);
          exit(0);
        }
        lc.clearDisplay(0);
        j = 0;
      }
    }
    irrecv.resume();
  }

  unsigned long currentTime = millis();
  if (currentTime - previousTime >= time) {
    //lc.clearDisplay(0);
    printNumber(i);
    i--;
    previousTime = currentTime;
  }

  while (i == 0) {
    lc.clearDisplay(0);
    delay(500);
    blinking();
    delay(500);
  }

  if (ESP01.available()) {
    String str = ESP01.readStringUntil('\n'); //Uitlezen van chars op de softserial van de ESP-01


    if (str.indexOf(':') > 0) {
      int index = str.indexOf(':'); //Zoek index van ':' voor uitlezen code

      String result = str.substring(index + 1, str.length() - 1); //Substring van de code

      if (result.length() == 4) {
        for (byte i = 0; i < 4; i++) {
          code[i] = result.charAt(i); //Add to char array
        }
      }
    }

  }

}
