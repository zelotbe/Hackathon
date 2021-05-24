#include <Arduino.h>
#include <Wire.h>

#define ADDRESS 0x20

const byte settings[6][2] = {
  //{address, setting}
  {0x00, 0xFF}, // Set portA as input for encoders
  {0x01, 0xFC}, // Set portB as input for buttons, output for LEDs
  {0x04, 0x3F}, // Set interrupts for encoders
  {0x05, 0xE0}, // Set interrupts for buttons
  {0x0C, 0xFF}, // Enable pull-up on portA
  {0x0D, 0xFC} // Enable pull-up on upper portB
};

void initExpander() {
  // Encoder: portA (0-5)
  // Buttons: portB (7-5)
  // Leds: portB (0-1)
  Wire.begin(); // Join bus as master

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Starting...");
  initExpander();
  writeGreenLed(HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(2)) {
    byte resultPortA;
    Wire.beginTransmission(ADDRESS);
    Wire.write(0x10);
    Wire.endTransmission();
    Wire.requestFrom(ADDRESS, 1);
    while (Wire.available()) {
      resultPortA = Wire.read();
    }
    Serial.println(resultPortA, BIN);
  }
  if(digitalRead(3)) {
    byte resultPortB;
    Wire.beginTransmission(ADDRESS);
    Wire.write(0x11);
    Wire.endTransmission();
    Wire.requestFrom(ADDRESS, 1);
    while (Wire.available()) {
      resultPortB = Wire.read();
    }
    Serial.println(resultPortB, BIN); // If only 7 bits get printed, first 0 is not printed, there is no zero-fill
  }

}