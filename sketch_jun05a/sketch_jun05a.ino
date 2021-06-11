#include "LedControl.h"

// Initialiseer de LedControl bibliotheek met
// pin 7 op DIN, 
// pin 6 op Clk, 
// pin 5 op LOAD
// aantal displays 1
LedControl lc = LedControl(11, 13, 10, 1);

int i = 5909;

void setup() {
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
      i=i-40;
    }
    // Toon de getallen op het display
    lc.setDigit(0, 0, thousands, false);
    lc.setDigit(0, 1, hundreds, false);
    lc.setDigit(0, 2, tens, false); 
    lc.setDigit(0, 3, ones, false); 
} 





void loop() { 
  lc.clearDisplay(0);
  printNumber(i);
  i--;
  delay(1000);
}
