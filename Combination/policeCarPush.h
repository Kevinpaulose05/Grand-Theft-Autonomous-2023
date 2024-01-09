
int flag =0;
int error =1000;

#define RGBLED 18 // for ESP32S2 Devkit pin 18, for M5 stamp=2
#define SIGNALPIN1 40 // pin receiving signal from Vive circuit
#define SIGNALPIN2 41 // pin receiving signal from Vive circuit


#define UDPPORT 2510 // For GTA 2022C game 
#define STUDENTIP 170 // choose a teammembers assigned IP number
#define teamNumber 5

#define FREQ 1 // in Hz

Vive510 vive1(SIGNALPIN1);
Vive510 vive2(SIGNALPIN2);



WiFiUDP UDPServer;
IPAddress ipTarget(192, 168, 1, 255); // 255 => broadcast


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

int car_y=0;

int rf_PWM = 120;  // Right Front Motor
int lf_PWM = 120; // Left Front Motor
int rr_PWM = 120; // Right Rear Motor
int lr_PWM = 120; // Left Front Motor

int FR_RPM = 0;
int FL_RPM = 0;
int BR_RPM = 0;
int BL_RPM = 0;


bool flag_FR = 0;
bool flag_FL = 0;
bool flag_BR = 0;
bool flag_BL = 0;


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

const double kp = 0.5;
const double kd = 0.01;
int MAX_PWM=80;

// Target positions
double targetX = 0.0;
double targetY = 0.0;

// Current positions
double currentX = 0.0;
double currentY = 0.0;

// Previous errors for derivative term
double prevErrorX = 0.0;
double prevErrorY = 0.0;
double prevErrorO = 0.0;

