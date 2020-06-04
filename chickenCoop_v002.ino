#include "RTClib.h"
#include <Time.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
//#include <TimeLord.h>

RTC_DS3231 rtc;

int const pinDoorSensorPower = 9;
int const pinDoorSensor1 = 7;
int const pinDoorSensor2 = 8;
int const pinLedDoorOpen = 3;
int const pinLedDoorClosed = 2;

int const motorEnablePin = 6;
int const motorPin1 = 4;
int const motorPin2 = 5;
int const motorSwitchPin1 = 10;
int const motorSwitchPin2 = 11;

int doorSensor1Value;
int doorSensor2Value;
int motorSwitchPin1Value;
int motorSwitchPin2Value;

bool doorOpen = false;
bool doorClosed = false;

bool doOpenDoor = false;
bool doCloseDoor = false;

bool manualDrive1 = false;
bool manualDrive2 = false;

uint32_t syncProvider()//function which sets up the RTC as the source of external time
{
  return rtc.now().unixtime();
}

void setup() {
  pinMode(pinDoorSensorPower, OUTPUT);
  digitalWrite(pinDoorSensorPower, HIGH); // door sensors activated
 
  pinMode(pinDoorSensor1, INPUT);
  pinMode(pinDoorSensor2, INPUT);
  pinMode(pinLedDoorOpen, OUTPUT);
  pinMode(pinLedDoorClosed, OUTPUT);
 
  digitalWrite(pinLedDoorOpen, LOW);
  digitalWrite(pinLedDoorClosed, LOW);

  pinMode(motorEnablePin, OUTPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorSwitchPin1, INPUT);
  pinMode(motorSwitchPin2, INPUT);
  
  Serial.begin(9600);

  Alarm.delay(3000); // wait for console opening

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  // uncomment the line if resetting the time
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // SYNC the RTC and systime at boot
  setSyncProvider(syncProvider);   // the function to get the time from the RTC
  
  if(timeStatus()!= timeSet) 
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");  
  setSyncInterval(600); // set the number of seconds between re-sync of time

  Alarm.alarmOnce(22, 14, 00,  letsOpenTheDoor);
  Alarm.alarmOnce(22, 15, 00, letsCloseTheDoor);

  printTime();
  digitalWrite(motorEnablePin, HIGH);

}


void loop() {
  
  // read sensor values
  doorSensor1Value = digitalRead(pinDoorSensor1);    
  doorSensor2Value = digitalRead(pinDoorSensor2);

  // set door status
  if(doorSensor1Value == HIGH){
      doorOpen = true;
  } 
  if(doorSensor1Value == LOW){
     doorOpen = false;
  }

  if(doorSensor2Value == HIGH){
    doorClosed = true;
  }
  if(doorSensor2Value == LOW){
     doorClosed = false;
  }

  // read switch values

  motorSwitchPin1Value = digitalRead(motorSwitchPin1);
  motorSwitchPin2Value = digitalRead(motorSwitchPin2);
  if(motorSwitchPin1Value == HIGH){
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    manualDrive1 = true;
     Serial.println("manual driving motor direction 1");
  }
  if(motorSwitchPin1Value == LOW && manualDrive1){
    stopMotor();
    manualDrive1 = false;  
    Serial.println("manual driving motor: stop");
  }
  
  if(motorSwitchPin2Value == HIGH){
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    manualDrive2 = true;
    Serial.println("manual driving motor direction 2");
  }
  if(motorSwitchPin2Value == LOW && manualDrive2){
    stopMotor();
    manualDrive2 = false;
    Serial.println("manual driving motor: stop");
    }
  
  // printSerialValues();
 //  printTime();

  // open door
  if(doOpenDoor && ! doorOpen){
//    digitalWrite(motorEnablePin, HIGH);
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW); 
    Serial.println("trying to open door");
   } 
   else {
    doOpenDoor = false;
    stopMotor();
   }
   
   // open door
  if(doCloseDoor && ! doorClosed){
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH); 
    Serial.println("trying to close the door");
   } 
   else {
    doCloseDoor = false;
    stopMotor();
   }
    
   // send signals to LED lights
   showDoorStatusWithLEDs();
    
//  Alarm.delay(10);
}


void stopMotor(){
   digitalWrite(motorPin1, LOW);
   digitalWrite(motorPin2, LOW);
 //  digitalWrite(motorEnablePin, LOW);
}

void showDoorStatusWithLEDs(){
   if(doorOpen){
    digitalWrite(pinLedDoorOpen, HIGH);
    } else {
     digitalWrite(pinLedDoorOpen, LOW);
    }
   if(doorClosed){
    digitalWrite(pinLedDoorClosed, HIGH);
   }else{
    digitalWrite(pinLedDoorClosed, LOW);
    }
}

void letsOpenTheDoor(){
  Serial.println("Lets Open The Door");
  printTime();
  printSerialValues();

  doOpenDoor = true;
  }

void letsCloseTheDoor(){
  Serial.println("Lets close The Door");
  printTime();
  printSerialValues();

  doCloseDoor = true;
  }

void printSerialValues(){
  //Serial.print("     door sensor value 1 : ");
  //Serial.print(doorSensor1Value);
  //Serial.print("     door sensor value 2 : ");
  //Serial.println(doorSensor2Value);

  Serial.print("door switch 1 value : ");
  Serial.print(motorSwitchPin1Value);
  Serial.print("door switch 2 value : ");
  Serial.println(motorSwitchPin2Value);
  }

void printTime(){
   DateTime now = rtc.now();
   
   Serial.print(now.year(), DEC);
   Serial.print('/');
   Serial.print(now.month(), DEC);
   Serial.print('/');
   Serial.print(now.day(), DEC);
   Serial.print(' ');
   Serial.print(now.hour(), DEC);
   Serial.print(':');
   Serial.print(now.minute(), DEC);
   Serial.print(':');
   Serial.println(now.second(), DEC);
 }
