#define latchPin D1
#define clockPin D2
#define dataPin D3

#define LED 7

volatile byte controllerData[16] = {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1};
volatile byte mouseData[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
volatile int clockCycle = 0;

byte sensitivity = 0;

int sensitivityCycle = 0;

void setup() {
  pinMode(latchPin, INPUT);
  pinMode(clockPin, INPUT);
  pinMode(dataPin, OUTPUT);
  
  attachInterrupt(latchPin, latch, RISING);
  attachInterrupt(clockPin, snesClock, RISING);
  
  pinMode(LED, OUTPUT);                            
  digitalWrite(LED, LOW);
  
  Particle.subscribe("mouse", mouseHandler);
  
}

void loop() {

}

void setData() {
  if (controllerData[clockCycle]) {
    digitalWrite(dataPin, LOW);
  }
  else {
    digitalWrite(dataPin, HIGH);
  }
}

void setMouseData() {
  if (mouseData[clockCycle - 16]) {
    digitalWrite(dataPin, LOW);
  }
  else {
    digitalWrite(dataPin, HIGH);
  }
}

void latch() {
  if (clockCycle > 32) { // snes wants to change sensitivity
      sensitivityCycle++;
      
      if (sensitivityCycle == 30) {
          clockCycle = 0;
          
          changeSensitivity();
      }
      
      return;
  }   
 
  sensitivityCycle = 0;
  clockCycle = 0;
  setData();
}

void snesClock() {
  clockCycle++;

  if (clockCycle < 16) {
    setData();
  }
  else if (clockCycle < 32) {
    setMouseData();
  }
  else {
    digitalWrite(dataPin, HIGH);
  }
}  

void changeSensitivity() {
    sensitivity++;
    
    if (sensitivity > 2) {
        sensitivity = 0;
    }
    
    switch (sensitivity) {
      case 0:
        controllerData[10] = 1;
        controllerData[11] = 1;
      case 1:
        controllerData[10] = 1;
        controllerData[11] = 0;
      case 2:
        controllerData[10] = 0;
        controllerData[11] = 1;
    }
}

void mouseHandler(String event, String data) {
    digitalWrite(LED, HIGH);
    
    if (data == "up") {
      mouseData[0] = 1;
      mouseData[7] = 1;
    }
    
    if (data == "down") {
        mouseData[0] = 0;
        mouseData[7] = 1;
    }
    
    if (data == "left") {
      mouseData[8] = 1;
      mouseData[15] = 1;
    }
    
    if (data == "right") {
        mouseData[8] = 0;
        mouseData[15] = 1;
    }
    
    if (data == "stop") {
        mouseData[7] = 0;
        mouseData[15] = 0;
        controllerData[9] = 0; 
    }
    
    if (data == "click") {
        controllerData[9] = 1; 
    }
}