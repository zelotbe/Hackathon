#include <IRremote.h>
#include <SevSeg.h>
SevSeg sevseg;         
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
int reciever = 13;
uint32_t Previous; 
IRrecv irrecv(reciever);
decode_results results;



void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  byte numDigits = 1;
  byte digitPins[] = {};
  byte segmentPins[] = {6, 5, 2, 10, 4, 7, 8, 9};
  bool resistorsOnSegments = true;

  byte hardwareConfig = COMMON_ANODE; 
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);
}


void loop() {
  if (irrecv.decode(&results)){
     if (results.value==0xFFFFFFFF) { 
     results.value=Previous;
     }
     switch(results.value) {
           case Button_1 : sevseg.setNumber(1); sevseg.refreshDisplay(); break;           //geef nummer 1 weer
           case Button_2 : sevseg.setNumber(2); sevseg.refreshDisplay(); break;           //geef nummer 2 weer
           case Button_3 : sevseg.setNumber(3); sevseg.refreshDisplay(); break;           //geef nummer 3 weer
           case Button_4 : sevseg.setNumber(4); sevseg.refreshDisplay(); break;           //geef nummer 4 weer
           case Button_5 : sevseg.setNumber(5); sevseg.refreshDisplay(); break;           //geef nummer 5 weer
           case Button_6 : sevseg.setNumber(6); sevseg.refreshDisplay(); break;           //geef nummer 6 weer
           case Button_7 : sevseg.setNumber(7); sevseg.refreshDisplay(); break;           //geef nummer 7 weer
           case Button_8 : sevseg.setNumber(8); sevseg.refreshDisplay(); break;           //geef nummer 8 weer
           case Button_9 : sevseg.setNumber(9); sevseg.refreshDisplay(); break;           //geef nummer 9 weer
           case Button_0 : sevseg.setNumber(0); sevseg.refreshDisplay(); break;           //geef nummer 0 weer
      }
    Serial.println (results.value, HEX);
    irrecv.resume();

     
    }

}
