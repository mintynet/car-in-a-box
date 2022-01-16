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
  Serial.println("Setup for 9V motor supply:");
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
    case 0 ... 24:
      abs_pwm_value = 30;
      break;
    case 25 ... 26:
      abs_pwm_value = 31;
      break;
    case 27 ... 28:
      abs_pwm_value = 32;
      break;
    case 29 ... 30:
      abs_pwm_value = 33;
      break;
    case 31:
      abs_pwm_value = 34;
      break;
    case 32 ... 33:
      abs_pwm_value = 35;
      break;
    case 34 ... 37:
      abs_pwm_value = 36;
      break;
    case 38 ... 40:
      abs_pwm_value = 37;
      break;
    case 41 ... 42:
      abs_pwm_value = 38;
      break;
    case 43 ... 45:
      abs_pwm_value = 39;
      break;
    case 46 ... 47:
      abs_pwm_value = 40;
      break;
    case 48:
      abs_pwm_value = 41;
      break;
    case 49 ... 51:
      abs_pwm_value = 42;
      break;
    case 52 ... 53:
      abs_pwm_value = 43;
      break;
    case 54 ... 56:
      abs_pwm_value = 44;
      break;
    case 57 ... 58:
      abs_pwm_value = 45;
      break;
    case 59 ... 61:
      abs_pwm_value = 46;
      break;
    case 62:
      abs_pwm_value = 47;
      break;
    case 63 ... 65:
      abs_pwm_value = 48;
      break;
    case 66:
      abs_pwm_value = 49;
      break;
    case 67:
      abs_pwm_value = 50;
      break;
    case 68 ... 70:
      abs_pwm_value = 51;
      break;
    case 71 ... 72:
      abs_pwm_value = 52;
      break;
    case 73 ... 75:
      abs_pwm_value = 53;
      break;
    case 76 ... 77:
      abs_pwm_value = 54;
      break;
    case 78 ... 79:
      abs_pwm_value = 55;
      break;
    case 80:
      abs_pwm_value = 56;
      break;
    case 81 ... 82:
      abs_pwm_value = 57;
      break;
    case 83:
      abs_pwm_value = 58;
      break;
    case 84:
      abs_pwm_value = 59;
      break;
    case 85 ... 87:
      abs_pwm_value = 60;
      break;
    case 88:
      abs_pwm_value = 61;
      break;
    case 89 ... 90:
      abs_pwm_value = 62;
      break;
    case 91:
      abs_pwm_value = 63;
      break;
    case 92 ... 93:
      abs_pwm_value = 64;
      break;
    case 94 ... 96:
      abs_pwm_value = 65;
      break;
    case 97:
      abs_pwm_value = 66;
      break;
    case 98 ... 99:
      abs_pwm_value = 67;
      break;
    case 100 ... 101:
      abs_pwm_value = 68;
      break;
    case 102:
      abs_pwm_value = 69;
      break;
    case 103:
      abs_pwm_value = 70;
      break;
    case 104:
      abs_pwm_value = 71;
      break;
    case 105:
      abs_pwm_value = 72;
      break;
    case 106 ... 107:
      abs_pwm_value = 73;
      break;
    case 108 ... 109:
      abs_pwm_value = 74;
      break;
    case 110:
      abs_pwm_value = 75;
      break;
    case 111:
      abs_pwm_value = 76;
      break;
    case 112:
      abs_pwm_value = 77;
      break;
    case 113:
      abs_pwm_value = 78;
      break;
    case 114:
      abs_pwm_value = 79;
      break;
    case 115 ... 116:
      abs_pwm_value = 80;
      break;
    case 117:
      abs_pwm_value = 81;
      break;
    case 118:
      abs_pwm_value = 82;
      break;
    case 119:
      abs_pwm_value = 83;
      break;
    case 120 ... 121:
      abs_pwm_value = 84;
      break;
    case 122:
      abs_pwm_value = 85;
      break;
    case 123:
      abs_pwm_value = 86;
      break;
    case 124:
      abs_pwm_value = 87;
      break;
    case 125:
      abs_pwm_value = 88;
      break;
    case 126:
      abs_pwm_value = 89;
      break;
    case 127 :
      abs_pwm_value = 90;
      break;
    case 128:
      abs_pwm_value = 91;
      break;
    case 129:
      abs_pwm_value = 92;
      break;
    case 130:
      abs_pwm_value = 95;
      break;
    case 131:
      abs_pwm_value = 96;
      break;
    case 132:
      abs_pwm_value = 97;
      break;
    case 133:
      abs_pwm_value = 99;
      break;
    case 134:
      abs_pwm_value = 100;
      break;
    case 135:
      abs_pwm_value = 102;
      break;
    case 136:
      abs_pwm_value = 103;
      break;
    case 137 ... 138:
      abs_pwm_value = 104;
      break;
    case 139:
      abs_pwm_value = 105;
      break;
    case 140:
      abs_pwm_value = 107;
      break;
    case 141:
      abs_pwm_value = 108;
      break;
    case 142 ... 143:
      abs_pwm_value = 109;
      break;
    case 144:
      abs_pwm_value = 110;
      break;
    case 145:
      abs_pwm_value = 111;
      break;
    case 146:
      abs_pwm_value = 112;
      break;
    case 147:
      abs_pwm_value = 113;
      break;
    case 148 ... 160:
      abs_pwm_value = 114;
      break;
  }

  if (abs_pot_value < 5) {
    abs_pwm_value = 0;
  }
  //abs_pwm_value = mph_req;
  
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
