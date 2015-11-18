#include <AFMotor.h>
#include <Usb.h> 
#include <AndroidAccessory.h> 
#include <NewPing.h>
#define SONAR_NUM     4 // Number or sensors.
#define MAX_DISTANCE 200 // Max distance in cm.
#define PING_INTERVAL 33 // Milliseconds between pings.
unsigned long pingTimer[SONAR_NUM]; // When each pings.
unsigned int cm[SONAR_NUM]; // Store ping distances.
uint8_t currentSensor = 0; // Which sensor is active.
NewPing sonar[SONAR_NUM] = { // Sensor object array.
  NewPing(41, 40, MAX_DISTANCE),
  NewPing(43, 42, MAX_DISTANCE),
  NewPing(45, 44, MAX_DISTANCE),
  NewPing(47, 46, MAX_DISTANCE)};
AndroidAccessory acc("syedtalha", "orion", "Description", 
"1", "URI", "Serial"); 
AF_DCMotor motorRearLeft(1);
AF_DCMotor motorRearRight(2);
AF_DCMotor motorFrontLeft(3);
AF_DCMotor motorFrontRight(4);
byte rcvmsg[4];
byte sntmsg[4];





//########################################################################################################################################################################
void setup() { 
  Serial.begin(115200); 
  pingTimer[0] = millis() + 75; // First ping start in ms.
  for (uint8_t i = 1; i < SONAR_NUM; i++)  {
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
  }
  acc.powerOn();
  motorFrontRight.run(FORWARD);   
  motorFrontLeft.run(FORWARD);   
  motorRearRight.run(FORWARD);   
  motorRearLeft.run(FORWARD);   

} 
//########################################################################################################################################################################








//########################################################################################################################################################################
void loop() {
  if (acc.isConnected()) {
    int len = acc.read(rcvmsg, sizeof(rcvmsg), 1); 
    if (len > 0)  {
      updateMotorSpeeds();
      showMotorDataOnSerial(rcvmsg);
      sensorLoop();
    }  else {
          accessorySendingWrongData();

    }
  }  
  else   {
    accessoryNotConnected();
  }







}
//########################################################################################################################################################################







//FUNCTIONS


void updateMotorSpeeds()  {
  if (bitRead(rcvmsg[0],0)==1)  {
    motorFrontRight.run(FORWARD); 
    motorFrontRight.setSpeed(rcvmsg[1]); 
  } 
  else if (bitRead(rcvmsg[0],0)==0) {
    motorFrontRight.run(BACKWARD);
    motorFrontRight.setSpeed(rcvmsg[1]); 
  }
  if (bitRead(rcvmsg[0],1)==1)  {
    motorFrontLeft.run(FORWARD);
    motorFrontLeft.setSpeed(rcvmsg[2]); 
  } 
  else if ((bitRead(rcvmsg[0],1)==0)) {
    motorFrontLeft.run(BACKWARD);
    motorFrontLeft.setSpeed(rcvmsg[2]); 
  }
  if (bitRead(rcvmsg[0],2)==1)  {
    motorRearRight.run(FORWARD);
    motorRearRight.setSpeed(rcvmsg[3]); 
  } 
  else if ((bitRead(rcvmsg[0],2)==0)) {
    motorRearRight.run(BACKWARD);
    motorRearRight.setSpeed(rcvmsg[3]); 
  }
  if (bitRead(rcvmsg[0],3)==1)  {
    motorRearLeft.run(FORWARD);
    motorRearLeft.setSpeed(rcvmsg[4]); 
  } 
  else if ((bitRead(rcvmsg[0],3)==0)){
    motorRearLeft.run(BACKWARD);
    motorRearLeft.setSpeed(rcvmsg[4]); 
  }
}




void showMotorDataOnSerial(byte rcvmsg[])  {
  Serial.print((int)rcvmsg[1]); 
  Serial.print(" ");
  Serial.print((int)rcvmsg[2]); 
  Serial.print(" ");
  Serial.print((int)rcvmsg[3]); 
  Serial.print(" ");
  Serial.print((int)rcvmsg[4]);
  Serial.print(" Direction is "); 
  Serial.print(bitRead(rcvmsg[0],0)); 
  Serial.print(bitRead(rcvmsg[0],1)); 
  Serial.print(bitRead(rcvmsg[0],2)); 
  Serial.print(bitRead(rcvmsg[0],3)); 
  Serial.println();
}







void echoCheck() { // If ping echo, set distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}




void oneSensorCycle() { // Do something with the results.
  if (acc.isConnected()) { 
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
      sntmsg[i] = cm[i] ; 
      //Serial.print(cm[i]);
      //Serial.print("      ");
    }
    acc.write(sntmsg, 4);
   // Serial.println();
  }
}


void sensorLoop()  {
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    if (millis() >= pingTimer[i]) {
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;
      if (i == 0 && currentSensor == SONAR_NUM - 1)
        oneSensorCycle(); // Do something with results.
      sonar[currentSensor].timer_stop();
      currentSensor = i;
      cm[currentSensor] = 0;
      sonar[currentSensor].ping_timer(echoCheck);
    }
  }
}


void accessoryNotConnected()  {
  emergencyStopThrottles();
}
void accessorySendingWrongData()  {
  accessoryNotConnected();
}

void emergencyStopThrottles()  {
  motorFrontRight.setSpeed(0);   
  motorFrontLeft.setSpeed(0);     
  motorRearRight.setSpeed(0);   
  motorRearLeft.setSpeed(0);   
}

