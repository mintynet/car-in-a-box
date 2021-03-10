#include "mcp_can.h"                              // version 01/05/17
#include <SPI.h>                                  // version 1.0.0
#include <LiquidCrystal_I2C.h>                    // version 1.1.2
#include <Wire.h>                                 // version 1.0.0

long unsigned int         rxId;
unsigned char         len = 0;
unsigned char         rxBuf[8];
char                  msgString[128];             // Array to store serial string

#define CAN0_INT 2                                // Set INT to pin 2
#define I2CLCD1Add        0x27
MCP_CAN                   CAN0(10);               // Set CS to pin 10

byte xPos                 = 0;                    // LCD xPos
byte yPos                 = 0;                    // LCD yPos

int abs_pot_pin           = A0;
int abs_pot_value         = 0;
int abs_pwm_pin           = 6; 
int abs_pwm_value         = 16;

int kph_req               = 0;
int kph_value             = 16000;

int mph_req               = 0;
int mph_value             = 100;

long previousMillis       = 0;        
long update_interval      = 100;

LiquidCrystal_I2C         lcd (I2CLCD1Add,20,4);  // initialize the 20x4

// the 8 arrays that form each segment of the custom numbers
byte LT[8] = {
  B00111,
  B01111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte UB[8] = {
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte RT[8] = {
  B11100,
  B11110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte LL[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B01111,
  B00111
};
byte LB[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
};
byte LR[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11110,
  B11100
};
byte UMB[8] = {
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111
};
byte LMB[8] = {
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
};

//----------------------------------------------------------------

static void custom0O() {
  // uses segments to build the number 0
  lcd.setCursor(xPos, yPos);
  lcd.write(8);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(xPos, yPos + 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
} // custom0O

//----------------------------------------------------------------

static void custom1() {
  lcd.setCursor(xPos, yPos);
  lcd.print(" ");
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(xPos, yPos + 1);
  lcd.print("  ");
  lcd.write(255);
} // custom1

//----------------------------------------------------------------

static void custom2() {
  lcd.setCursor(xPos, yPos);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(xPos, yPos + 1);
  lcd.write(3);
  lcd.write(7);
  lcd.write(7);
} // custom2

//----------------------------------------------------------------

static void custom3() {
  lcd.setCursor(xPos, yPos);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(xPos, yPos + 1);
  lcd.write(7);
  lcd.write(7);
  lcd.write(5);
} // custom3

//----------------------------------------------------------------

static void custom4() {
  lcd.setCursor(xPos, yPos);
  lcd.write(3);
  lcd.write(4);
  lcd.write(2);
  lcd.setCursor(xPos, yPos + 1);
  lcd.print("  ");
  lcd.write(255);
} // custom4

//----------------------------------------------------------------

static void custom5() {
  lcd.setCursor(xPos, yPos);
  lcd.write(255);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(xPos, yPos + 1);
  lcd.write(7);
  lcd.write(7);
  lcd.write(5);
} // custom5

//----------------------------------------------------------------

static void custom6() {
  lcd.setCursor(xPos, yPos);
  lcd.write(8);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(xPos, yPos + 1);
  lcd.write(3);
  lcd.write(7);
  lcd.write(5);
} // custom6

//----------------------------------------------------------------

static void custom7() {
  lcd.setCursor(xPos, yPos);
  lcd.write(1);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(xPos, yPos + 1);
  lcd.print(" ");
  lcd.write(8);
  lcd.print(" ");
} // custom7

//----------------------------------------------------------------

static void custom8() {
  lcd.setCursor(xPos, yPos);
  lcd.write(8);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(xPos, yPos + 1);
  lcd.write(3);
  lcd.write(7);
  lcd.write(5);
} // custom8

//----------------------------------------------------------------

static void custom9() {
  lcd.setCursor(xPos, yPos);
  lcd.write(8);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(xPos, yPos + 1);
  lcd.print("  ");
  lcd.write(255);
} // custom9

//----------------------------------------------------------------

static void printNum(char i, int XPOS, int YPOS) {
  xPos = XPOS;
  yPos = YPOS;
  switch (i) {
    case 0:
      custom0O();
      break;
    case 1:
      custom1();
      break;
    case 2:
      custom2();
      break;
    case 3:
      custom3();
      break;
    case 4:
      custom4();
      break;
    case 5:
      custom5();
      break;
    case 6:
      custom6();
      break;
    case 7:
      custom7();
      break;
    case 8:
      custom8();
      break;
    case 9:
      custom9();
      break;
  }
} // printNum()

//----------------------------------------------------------------

static void disp2digits(int iOut, int XPOS, int YPOS) {
  //char temptest;
  String testStr = String(iOut);
  if (iOut == 0) {
    printNum(0, XPOS, YPOS);
    printNum(0, XPOS + 3, YPOS);
  } else if (iOut < 10) {
    printNum(0, XPOS, YPOS);
    printNum(testStr[0] - 48, XPOS + 3, YPOS);
  } else {
    printNum(testStr[0] - 48, XPOS, YPOS);
    printNum(testStr[1] - 48, XPOS + 3, YPOS);
  }
} // disp2digits()

//----------------------------------------------------------------

static void disp3digits(int iOut, int XPOS, int YPOS) {
  //char temptest;
  String testStr = String(iOut);
  if (iOut == 0) {
    printNum(0, XPOS, YPOS);
    printNum(0, XPOS + 3, YPOS);
    printNum(0, XPOS + 6, YPOS);
  } else if (iOut < 10) {
    printNum(0, XPOS, YPOS);
    printNum(0, XPOS + 3, YPOS);
    printNum(testStr[0] - 48, XPOS + 6, YPOS);
  } else if (iOut < 100) {
    printNum(0, XPOS, YPOS);
    printNum(testStr[0] - 48, XPOS + 3, YPOS);
    printNum(testStr[1] - 48, XPOS + 6, YPOS);
  } else {
    printNum(testStr[0] - 48, XPOS, YPOS);
    printNum(testStr[1] - 48, XPOS + 3, YPOS);
    printNum(testStr[2] - 48, XPOS + 6, YPOS);
  }
} // disp3digits()

//----------------------------------------------------------------

static void disp4digits(long iOut, int XPOS, int YPOS) {
  //char temptest;
  String testStr = String(iOut);
  if (iOut == 0) {
    printNum(0, XPOS, YPOS);
    printNum(0, XPOS + 3, YPOS);
    printNum(0, XPOS + 6, YPOS);
    printNum(0, XPOS + 9, YPOS);
  } else if (iOut < 10) {
    printNum(0, XPOS, YPOS);
    printNum(0, XPOS + 3, YPOS);
    printNum(0, XPOS + 6, YPOS);
    printNum(testStr[0] - 48, XPOS + 9, YPOS);
  } else if (iOut < 100) {
    printNum(0, XPOS, YPOS);
    printNum(0, XPOS + 3, YPOS);
    printNum(testStr[0] - 48, XPOS + 6, YPOS);
    printNum(testStr[1] - 48, XPOS + 9, YPOS);
  } else if (iOut < 1000) {
    printNum(testStr[0] - 48, XPOS + 3, YPOS);
    printNum(testStr[1] - 48, XPOS + 6, YPOS);
    printNum(testStr[2] - 48, XPOS + 9, YPOS);
  } else {
    printNum(testStr[0] - 48, XPOS, YPOS);
    printNum(testStr[1] - 48, XPOS + 3, YPOS);
    printNum(testStr[2] - 48, XPOS + 6, YPOS);
    printNum(testStr[3] - 48, XPOS + 9, YPOS);
  }
} // disp4digits()

//----------------------------------------------------------------

void setup() {
  pinMode(abs_pwm_pin, OUTPUT);
  Serial.begin(9600);
  if(CAN0.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                     // Configuring pin for /INT input
  
  Serial.println("PD0 ABS SIM...");
  lcd.init();
  lcd.clear();
  lcd.createChar(1, UB);
  lcd.createChar(2, RT);
  lcd.createChar(3, LL);
  lcd.createChar(4, LB);
  lcd.createChar(5, LR);
  lcd.createChar(6, UMB);
  lcd.createChar(7, LMB);
  lcd.createChar(8, LT);
  lcd.backlight();
} //setup()

//----------------------------------------------------------------

void loop() {
  unsigned long currentMillis = millis();
  //Reading from potentiometer
  abs_pot_value = 1023-analogRead(abs_pot_pin);
  //Mapping the Values between 0 to 255 because we can give output
  //from 0 - 255 using the analogwrite funtion
  //pwm_value = map(pot_value, 0, 1023, 16, 255);
  kph_req = map(abs_pot_value,0,1023,0,23808);
  mph_req = kph_req/100/1.6;
  switch (mph_req) {
    case 0 ... 10:
      abs_pwm_value = 10;
      break;
    case 11 ... 20:
      abs_pwm_value = 20;
      break;
    case 21 ... 30:
      abs_pwm_value = 30;
      break;
    case 31 ... 40:
      abs_pwm_value = 40;
      break;
    case 41 ... 50:
      abs_pwm_value = 50;
      break;
    case 51 ... 60:
      abs_pwm_value = 60;
      break;
    case 61 ... 68:
      abs_pwm_value = 70;
      break;
    case 69 ... 74:
      abs_pwm_value = 71;
      break;
    case 75 ... 76:
      abs_pwm_value = 73;
      break;
    case 77 ... 79:
      abs_pwm_value = 74;
      break;
    case 80 ... 82:
      abs_pwm_value = 75;
      break;
    case 83 ... 86:
      abs_pwm_value = 76;
      break;
    case 87 ... 90:
      abs_pwm_value = 77;
      break;
    case 91 ... 94:
      abs_pwm_value = 78;
      break;
    case 95 ... 96:
      abs_pwm_value = 79;
      break;
    case 97 ... 99:
      abs_pwm_value = 80;
      break;
    case 100 ... 102:
      abs_pwm_value = 81;
      break;
    case 103 ... 106:
      abs_pwm_value = 82;
      break;
    case 107 ... 108:
      abs_pwm_value = 83;
      break;
    case 109:
      abs_pwm_value = 84;
      break;
    case 110 ... 113:
      abs_pwm_value = 85;
      break;
    case 114 ... 115:
      abs_pwm_value = 86;
      break;
    case 116:
      abs_pwm_value = 87;
      break;
    case 117:
      abs_pwm_value = 88;
      break;
    case 118 ... 123:
      abs_pwm_value = 89;
      break;
    case 124 ... 125:
      abs_pwm_value = 90;
      break;
    case 126 ... 129:
      abs_pwm_value = 91;
      break;
    case 130:
      abs_pwm_value = 92;
      break;
  }
  if (abs_pot_value < 5) {
    abs_pwm_value = 0;
  }
  analogWrite(abs_pwm_pin, abs_pwm_value);
  delay(1);
 
  if(!digitalRead(CAN0_INT))
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);
    
    if(rxId == 0xB6)
    {
      kph_value = (rxBuf[2] * 256) + rxBuf[3];
      mph_value = kph_value/100/1.6;
    }
  }
  //lcd.clear();
  if(currentMillis - previousMillis > update_interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   
    Serial.print("Pot\t:");
    Serial.print(abs_pot_value);
    Serial.print("\t,PWM:\t");
    Serial.print(abs_pwm_value);
    Serial.print(",KPH REQ\t:");
    Serial.print(kph_req/100);
    Serial.print(",KPH CAN\t:");
    Serial.print(kph_value/100);
    Serial.print(" millis():\t");
    Serial.println(millis());
    lcd.setCursor(0,0);
    lcd.print("PD0:");
    lcd.setCursor(6,0);
    lcd.print("req");
    lcd.setCursor(6,1);
    lcd.print("mph");
    lcd.setCursor(6,2);
    lcd.print("cur");
    lcd.setCursor(6,3);
    lcd.print("mph");
    disp3digits(mph_req, 10, 0);
    disp3digits(mph_value, 10, 2);
    lcd.setCursor(0,3);
    if(abs_pwm_value < 10) {
      lcd.print("0");
    }
    if(abs_pwm_value < 100) {
      lcd.print("0");
    }
    lcd.print(abs_pwm_value);
  }
} //loop()
