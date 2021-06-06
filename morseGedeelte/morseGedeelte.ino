#include <IRremote.h>
//Website als reference gebruikt voor de morse code regels/standaard
//http://students.cs.ucl.ac.uk/schoolslab/projects/PY2/introduction.html
//Quintin Coryn-ICT6


//Nog bekijken voor transmitter
int IR_TX = 1; //VERANDEREN VOOR LATER, alleen voor testen van code

//Nummers 0-9
char code[4] = {'5', '2', '1', '0'};
char* numbers[] = {
  "-----", ".----", "..---", "...--", "....-", ".....",
  "-....", "--...", "---..", "----."
};
const int morseDelay = 250;
int dot = 500;
int dash = dot * 3;
int pause = dot;
int Spaces = dot * 3;

void setup() {
  pinMode(12, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  char currentNumber;
  for (int i = 0; i < sizeof(code) - 1; i++) {
    currentNumber = code[i];
    Serial.println(currentNumber);
    delay(500);
    charOutNumbers(currentNumber);
  }
  LightsOff(8000);

}
void playDot()
{
  digitalWrite(12, HIGH);
  delay(dot);
}
void playDash()
{
  digitalWrite(12, HIGH);
  delay(dash);
}

// Turn Off
void LightsOff(int delayTime)
{
  digitalWrite(12, LOW);
  delay(delayTime);
}

void charOutNumbers(char currentChar) {
  switch (currentChar) {
    case '0':
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      break;

    case '1':
      playDot();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);

      break;

    case '2':
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      break;

    case '3':
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      break;

    case '4':
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      break;

    case '5':
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      break;

    case '6':
      playDash();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      break;

    case '7':
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      break;

    case '8':
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      break;

    case '9':
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDash();
      LightsOff(pause);
      playDot();
      LightsOff(pause);
      break;
    default:
      LightsOff(Spaces);
  }

  delay(morseDelay);
}
