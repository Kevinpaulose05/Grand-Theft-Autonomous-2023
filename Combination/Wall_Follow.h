
const int trigPin1 = 21;
const int echoPin1 = 20;
const int trigPin2 = 34;
const int echoPin2 = 33;


const int wallDistance = 20;  // Set the desired distance from the wall in centimeters
const double wallFollowSpeed = 0.9;  // Set the desired forward speed
const double wallKP = 0.7;  // Set the proportional constant for PID control
const double wallKD = 0.01;  // Set the derivative constant for PID control

double prevWallError = 0.0;
double integralWallError = 0.0;

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
        calculateMecanumWheelSpeeds(wallEffort, wallFollowSpeed, fl_pwm, fr_pwm, rl_pwm, rr_pwm , x1, x2);

        // Apply PWM values to move the robot
        moveMotors(fr_pwm, fl_pwm, rr_pwm, rl_pwm, MEC_FORWARD);

        // Update previous error for the next iteration
        prevWallError = wallError;
    }
}

void Wall_follow() {
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
    
}
