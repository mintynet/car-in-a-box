// SMEG IVI WIRING
// Head unit      Screen    
// Pin 9          Pin 1 12v
// Pin 10         CAN HI to can transceiver
// Pin 11         Pin 2 CAN LO
// Pin 12         12v Live
// Pin 13         CAN LO to can transceiver
// Pin 14         Pin 4 CAN HI
// Pin 15         Pin 3 Gnd
// Pin 16         Gnd
// Arduino NANO with MCP2515 CAN adapter

// Board library definition 1.6.21
// New ATmega328P bootloader

#include <mcp_can.h>                                  // version 25/09/17
#include <SPI.h>                                      // version 1.0.0

MCP_CAN CAN0(10);     // Set CS to pin 10 (pin 2 interupt)

void setup()
{
  Serial.begin(115200);

  // Initialize MCP2515 running at 8MHz with a baudrate of 125kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ) == CAN_OK) Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
}

const unsigned int msg_1 = 0x036;
const unsigned int msg_2 = 0x0F6;
const unsigned int msg_3 = 0x167;
const unsigned int msg_4 = 0x0A2;
const unsigned int msg_5 = 0x0B6;
unsigned int vin_1 = 0x492;
unsigned int vin_2 = vin_1-64;
unsigned int vin_3 = vin_2+32;

byte data_1[8] = {0x0E,0x00,0x64,0x0F,0x31,0x00,0x01,0xAC};
byte data_2[8] = {0x88,0x3C,0xFF,0xFF,0xFF,0xFF,0xFF,0x10};
byte data_3[8] = {0x08,0x10,0x00,0x00,0x7F,0xFF,0x00,0x00};
byte data_4[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
byte data_5[8] = {0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xD0};
byte data_vin_1[3] = {0x56,0x46,0x33};
byte data_vin_2[6] = {0x43,0x41,0x5A,0x4D,0x5A,0x30};
byte data_vin_3[8] = {0x45,0x54,0x30,0x33,0x33,0x36,0x38,0x38};
byte data_end[2] = {0x66,0x66};

const boolean gDEBUG = false;

void msg1() {
  byte sndStat1 = CAN0.sendMsgBuf(msg_1,0,8,data_1);
  if (gDEBUG) {
    if(sndStat1 == CAN_OK){
      Serial.println("1 Sent Successfully: ");
    } else {
      Serial.println("Error Sending 1...");
    }
  }
}

void msg2() {
  byte sndStat2 = CAN0.sendMsgBuf(msg_2,0,8,data_2);
  if (gDEBUG) {
    if(sndStat2 == CAN_OK){
      Serial.println("2 Sent Successfully: ");
    } else {
      Serial.println("Error Sending 2...");
    }
  }  
}

void msg3() {
  byte sndStat3 = CAN0.sendMsgBuf(msg_3,0,8,data_3);
  if (gDEBUG) {
    if(sndStat3 == CAN_OK){
      Serial.println("3 Sent Successfully: ");
    } else {
      Serial.println("Error Sending 3...");
    }
  }
}

void msg4() {
  byte sndStat4 = CAN0.sendMsgBuf(msg_4,0,6,data_4);
  if (gDEBUG) {
    if(sndStat4 == CAN_OK){
      Serial.println("4 Sent Successfully: ");
    } else {
      Serial.println("Error Sending 4...");
    }
  }
}

void msg5() {
  byte sndStat5 = CAN0.sendMsgBuf(msg_5,0,8,data_5);
  if (gDEBUG) {
    if(sndStat5 == CAN_OK){
      Serial.println("5 Sent Successfully: ");
    } else {
      Serial.println("Error Sending 5...");
    }
  }
}

void vin1() {
  byte vin1stat = CAN0.sendMsgBuf(vin_1,0,8,data_vin_1);
  if (!gDEBUG) {
    if(vin1stat == CAN_OK){
      Serial.print(vin_1,HEX);
      Serial.println(" VIN1 Sent Successfully: ");
    } else {
      Serial.println("Error Sending VIN1...");
    }
  }
}

void vin2() {
  byte vin2stat = CAN0.sendMsgBuf(vin_2,0,8,data_vin_2);
  if (!gDEBUG) {
    if(vin2stat == CAN_OK){
      Serial.print(vin_2,HEX);
      Serial.println(" VIN2 Sent Successfully: ");
    } else {
      Serial.println("Error Sending VIN2...");
    }
  }
}

void vin3() {
  byte vin3stat = CAN0.sendMsgBuf(vin_3,0,8,data_vin_3);
  if (!gDEBUG) {
    if(vin3stat == CAN_OK){
      Serial.print(vin_3,HEX);
      Serial.println(" VIN3 Sent Successfully: ");
    } else {
      Serial.println("Error Sending VIN3...");
    }
  }
}

void loop() {
  vin1();
  vin2();
  vin3();
  for (int h=0;h<2;h++) {
    msg4();
    //Serial.print("h ");
    //Serial.println(millis());
    for (int i=0;i<5;i++) {
      msg1();
      msg2();
      msg3();
      msg5();
      delay(26);
      msg3();
      delay(30);
      msg1();
      msg2();
      msg3();
      msg5();
      delay(26);
      //Serial.print("i ");
      //Serial.println(millis());
    }
  }
  byte sndStat = CAN0.sendMsgBuf(0x666,0,2,data_end);
  Serial.print(millis());
  Serial.println(" Done One loop");
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/