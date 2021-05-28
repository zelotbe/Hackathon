#include <IRremote.h>
//Website als reference gebruikt voor de morse code regels/standaard
//http://students.cs.ucl.ac.uk/schoolslab/projects/PY2/introduction.html
//Quintin Coryn-ICT6


//Nog bekijken voor transmitter
int IR_TX = 1; //VERANDEREN VOOR LATER, alleen voor testen van code

//Numbers 0-9
const char* numbers[] = {
  "-----", ".----", "..---", "...--", "....-", ".....",
"-....", "--...", "---..", "----."
};
const int morseDelay = 250;


void setup() {
  Serial.begin(9600);
}

void loop() {
  char currentChar;

 //afwachten tot wat de input is van de vorige puzzel

}

void charOutNumbers(char* numbers){
  int count;

  while(numbers[count] != '\0'){
    slashDotDetection(numbers[count]);
    count++;
  }
}

void slashDotDetection(char currentChar){
  
  //Aanzetten van de transmitter
  digitalWrite(IR_TX, HIGH);

  //Duratie is 3 keer een dot
  if(currentChar == '-'){
    delay(1500);
  }

  if(currentChar == '.'){
    delay(500);
  }

  digitalWrite(IR_TX, LOW);
  delay(morseDelay);
}
