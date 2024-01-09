/*HTML interface*/
//#include <WiFi.h>
//#include <WiFiUdp.h>
#include "html510.h"
#include "main_html_code.h"
#include "WiFi.h"
#include "WiFiUdp.h"
#include "vive510.h"

#include <PID_v1.h>

const char* ssid     = "TP-Link_E0C8";
const char* password = "52665134";

//Initialize HTML server
HTML510Server h(80);

/*ESPNow setup*/
#include "ESPnow-sender.h"

bool bPushingPoliceCar = false;
#include "policeCarPush.h"

bool bBeaconTracking = false;
#include "beacon_tracking.h"

/*Wallfollowing*/
#include "Wall_Follow.h"
bool bWallFollowing = false;

/*Beacon tracking*/


/*Pushing police car*/


/*HTML callback functions */
#include "html_cb_fxns.h"


void setup() {
  int i=0;
  // Set up Serial Monitor
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.config(IPAddress(192, 168, 1, STUDENTIP), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  WiFi.begin(ssid, password);

  Serial.printf("team  #%d ", teamNumber); 
  Serial.print("Connecting to ");  Serial.println(ssid);
  while(WiFi.status()!=WL_CONNECTED && i++ < 20){
    delay(500);   Serial.print(".");
  }
  if (i<19) {
    Serial.println("WiFi connected as "); Serial.print(WiFi.localIP());
  } else {
    Serial.printf("Could not connect err: %d ",i); 
  }

    UDPServer.begin(UDPPORT);
  
  vive1.begin();
  vive2.begin();
  Serial.println("  Vive trackers started");

  /*Begin HTML server and assign callback functions*/
   h.begin();
   h.attachHandler("/ ", handleRoot);
   h.attachHandler("/hitFollowWall", handleFollowWall);
   h.attachHandler("/hitGrabBeacon", handleGrabBeacon);
   h.attachHandler("/hitPushPoliceCar", handlePushPoliceCar);
   h.attachHandler("/output", handleOutput);
   h.attachHandler("/vivelocation", handleViveLocationOutput);

  // h.attachHandler("/hitForward", hitForward);
  // h.attachHandler("/hitBackward", hitBackward);
  // h.attachHandler("/hitLeft", hitLeft);
  // h.attachHandler("/hitRight", hitRight);
  // h.attachHandler("/hitClockwise", hitClockwise);
  // h.attachHandler("/hitCounterClockwise", hitCounterClockwise);
  // h.attachHandler("/hitStop", hitStop);

//  /*Initialize ESPNow*/
//  Serial.print("ESPNow Receiving MAC: "); Serial.println(WiFi.macAddress());
//  if(esp_now_init() != ESP_OK)
//  {
//    Serial.println("init failed");
//  }
//  esp_now_register_recv_cb(OnDataRecv);

  //Wall following setup
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

  //beacon tracking setup
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
  // delay(5000);

}


void loop() {
  if (true == bWallFollowing)
  {
    Wall_follow();
  }
  else if(true == bBeaconTracking)
  {
    beacon_tracking();
  }
  else if(true == bPushingPoliceCar)
  {
    policeCarPush();
  }

  //Run HTML server
  h.serve();
  delay(10);
}
