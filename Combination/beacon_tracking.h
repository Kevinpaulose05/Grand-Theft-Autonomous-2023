
#define Bleft_H 2
#define Bleft_L 1
#define Bright_H 38
#define Bright_L 39

//
//bool flag_FR=0;
//bool flag_FL=0;
//bool flag_BR=0;
//bool flag_BL=0;


static bool highDetected = false;

void beacon_tracking() {
  
  Serial.println("Beacon");
  while (!highDetected) {
     moveMotors_dircontrol(100, 100, 100, 100, MEC_RIGHT);
     if (digitalRead(Bleft_L) == HIGH && digitalRead(Bright_L) == HIGH) {
    // Both sensors are high, move forward
      highDetected = true;
      break;
     }
     delay(2500);
     moveMotors_dircontrol(100, 100, 100, 100, MEC_FORWARD);
     if (digitalRead(Bleft_H) == HIGH && digitalRead(Bright_H) == HIGH) {
    // Both sensors are high, move forward
      highDetected = true;
      break;
     }
     delay(1000);
     moveMotors_dircontrol(100, 100, 100, 100, MEC_LEFT);
     if (digitalRead(Bleft_H) == HIGH && digitalRead(Bright_H) == HIGH) {
    // Both sensors are high, move forward
      highDetected = true;
      break;
     }
     delay(2500);
     moveMotors_dircontrol(100, 100, 100, 100, MEC_FORWARD);
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
