/***************************************************************
* Title: MyRobots Sensor Sample
* Authors: Carlos Asmat
* Date: 24/01/2012
* Licence: GPL v3
* Description: Sends sensor data via serial communication.
* URL: www.myrobots.com
*
* Description:
* To be used with the MyRobots Connect or any otehr serial
* gateway to read and send sensor data to MyRobots.com
*
* This sketch allows to send information from a robot or a sensor
* to the Myrobots.com Server. The information can be sent wirelessly
* via Xbee to a serial gateway.
*
* This sketch requires the EasyTransfer Library available he:
* http://www.billporter.info/easytransfer-arduino-library/
* 
***************************************************************/

#include <EasyTransfer.h>
#define max_feeds 8
#define baud_rate 19200
#define ROBOT_KEY "YOUR KEY HERE"
#define update_delay 15000

//Analogue Sensors
int sensor[] = {A0, A1, A2, A3, A4, A5}; //Analogue sensor pins
String robotKey = ROBOT_KEY;
String status_message = "I am testing a 70 char message, it is fairly long! Yay!!!!!!!!!!!!!!!";

//create object
EasyTransfer ET; 

struct SEND_DATA_STRUCTURE
{
  char command;
  char key[17];
  int data[max_feeds];
  int coordinates[3];
  char status_str[70];
};

//give a name to the group of data
SEND_DATA_STRUCTURE localRobot;

void setup()
{
  Serial.begin(baud_rate);
  //start the library
  ET.begin(details(localRobot), &Serial);
  
  //define robot key
  robotKey.toCharArray(localRobot.key, 17);
}

void loop(){

  updateRobot();
  delay(update_delay);
}

void updateRobot()
{
  int i = 0;
  for (i;i < 6; i++)
  {
    localRobot.data[i] = analogRead(sensor[i]);
  }
  for (i;i < max_feeds; i++)
  {
    localRobot.data[i] = 110;
  }
  localRobot.command = 1;
  localRobot.coordinates[0]=45;
  localRobot.coordinates[1]=78;
  localRobot.coordinates[2]=35;
  status_message.toCharArray(localRobot.status_str, 70);
  ET.sendData();
}
