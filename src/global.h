#pragma once

#include <Arduino.h>
// Konfig
#include "config.h"

// Main
void mainRegisterLoop(void (*callback)());

// Kommunikation
struct structMsgData
{
  String func;
  uint16_t cnt;
  int32_t parSet[6][8];
};
extern structMsgData MsgData;
void comSetup();
void comLoop();
void sendACK();
void sendFIN();
void sendERR(uint8_t errnum);
void sendPOS();
#ifndef UNO_TEST
void sendSRV();
#else
void sendSRV(int32_t nr, int32_t val);
#endif
void sendESS();
void sendLSS();
void sendRES();

// Robo Ctrl
void roboCtrlSetup();
void roboLoop();
void roboMOV();
void roboREF();
void roboSRV();
void roboWAI();
bool roboGetRefOkay(int i);
int32_t roboGetStepperPos(int i);
int roboGetServoPos(int i);
bool roboGetEstop();
bool roboGetLimitSwitch(int i);
