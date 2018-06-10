#define latchPin D1
#define clockPin D2
#define dataPin D3

#define LED 7

volatile byte controllerData[16] = {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1};
byte mouseData[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
volatile int clockCycle = 0;

byte tmpMouseData[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
byte tmpMouseClick = 0;

byte sensitivity = 0;

int sensitivityCycle = 0;

unsigned int localPort = 8888;

UDP Udp;

SYSTEM_THREAD(ENABLED);

void setup() {
  waitUntil(Particle.connected);
  Udp.begin(localPort);
  Serial.begin(9600);
  Serial.println(WiFi.localIP());
  
  pinMode(latchPin, INPUT);
  pinMode(clockPin, INPUT);
  pinMode(dataPin, OUTPUT);
  
  attachInterrupt(latchPin, latch, RISING);
  attachInterrupt(clockPin, snesClock, RISING);
  
  pinMode(LED, OUTPUT);                            
  digitalWrite(LED, LOW);
}

void loop() {
    int size = Udp.parsePacket();
    if (size > 0) {
        
        //Serial.println(size, 1);
        
        // Read first char of data received
        char mouse = Udp.read();
        char mouseX = Udp.read();
        char mouseY = Udp.read();

        //Serial.println(mouseX >> 7 & 1, 1);
       // Serial.println(mouseY, 1);
        
        for (int i=0; i<8; i++) {
            tmpMouseData[i] = mouseY >> (7 - i) & 1;
            tmpMouseData[i + 8] = mouseX >> (7 - i) & 1;
        }
        
       tmpMouseClick = mouse;
        
        //Serial.println(mouse, 1);*/
    }
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
  
  memcpy(&mouseData, &tmpMouseData, 16);
 
 controllerData[9] = tmpMouseClick;
 
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