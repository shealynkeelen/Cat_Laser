//calling in libraries for IR remote and servo motors
#include <IRremote.h>
#include <Servo.h>

//defining IR receiver pin
const int RECV_PIN = A0;

//defining two servo motors 
Servo servo1;   //controlls left to right
Servo servo2;   //controlls up dowm motion

//remote command values found from remote
const byte CMD_OFF     = 0x45;   //power button
const byte CMD_SLOW    = 0x0C;   //button 1
const byte CMD_REGULAR = 0x18;   //button 2
const byte CMD_FAST    = 0x5E;   //button 3

//defining different opperating modes
//goal was three modes; slow, medium, and fast
enum Mode {
  MODE_OFF,
  MODE_REGULAR,
  MODE_FAST,
  MODE_SLOW
};

//creating variable to store current mode
//starting with system off
Mode currentMode = MODE_OFF;

//creating a variable to track the last time the laser moved
unsigned long lastMoveTime = 0;

//delay betweem movements
int moveDelay = 1000;

//setting limits to the range of motion of the motors
int panMin = 20;
int panMax = 160;
int tiltMin = 45;
int tiltMax = 135;

//defining starting position of the motors
int panPos = 90;
int tiltPos = 90;

//creating function to check remote signal
void checkRemote() {
  //if a signal has been received by the remote
  if (IrReceiver.decode()) {
    //getting command value from the remote
    byte cmd = IrReceiver.decodedIRData.command;

    //checking which button was pressed and updaing mode accoringly
    if (cmd == CMD_OFF) {
      currentMode = MODE_OFF; //turn system off
    }

    else if (cmd == CMD_SLOW) {
      currentMode = MODE_SLOW;    //put system in slow mode
      moveDelay = 1800;           //larger delay between motions
    }
    else if (cmd == CMD_REGULAR) {
      currentMode = MODE_REGULAR; //system operating at medium speed
      moveDelay = 1000;           //slightly smaller delay
    }   
    else if (cmd == CMD_FAST) { 
      currentMode = MODE_FAST;    //sytem operating in fast mode
      moveDelay = 300;            //much shorter delay between movements
    }

    //get ready to receive the next signal
    IrReceiver.resume();
    delay(150);
  }
}

//creating function to control the motion of the laser
void moveLaser() {
  //track the current time
  unsigned long currentTime = millis();

  //only move when the correct amount of time has passed
  if (currentTime - lastMoveTime >= (unsigned long)moveDelay) {

    //declare variables for new vertical and horizontal positions
    int newPan;
    int newTilt;

    //declare variable for minimum motion
    int minMove;

    //defining different movement sizes for each mode
    if (currentMode == MODE_FAST) {
      minMove = 45; //larger motion
    }
    else if (currentMode == MODE_SLOW) {
      minMove = 20; //smaller movements allowed
    }
    else {
      minMove = 30;
    }

    //generate a new left/right position thats greater than the minimum movement
    do {
      newPan = random(panMin, panMax + 1);
    }
    while (abs(newPan - panPos) < minMove);
```
    //generate a new up/down position thats greater than the minimum movement
    do {
      newTilt = random(tiltMin, tiltMax + 1);
    }
    while (abs(newTilt - tiltPos) < minMove);

    //update positions
    panPos = newPan;
    tiltPos = newTilt;

    //move servo motors to new positions
    servo1.write(panPos);
    servo2.write(tiltPos);

    //update last motion time
    lastMoveTime = currentTime;
  }
}

void setup() {
  //starting serial communication
  Serial.begin(9600);

  //assigning servos to the correct pins
  servo1.attach(3);      
  servo2.attach(6);

  //moving the motors into their initial positions
  servo1.write(panPos);
  servo2.write(tiltPos);

  //initialize the IR receiver 
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);

  //random generator based on an unused pin
  randomSeed(analogRead(A1));
}

void loop() {
  //check for remote input
  checkRemote();

  //only move the laser if the system is not in off mode
  if (currentMode != MODE_OFF) {
    moveLaser();
  }
}