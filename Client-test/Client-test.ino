#include <SoftwareSerial.h>
SoftwareSerial ESP01(2, 3); // RX, TX


const String ssid = "ESPap";
const String password = "esp123456789";
char code[4] = {' ', ' ', ' ', ' '};
bool beginMetLezen = false;
bool codeNietGevonden = true;
int i = 0;
int IR_TX = 1; 

const char* numbers[] = {
  "-----", ".----", "..---", "...--", "....-", ".....",
"-....", "--...", "---..", "----."
};
const int morseDelay = 250;

void setup() {
  Serial.begin(9600);
  ESP01.begin(9600);
  delay(3000);
  connectToNetwork();
}

void loop() {
  if (ESP01.available() && codeNietGevonden) {
    char temp = ESP01.read();

    if(code[3] != ' '){
      beginMetLezen = false;
    }
    
    if(beginMetLezen){
      code[i] = temp;
      i++;
    }
    
    if(temp == ':'){
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
  
  if(code[3] != ' '){
    codeNietGevonden == false;
    Serial.println("CODE GEVONDEN");
  }
  
  delay(500);
}

void connectToNetwork() {
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

void printResponse() {
  while (ESP01.available()) {
    Serial.println(ESP01.readStringUntil('\n'));
  }
}

String generateRandomCode() {
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

void charOutNumbers(char* numbers){
  int count;

  while(numbers[count] != '\0'){
    slashDotDetection(numbers[count]);
    count++;
  }
  delay(morseDelay);
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
