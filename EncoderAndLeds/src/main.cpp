#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

#define ADDRESS 0x20
#define DEVIATION 30

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

const byte targetLedPins[] = {
  0, // Target R
  13, // Target G
  12, // Target B
};

const int currentLedPins[] = {
  14, // Current R
  1, // Current G
  3 // Current B
};

byte targetLedValues[] = {0, 0, 0};
byte currentLedValues[] = {0, 0, 0};

bool codeCorrect = false;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, 16, 5, 4);


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

boolean checkForCodeMatch() {
  for(byte i = 0; i < sizeof(targetLedValues); i++) {
    if(currentLedValues[i] - DEVIATION < targetLedValues[i]) return false;
    if(currentLedValues[i] + DEVIATION > targetLedValues[i]) return false;
  }
  return true;
}

void updateCurrentLed() {
  for(byte i = 0; i < sizeof(currentLedValues); i++) analogWrite(currentLedPins[i], currentLedValues[i]);
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
    currentLedValues[0] += 5;
    if (currentLedValues[0] > 255) currentLedValues[0] = 255;
  }

  //Counter-Clockwise
  if(((prevResultRed == 0x00 && currentResultRed == 0x02) || (prevResultRed == 0x02 && currentResultRed == 0x03)) || ((prevResultRed == 0x03 && currentResultRed == 0x01) || (prevResultRed == 0x01 && currentResultRed == 0x00))) {
    currentLedValues[0] -= 5;
    if (currentLedValues[0] < 0) currentLedValues[0] = 0;
  }


  // Detect change in green
  // Clockwise
  if(((prevResultGreen == 0x00 && currentResultGreen == 0x01) || (prevResultGreen == 0x01 && currentResultGreen == 0x03)) || ((prevResultGreen == 0x03 && currentResultGreen == 0x02) || (prevResultGreen == 0x02 && currentResultGreen == 0x00))) {
    currentLedValues[1] += 5;
    if (currentLedValues[1] > 255) currentLedValues[1] = 255;
  }

  //Counter-Clockwise
  if(((prevResultGreen == 0x00 && currentResultGreen == 0x02) || (prevResultGreen == 0x02 && currentResultGreen == 0x03)) || ((prevResultGreen == 0x03 && currentResultGreen == 0x01) || (prevResultGreen == 0x01 && currentResultGreen == 0x00))) {
    currentLedValues[1] -= 5;
    if (currentLedValues[1] < 0) currentLedValues[1] = 0;
  }


  // Detect change in blue
  // Clockwise
  if (((prevResultBlue == 0x00 && currentResultBlue == 0x01) || (prevResultBlue == 0x01 && currentResultBlue == 0x03)) || ((prevResultBlue == 0x03 && currentResultBlue == 0x02) || (prevResultBlue == 0x02 && currentResultBlue == 0x00))) {
    currentLedValues[2] += 5;
    if (currentLedValues[2] > 255) currentLedValues[2] = 255;
  }

  //Counter-Clockwise
  if(((prevResultBlue == 0x00 && currentResultBlue == 0x02) || (prevResultBlue == 0x02 && currentResultBlue == 0x03)) || ((prevResultBlue == 0x03 && currentResultBlue == 0x01) || (prevResultBlue == 0x01 && currentResultBlue == 0x00))) {
    currentLedValues[2] -= 5;
    if (currentLedValues[2] < 0) currentLedValues[2] = 0;
  }

  updateCurrentLed();
  codeCorrect = checkForCodeMatch();
  prevEncoderValue = result;

}

void newPWMValues() {
  for(byte i = 0; i < sizeof(targetLedPins); i++) {
    targetLedValues[i] = random(0, 1024);
    analogWrite(targetLedPins[i], targetLedValues[i]);
  }
}

void reset() {
  // Reset the code and the colorvalues
  newPWMValues();
}

void previousNumber() {
  // Reset the colorvalues and get previous digit of code
}

void nextNumber() {
  // Reset the colorvalues and get next digit of code
  newPWMValues();
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
  u8g2.begin();

  u8g2.firstPage();
  u8g2.setFont(u8g2_font_logisoso32_tf);
  do {
    u8g2.setCursor(10, 30);
    u8g2.print("test");
  } while (u8g2.nextPage());

  initExpander();

  for(byte pin: targetLedPins) pinMode(pin, OUTPUT);
  for (byte pin : currentLedPins) pinMode(pin, OUTPUT);

  newPWMValues();

  writeRedLed(HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(2)) {
    handleEncoder();
  }
  if(digitalRead(3)) {
    handleButton();
  }
  while(codeCorrect) {
    // Send morse

    // Handle next, previous, reset
    if(digitalRead(3)) {
      handleButton();
    }
  }
}