void moveMotors_dircontrol(int speedRF, int speedLF, int speedRR, int speedLR, byte dircontrol) {

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

void moveMotors(int speedRF, int speedLF, int speedRR, int speedLR, byte dircontrol) {
  // Moves all 4 motors
  // Directions specified in direction byte

  // Right Front Motor
  digitalWrite(MF_AI1, speedRF > 0);
  digitalWrite(MF_AI2, speedRF < 0);
  ledcWrite(mtrRFpwmchannel, abs(speedRF));

  // Left Front Motor
  digitalWrite(MF_BI1, speedLF > 0);
  digitalWrite(MF_BI2, speedLF < 0);
  ledcWrite(mtrLFpwmchannel, abs(speedLF));

  // Right Rear Motor
  digitalWrite(MR_AI1, speedRR > 0);
  digitalWrite(MR_AI2, speedRR < 0);
  ledcWrite(mtrRRpwmchannel, abs(speedRR));

  // Left Rear Motor
  digitalWrite(MR_BI1, speedLR > 0);
  digitalWrite(MR_BI2, speedLR < 0);
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


double calculatePDEffort(double target, double current, double prevError, double kp, double kd) {
    double error = target - current;
    double derivative = (error - prevError);
    double effort = kp * error + kd * derivative;
    return effort;
}

double calculatePDEffort_Rotation(double target, double current, double prevError, double kp, double kd, int fr_pwm, int fl_pwm, int rr_pwm, int rl_pwm) {
    double error = target - current;
    double derivative = (prevError - error);  // Adjusted the derivative calculation
    double effort = kp * error + kd * derivative;

    // Determine motor direction based on the sign of the error
    if (error > 30) {
        // Clockwise rotation
        moveMotors_dircontrol(fr_pwm, fl_pwm, rr_pwm, rl_pwm, MEC_ROTATE_CLOCKWISE);
    } 
    else if(error < -30) {
        // Anticlockwise rotation
        moveMotors_dircontrol(fr_pwm, fl_pwm, rr_pwm, rl_pwm, MEC_ROTATE_COUNTERCLOCKWISE);
    }
    
    
    return effort;
}


void calculateMecanumWheelSpeeds(double xEffort, double yEffort, int& fl_pwm, int& fr_pwm, int& rl_pwm, int& rr_pwm, int x1 , int x2 ) {
    // Calculate individual wheel speeds based on control efforts
    double front_left, front_right, rear_left, rear_right;
    
    if (x2 >= x1){
      front_left = yEffort - xEffort;
      front_right = -yEffort - xEffort;
      rear_left = -yEffort - xEffort;
      rear_right = yEffort - xEffort;
      }
     else{

      front_left = -yEffort + xEffort;
      front_right = yEffort + xEffort;
      rear_left = +yEffort + xEffort;
      rear_right = -yEffort + xEffort;
      }

    // Normalize speeds to ensure they are within the valid range
    double max_speed = fmax(fabs(front_left), fmax(fabs(front_right), fmax(fabs(rear_left), fabs(rear_right))));
    if (max_speed > 1.0) {
        front_left /= max_speed;
        front_right /= max_speed;
        rear_left /= max_speed;
        rear_right /= max_speed;
    }

    // Scale speeds to PWM values
    fl_pwm = int(front_left * MAX_PWM);
    fr_pwm = int(front_right * MAX_PWM);
    rl_pwm = int(rear_left * MAX_PWM);
    rr_pwm = int(rear_right * MAX_PWM);
}


int moveToTargetCoordinates(double targetX, double targetY, double currentX, double currentY, int x1 , int x2) {
    double effortX = calculatePDEffort(targetX, currentX, prevErrorX, kp, kd);
    double effortY = calculatePDEffort(targetY, currentY, prevErrorY, kp, kd);

    // Calculate mecanum wheel speeds based on control efforts
    int fl_pwm, fr_pwm, rl_pwm, rr_pwm;
    calculateMecanumWheelSpeeds(effortX, effortY, fl_pwm, fr_pwm, rl_pwm, rr_pwm,x1 ,x2);

    // Apply PWM values to move the robot
    moveMotors(fr_pwm, fl_pwm, rr_pwm, rl_pwm, MEC_FORWARD);

    // Update previous errors for the next iteration
    prevErrorX = targetX - currentX;
    prevErrorY = targetY - currentY;
    return prevErrorY;
//    Serial.println(prevErrorY);
}



void UdpSend(int x, int y )
{
  char udpBuffer[20];
//  sprintf(udpBuffer, "%02d:%d:%4d,%4d",teamNumber,vivenumber,x,y);   
  sprintf(udpBuffer, "%02d:%4d,%4d",teamNumber,x,y);
                                              
  UDPServer.beginPacket(ipTarget, UDPPORT);
  UDPServer.println(udpBuffer);
  UDPServer.endPacket();
  Serial.println(udpBuffer);
}
int handleUDPServer() {
   const int UDP_PACKET_SIZE = 14; // can be up to 65535          
   uint8_t packetBuffer[UDP_PACKET_SIZE];

   int cb = UDPServer.parsePacket(); // if there is no message cb=0
   if (cb) {
      int x,y;
      packetBuffer[13]=0; // null terminate string

    UDPServer.read(packetBuffer, UDP_PACKET_SIZE);
      x = atoi((char *)packetBuffer+3); // ##,####,#### 2nd indexed char
      y = atoi((char *)packetBuffer+8); // ##,####,#### 7th indexed char
//      Serial.print("From Team ");
//      Serial.println((char *)packetBuffer);
//      Serial.println(x);
//      Serial.println(y);
      return y;
   }
}

void policeCarPush() {
  static long int ms = millis();
  static uint16_t x1,y1 , x2 , y2 ,x_center , y_center , x_target,y_target;
  double x1_f , y1_f , x2_f , y2_f;
  

  if (millis()-ms>1000/FREQ) {
    ms = millis();
    if (WiFi.status()==WL_CONNECTED)
        neopixelWrite(RGBLED,255,255,255);  // full white
    UdpSend(x_center,y_center);
  }
  
  if (vive1.status() == VIVE_RECEIVING and vive2.status()==VIVE_RECEIVING) {
    x1 = vive1.xCoord();
    y1 = vive1.yCoord();
    x2 = vive2.xCoord();
    y2 = vive2.yCoord();


    x_center= (x1+x2)/2;
    y_center= (y1+y2)/2;
    
    neopixelWrite(RGBLED,0,x1/200,y1/200);  // blue to greenish
  }
  else {
    x1=0;
    y1=0; 
    x2=0;
    y2=0;
    x_center=0;
    y_center=0;
    switch (vive1.sync(5)) {
      break;
      case VIVE_SYNC_ONLY: // missing sweep pulses (signal weak)
        neopixelWrite(RGBLED,64,32,0);  // yellowish
      break;
      default:
      case VIVE_NO_SIGNAL: // nothing detected     
        neopixelWrite(RGBLED,128,0,0);  // red
    }
    switch (vive2.sync(5)) {
        // Handle synchronization status for Vive 2
        case VIVE_SYNC_ONLY:
            // Code to handle synchronization status for Vive 2
            break;
        default:
        case VIVE_NO_SIGNAL:
            // Code to handle no signal for Vive 2
            break;
    }
  }

//    x_center=1000;
//    x1=980;

//    double effortO = calculatePDEffort_Rotation(y_center, y1, prevErrorO, kp, kd, 50, 50, 50, 50);
//    
//    prevErrorO = y_center - y1;
//
//
//      if( prevErrorO<30 and prevErrorO>-30){
//
//        x_target=4400;
//        y_target=3900;
//        moveToTargetCoordinates(x_target, y_target, x_center, y_center);       
//      }

    if (car_y==0){
      car_y = handleUDPServer();
      error = 1000;
      Serial.println("Hello");
//      delay(4000);
      }

//    int car_y = handleUDPServer();
      
    Serial.println(error);

//    x_target=x_center;
//    y_target= car_y;
//    error = moveToTargetCoordinates(x_target, y_target, x_center, y_center);
    
//
    if (flag==0){
       Serial.println("Hello");
       x_target=x_center;
       y_target= car_y -100;
       error = moveToTargetCoordinates(x_target, y_target, x_center, y_center, x1 ,x2);  
      }
    

    if (error<20 and error >-20 and error!=0 ){
      flag=1;
      moveMotors_dircontrol(150, 150, 150, 150, MEC_FORWARD);
//      while (true) {
//         moveMotors_dircontrol(120, 120, 120, 120, MEC_FORWARD);
//        }
      }

    // Add a delay to control loop execution
    delay(100);  // Adjust the delay as needed
}
