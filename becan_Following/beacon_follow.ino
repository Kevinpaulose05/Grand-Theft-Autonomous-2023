// Define Motor Connections
// Right Front Motor
#define MF_PWMA 9
#define MF_AI1 10
#define MF_AI2 11
 
// Left Front Motor
#define MF_PWMB 12
#define MF_BI1 14
#define MF_BI2 13

// Right Rear Motor
#define MR_PWMA 4
#define MR_AI1 5
#define MR_AI2 6
 
// Left Rear Motor
#define MR_PWMB 7
#define MR_BI1 15
#define MR_BI2 16


#define Bleft_H 2
#define Bleft_L 1
#define Bright_H 38
#define Bright_L 39

#define Encoder_parts   20

int rf_PWM = 120;  // Right Front Motor
int lf_PWM = 120;  // Left Front Motor
int rr_PWM =120;  // Right Rear Motor
int lr_PWM=120; // Left Front Motor

int FR_RPM = 0;
int FL_RPM = 0;
int BR_RPM = 0;
int BL_RPM = 0;

unsigned long counter_FR_1 = 0;
unsigned long counter_FR_2 = 0;
unsigned long counter_FL_1 = 0;
unsigned long counter_BR_1 = 0;
unsigned long counter_BL_1 = 0;
unsigned long counter_FL_2 = 0;
unsigned long counter_BR_2 = 0;
unsigned long counter_BL_2 = 0;
unsigned long counter_FR = 0;
unsigned long counter_FL = 0;
unsigned long counter_BR = 0;
unsigned long counter_BL = 0;

bool flag_FR=0;
bool flag_FL=0;
bool flag_BR=0;
bool flag_BL=0;

 
// Define Bytes to represent Mecannum Wheel Modes
// Individual bits define TB6612FNG motor driver module input states
// B7 = MF_AI1, B6 = MF_AI2, B5 = MF_BI1, B4 = MF_BI2, B3 = MR_AI1, B2 = MR_AI2, B1 = MR_BI1, B0 = MR_BI2
 
const byte MEC_FORWARD = B10101010;
const byte MEC_BACKWARD = B01010101;
const byte MEC_RIGHT = B01101001;
const byte MEC_LEFT = B10010110;
const byte MEC_ROTATE_CLOCKWISE = B01100110;
const byte MEC_ROTATE_COUNTERCLOCKWISE = B10011001;
 
// Variable for test time delay
int timeDelay = 1000;
 
// PWM Parameters for motor control
// PWM Frequency = 1KHz
const int mtrPWMFreq = 1000;
// PWM Resolution
const int mtrPWMResolution = 8;
// Define PWM channels for each motor
const int mtrRFpwmchannel = 4;
const int mtrLFpwmchannel = 5;
const int mtrRRpwmchannel = 6;
const int mtrLRpwmchannel = 7;

static bool highDetected = false;


void moveMotors(int speedRF, int speedLF, int speedRR, int speedLR, byte dircontrol) {
 
  // Moves all 4 motors
  // Directions specified in direction byte
 
  // Right Front Motor
  digitalWrite(MF_AI1, bitRead(dircontrol, 7));
  digitalWrite(MF_AI2, bitRead(dircontrol, 6));
  ledcWrite(mtrRFpwmchannel, abs(speedRF));
 
  // Left Front Motor
  digitalWrite(MF_BI1, bitRead(dircontrol, 5));
  digitalWrite(MF_BI2, bitRead(dircontrol, 4));
  ledcWrite(mtrLFpwmchannel, abs(speedLF));
 
  // Right Rear Motor
  digitalWrite(MR_AI1, bitRead(dircontrol, 3));
  digitalWrite(MR_AI2, bitRead(dircontrol, 2));
  ledcWrite(mtrRRpwmchannel, abs(speedRR));
 
  // Left Rear Motor
  digitalWrite(MR_BI1, bitRead(dircontrol, 1));
  digitalWrite(MR_BI2, bitRead(dircontrol, 0));
  ledcWrite(mtrLRpwmchannel, abs(speedLR));
}
 
