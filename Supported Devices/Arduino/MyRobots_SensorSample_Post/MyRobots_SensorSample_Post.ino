/***************************************************************
* Title: MyRobots Sersor Posting Sample
* Authors: Carlos Asmat for MyRobots.com
* Date: 14/12/2010
* Licence: GPL v3
* Description: Post sensor data using serial communication.
* URL: http://www.myrobots.com
*
* Description:
* Intended to be used with an arduiono board along with analogue
* sensors and an XBee module for wireless communication.
*
* This sketch allows to send sensor data to the Myrobots.com Server.
* The communication must go through a Serial-to Etehrnet Gateway
* such as the MyRobots Connect:
* http://www.robotshop.com/ProductInfo.aspx?pc=RB-Myr-01
*
* This sketch requires the EasyTransfer Library available here:
* http://www.billporter.info/easytransfer-arduino-library/
* 
***************************************************************/

#include <EasyTransfer.h>
#define max_feeds 8
#define baud_rate 9600
#define ROBOT_KEY "PASTE YOUR KEY HERE"
#define update_delay 15000

//Analogue Sensors
int sensor[] = {A0, A1, A2, A3, A4, A5}; //Analogue sensor pins
String robotKey = ROBOT_KEY;
String status_message = "I am sending a 70 char message. This is fairly long!!!!!!!!!!!!!!!!!!";

//Create Easy Transfer Object
EasyTransfer ETout; 

struct ROBOT_DATA_STRUCTURE
{
  char command;
  int robotID;
  char key[17];
  char feeds;
  int data[max_feeds];
  int coordinates[3];
  char status_str[70];
};

//give a name to the group of data
ROBOT_DATA_STRUCTURE RobotTX;

void setup()
{
  Serial.begin(baud_rate);
  
  //start the library
  ETout.begin(details(RobotTX), &Serial);
  
  //define robot key
  robotKey.toCharArray(RobotTX.key, 17);
  
  //set the command to write
  RobotTX.command = 1;
  
  delay(30);
}

void loop(){

  updateRobot();
  delay(update_delay);
}

void updateRobot()
{
  int i =0;
  RobotTX.feeds = 0; //initialize feeds 

  //Read the sensors and store them in the robot data array.
  for (i; i < 6; i++)
  {
    RobotTX.data[i] = analogRead(sensor[i]);
    RobotTX.feeds |= 0x01 << i;
  }
  
  //fill the rest of the sensors.
  for (i; i < max_feeds; i++)
  {
    RobotTX.data[i] = analogRead(sensor[0]); //fill the rest of the fields with arbtrary data
    //RobotTX.feeds |= 0x01 << i;  //uncomment this line if the information in the rest of the data array is relevant.
  }
  
  RobotTX.robotID = 462; //this ID is irrelevant in this case but it is good practice to set it.
  
  //Set the coordinates as you desire.
  RobotTX.coordinates[0]=45; 
  RobotTX.coordinates[1]=78;
  RobotTX.coordinates[2]=35;
  
  //Setting the status string
  status_message.toCharArray(RobotTX.status_str, 70);
  
  //Send the data out
  ETout.sendData();
}
