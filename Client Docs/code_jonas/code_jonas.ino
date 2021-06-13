#include <IRremote.h>
#include "LedControl.h"
#define Button_0 0xFF6897
#define Button_1 0xFF30CF
#define Button_2 0xFF18E7
#define Button_3 0xFF7A85
#define Button_4 0xFF10EF
#define Button_5 0xFF38C7
#define Button_6 0xFF5AA5
#define Button_7 0xFF42BD
#define Button_8 0xFF4AB5
#define Button_9 0xFF52AD

const int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;

int cijfers[4];
uint32_t Previous;
LedControl lc = LedControl(11, 13, 10, 1);
const unsigned long time = 1000;
unsigned long previousTime=0;
int i = 5909;

void setup() {
  // De MAX72XX chip staat in power-safe mode bij het opstarten
  // deze maken we wakker met shutdown van apparaat 0
  lc.shutdown(0, false);
  // Stel de helderheid van het scherm op apparaat 0 in met een waarde van 0 tot 15
  lc.setIntensity(0, 5);
  // Maak scherm schoon van apparaat 0
  lc.clearDisplay(0);
  Serial.begin(9600);
  irrecv.enableIRIn();
}
void blinking(){
  lc.setDigit(0, 0, 0, false);
   lc.setDigit(0, 1, 0, false);
    lc.setDigit(0, 2, 0, false); 
    lc.setDigit(0, 3, 0, false);
    i=0;
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
      i=i-40;
    }
    // Toon de getallen op het display
    lc.setDigit(0, 0, thousands, false);
    lc.setDigit(0, 1, hundreds, false);
    lc.setDigit(0, 2, tens, false); 
    lc.setDigit(0, 3, ones, false); 
} 
int j = 0;
void loop() {
  if (irrecv.decode(&results)){
     if (results.value==0xFFFFFFFF) { 
     results.value=Previous;
     }
     
     switch(results.value) {
           case Button_1 : cijfers[j]={1}; break;           //geef nummer 1 weer
           case Button_2 : cijfers[j]={2}; break;           //geef nummer 2 weer
           case Button_3 : cijfers[j]={3}; break;           //geef nummer 3 weer
           case Button_4 : cijfers[j]={4}; break;           //geef nummer 4 weer
           case Button_5 : cijfers[j]={5}; break;           //geef nummer 5 weer
           case Button_6 : cijfers[j]={6}; break;           //geef nummer 6 weer
           case Button_7 : cijfers[j]={7}; break;           //geef nummer 7 weer
           case Button_8 : cijfers[j]={8}; break;           //geef nummer 8 weer
           case Button_9 : cijfers[j]={9}; break;           //geef nummer 9 weer
           case Button_0 : cijfers[j]={0}; break;           //geef nummer 0 weer
      }
    
    irrecv.resume();
     lc.setDigit(0,j+4,cijfers[j],false);
    }
    j++;
  unsigned long currentTime=millis(); 
  if(currentTime-previousTime >= time){
    lc.clearDisplay(0);
    printNumber(i);
    i--;
    previousTime=currentTime;
  }
 

  
  while(i==0){
    lc.clearDisplay(0);
    delay(500);
    blinking();
    delay(500);
  }
}