void stopMotors() {
 
  // Stops all motors and motor controllers
  ledcWrite(mtrRFpwmchannel, 0);
  ledcWrite(mtrLFpwmchannel, 0);
  ledcWrite(mtrRRpwmchannel, 0);
  ledcWrite(mtrLRpwmchannel, 0);
 
  digitalWrite(MF_AI1, 0);
  digitalWrite(MF_AI2, 0);
  digitalWrite(MF_BI1, 0);
  digitalWrite(MF_BI2, 0);
  digitalWrite(MR_AI1, 0);
  digitalWrite(MR_AI2, 0);
  digitalWrite(MR_BI1, 0);
  digitalWrite(MR_BI2, 0);
  Serial.println("Stop");
}



void handleForward(){
  moveMotors(rf_PWM, lf_PWM, rr_PWM, lr_PWM, MEC_FORWARD);
}

void setup() {
 
  // Set up Serial Monitor
  Serial.begin(115200);

 
  // Set all connections as outputs
  pinMode(MF_PWMA, OUTPUT);
  pinMode(MF_AI1, OUTPUT);
  pinMode(MF_AI2, OUTPUT);
  pinMode(MF_PWMB, OUTPUT);
  pinMode(MF_BI1, OUTPUT);
  pinMode(MF_BI2, OUTPUT);
  pinMode(MR_PWMA, OUTPUT);
  pinMode(MR_AI1, OUTPUT);
  pinMode(MR_AI2, OUTPUT);
  pinMode(MR_PWMB, OUTPUT);
  pinMode(MR_BI1, OUTPUT);
  pinMode(MR_BI2, OUTPUT);
  pinMode(Bleft_H, INPUT);
  pinMode(Bleft_L, INPUT);
  pinMode(Bright_L, INPUT);
  pinMode(Bright_H, INPUT);
 
  //Set up PWM channels with frequency and resolution
  ledcSetup(mtrRFpwmchannel, mtrPWMFreq, mtrPWMResolution);
  ledcSetup(mtrLFpwmchannel, mtrPWMFreq, mtrPWMResolution);
  ledcSetup(mtrRRpwmchannel, mtrPWMFreq, mtrPWMResolution);
  ledcSetup(mtrLRpwmchannel, mtrPWMFreq, mtrPWMResolution);
 
  // Attach channels to PWM output pins
  ledcAttachPin(MF_PWMA, mtrRFpwmchannel);
  ledcAttachPin(MF_PWMB, mtrLFpwmchannel);
  ledcAttachPin(MR_PWMA, mtrRRpwmchannel);
  ledcAttachPin(MR_PWMB, mtrLRpwmchannel);
  delay(5000);
  
}
 
void loop() {
  Serial.println(digitalRead(Bleft_H));
  while (!highDetected) {
     moveMotors(100, 100, 100, 100, MEC_RIGHT);
     if (digitalRead(Bleft_L) == HIGH && digitalRead(Bright_L) == HIGH) {
    // Both sensors are high, move forward
      highDetected = true;
      break;
     }
     delay(2500);
     moveMotors(100, 100, 100, 100, MEC_FORWARD);
     if (digitalRead(Bleft_H) == HIGH && digitalRead(Bright_H) == HIGH) {
    // Both sensors are high, move forward
      highDetected = true;
      break;
     }
     delay(1000);
     moveMotors(100, 100, 100, 100, MEC_LEFT);
     if (digitalRead(Bleft_H) == HIGH && digitalRead(Bright_H) == HIGH) {
    // Both sensors are high, move forward
      highDetected = true;
      break;
     }
     delay(2500);
     moveMotors(100, 100, 100, 100, MEC_FORWARD);
     if (digitalRead(Bleft_H) == HIGH && digitalRead(Bright_H) == HIGH) {
    // Both sensors are high, move forward
      highDetected = true;
      break;
     }
     delay(1000);
  }
 
  if (digitalRead(Bleft_H) == HIGH && digitalRead(Bright_H) == HIGH) {
    // Both sensors are high, move forward
    highDetected = true;
    moveMotors(100, 100, 100, 100, MEC_FORWARD);
  
  } else if (highDetected) {
    // Stop the motors if the sensors are not both high
    moveMotors(50, 50, 50, 50, MEC_ROTATE_COUNTERCLOCKWISE);
  }
  
  
  delay(100); // Add a small delay to avoid rapid checking
}
