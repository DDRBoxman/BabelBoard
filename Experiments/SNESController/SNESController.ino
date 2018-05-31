#include "samd21/include/samd21e18a.h"

const byte ledPin = 13;
const byte latchPin = 0;
const byte clockPin = 3;
const byte dataPin = 4;

volatile byte controllerData[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
volatile int clockCycle = 0;

byte stateLight = 0;

void blink() {
  controllerData[2] = !controllerData[2];
}

byte latchState = 0;

void latch() {
  clockCycle = 0;
  setData();

  stateLight = !stateLight;
  if (stateLight) {
    PORT->Group[0].OUTCLR.reg = (1<<10);
  }
  else {
    PORT->Group[0].OUTSET.reg = (1<<10);
  }
}

void snesClock() {
  clockCycle++;

  if (clockCycle < 16) {
    setData();
  }
  else {
    PORT->Group[0].OUTSET.reg = (1<<6);
  }
}  

void setData() {
  if (controllerData[clockCycle]) {
    PORT->Group[0].OUTCLR.reg = (1<<6);
  }
  else {
    PORT->Group[0].OUTSET.reg = (1<<6);
  }
}

int main( void ) {
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), blink, CHANGE);

  pinMode(latchPin, INPUT_PULLUP);
  pinMode(clockPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  digitalWrite(dataPin, HIGH);

  attachInterrupt(digitalPinToInterrupt(latchPin), latch, FALLING);
  attachInterrupt(digitalPinToInterrupt(clockPin), snesClock, RISING);
  
  while(true) {
    
  }

  return 0;
}

