#include <Arduino.h>
#include <Wire.h>

#define ADDRESS 0x20

int pwmR = 0;
int pwmG = 0;
int pwmB = 0;

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

int fetchFromAddress(byte addr) {
  byte result;
  Wire.beginTransmission(ADDRESS);
  Wire.write(addr);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS, 1);
  while (Wire.available()) {
    result = Wire.read();
  }
  return result;
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
    pwmR += 5;
    if(pwmR > 255) pwmR = 255;
  }

  //Counter-Clockwise
  if(((prevResultRed == 0x00 && currentResultRed == 0x02) || (prevResultRed == 0x02 && currentResultRed == 0x03)) || ((prevResultRed == 0x03 && currentResultRed == 0x01) || (prevResultRed == 0x01 && currentResultRed == 0x00))) {
    pwmR -= 5;
    if(pwmR < 0) pwmR = 0;
  }


  // Detect change in green
  // Clockwise
  if(((prevResultGreen == 0x00 && currentResultGreen == 0x01) || (prevResultGreen == 0x01 && currentResultGreen == 0x03)) || ((prevResultGreen == 0x03 && currentResultGreen == 0x02) || (prevResultGreen == 0x02 && currentResultGreen == 0x00))) {
    pwmG += 5;
    if(pwmG > 255) pwmG = 255;
  }

  //Counter-Clockwise
  if(((prevResultGreen == 0x00 && currentResultGreen == 0x02) || (prevResultGreen == 0x02 && currentResultGreen == 0x03)) || ((prevResultGreen == 0x03 && currentResultGreen == 0x01) || (prevResultGreen == 0x01 && currentResultGreen == 0x00))) {
    pwmG -= 5;
    if(pwmG < 0) pwmG = 0;
  }


  // Detect change in blue
  // Clockwise
  if (((prevResultBlue == 0x00 && currentResultBlue == 0x01) || (prevResultBlue == 0x01 && currentResultBlue == 0x03)) || ((prevResultBlue == 0x03 && currentResultBlue == 0x02) || (prevResultBlue == 0x02 && currentResultBlue == 0x00))) {
    pwmB += 5;
    if (pwmB > 255) pwmB = 255;
  }

  //Counter-Clockwise
  if(((prevResultBlue == 0x00 && currentResultBlue == 0x02) || (prevResultBlue == 0x02 && currentResultBlue == 0x03)) || ((prevResultBlue == 0x03 && currentResultBlue == 0x01) || (prevResultBlue == 0x01 && currentResultBlue == 0x00))) {
    pwmB -= 5;
    if(pwmB < 0) pwmB = 0;
  }

  prevEncoderValue = result;

}

void reset() {
  // Reset the code and the colorvalues
}

void previousNumber() {
  // Reset the colorvalues and get previous digit of code
}

void nextNumber() {
  // Reset the colorvalues and get next digit of code
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
  Serial.begin(9600);
  Serial.println("Starting...");
  initExpander();
  writeGreenLed(HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(2)) {
    handleEncoder();
  }
  if(digitalRead(3)) {
    handleButton();
  }

}
