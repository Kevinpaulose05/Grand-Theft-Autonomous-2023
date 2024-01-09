#include <PID_v1.h>

const int trigPin1 = 21;
const int echoPin1 = 20;
const int trigPin2 = 34;
const int echoPin2 = 33;

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


int rf_PWM = 150;  // Right Front Motor
int lf_PWM = 150; // Left Front Motor
int rr_PWM = 150; // Right Rear Motor
int lr_PWM = 150; // Left Front Motor

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

const double kp = 0.6;
const double kd = 0.01;
int MAX_PWM=100;

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

const int wallDistance = 20;  // Set the desired distance from the wall in centimeters
const double wallFollowSpeed = 0.9;  // Set the desired forward speed
const double wallKP = 0.7;  // Set the proportional constant for PID control
const double wallKD = 0.01;  // Set the derivative constant for PID control

double prevWallError = 0.0;
double integralWallError = 0.0;

//
//void followWall(double targetDistance, int sensorDistance) {
//    double wallError = targetDistance - sensorDistance;
//
//    // PID control for wall following
//    double wallEffort = wallKP * wallError + wallKD * (wallError - prevWallError);
//
//    // Calculate mecanum wheel speeds based on wall following effort
//    int fl_pwm, fr_pwm, rl_pwm, rr_pwm;
//    calculateMecanumWheelSpeeds(wallEffort, wallFollowSpeed, fl_pwm, fr_pwm, rl_pwm, rr_pwm);
//
//    // Apply PWM values to move the robot
//    moveMotors(fr_pwm, fl_pwm, rr_pwm, rl_pwm, MEC_FORWARD);
//
//    // Update previous error for the next iteration
//    prevWallError = wallError;
//}

void followWall(double minDistance, double maxDistance, int sensorDistance) {
    if (sensorDistance >= minDistance && sensorDistance <= maxDistance) {
        // Robot is within the desired wall distance range
        int fl_pwm, fr_pwm, rl_pwm, rr_pwm;
        calculateMecanumWheelSpeeds(0, wallFollowSpeed, fl_pwm, fr_pwm, rl_pwm, rr_pwm);
        moveMotors(fr_pwm, fl_pwm, rr_pwm, rl_pwm, MEC_FORWARD);
    } else {
        // Robot is outside the desired wall distance range
        double wallError = (minDistance + maxDistance) / 2.0 - sensorDistance;

        // PID control for wall following
        double wallEffort = wallKP * wallError + wallKD * (wallError - prevWallError);

        // Calculate mecanum wheel speeds based on wall following effort
        int fl_pwm, fr_pwm, rl_pwm, rr_pwm;
        calculateMecanumWheelSpeeds(wallEffort, wallFollowSpeed, fl_pwm, fr_pwm, rl_pwm, rr_pwm);

        // Apply PWM values to move the robot
        moveMotors(fr_pwm, fl_pwm, rr_pwm, rl_pwm, MEC_FORWARD);

        // Update previous error for the next iteration
        prevWallError = wallError;
    }
}


//double calculatePDEffort(double target, double current, double prevError, double kp, double kd) {
//    double error = target - current;
//    double derivative = (error - prevError);
//    double effort = kp * error + kd * derivative;
//    return effort;
//}
//
//double calculatePDEffort_Rotation(double target, double current, double prevError, double kp, double kd, int fr_pwm, int fl_pwm, int rr_pwm, int rl_pwm) {
//    double error = target - current;
//    double derivative = (prevError - error);  // Adjusted the derivative calculation
//    double effort = kp * error + kd * derivative;
//
//    // Determine motor direction based on the sign of the error
//    if (error > 30) {
//        // Clockwise rotation
//        moveMotors_dircontrol(fr_pwm, fl_pwm, rr_pwm, rl_pwm, MEC_ROTATE_CLOCKWISE);
//    } 
//    else if(error < -30) {
//        // Anticlockwise rotation
//        moveMotors_dircontrol(fr_pwm, fl_pwm, rr_pwm, rl_pwm, MEC_ROTATE_COUNTERCLOCKWISE);
//    }
//    
//    
//    return effort;
//}

void calculateMecanumWheelSpeeds(double xEffort, double yEffort, int& fl_pwm, int& fr_pwm, int& rl_pwm, int& rr_pwm) {
    // Calculate individual wheel speeds based on control efforts
    Serial.println(yEffort);
    Serial.println(xEffort);
    double front_left = yEffort - xEffort;
    double front_right = yEffort + xEffort;
    double rear_left = yEffort + xEffort;
    double rear_right = yEffort - xEffort;

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


//void moveToTargetCoordinates(double targetX, double targetY, double currentX, double currentY) {
//    double effortX = calculatePDEffort(targetX, currentX, prevErrorX, kp, kd);
//    double effortY = calculatePDEffort(targetY, currentY, prevErrorY, kp, kd);
//
//    // Calculate mecanum wheel speeds based on control efforts
//    int fl_pwm, fr_pwm, rl_pwm, rr_pwm;
//    calculateMecanumWheelSpeeds(effortX, effortY, fl_pwm, fr_pwm, rl_pwm, rr_pwm);
//
//    // Apply PWM values to move the robot
//    moveMotors(fr_pwm, fl_pwm, rr_pwm, rl_pwm, MEC_FORWARD);
//
//    // Update previous errors for the next iteration
//    prevErrorX = targetX - currentX;
//    prevErrorY = targetY - currentY;
////    Serial.println(prevErrorY);
//}


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
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

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

  
}

void loop() {
  
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
    long duration1, distance1;
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);
    duration1 = pulseIn(echoPin1, HIGH);
    distance1 = (duration1 / 2) / 29.1;  // Calculate distance in centimeters
  
    // Measure distance from the second ultrasonic sensor
    long duration2, distance2;
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    duration2 = pulseIn(echoPin2, HIGH);
    distance2 = (duration2 / 2) / 29.1;  // Calculate distance in centimeters
  
    // Print the distances to the serial monitor
    Serial.print("Distance Sensor 1: ");
    Serial.print(distance1);
    Serial.println(" cm");
  
    Serial.print("Distance Sensor 2: ");
    Serial.print(distance2);
    Serial.println(" cm");

    followWall(5, 15, distance2);  // Adjust the minimum and maximum distance as needed
    
    delay(10);  // Adjust the delay as needed
//
    if (distance1 <= 15){

      moveMotors_dircontrol(50, 50, 50, 50, MEC_ROTATE_COUNTERCLOCKWISE);
      delay(500);
      }

    

//    x_target=3300;
//    y_target=3900;
//    moveToTargetCoordinates(x_target, y_target, x_center, y_center);   
//
//    // Add a delay to control loop execution
//    handleUDPServer();
//    delay(40);  // Adjust the delay as needed
}
