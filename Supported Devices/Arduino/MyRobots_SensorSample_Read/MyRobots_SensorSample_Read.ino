/***************************************************************
* Title: MyRobots Sersor Posting Sample
* Authors: Carlos Asmat for MyRobots.com
* Date: 14/12/2010
* Licence: GPL v3
* Description: Read a MyRobots.com Feed and Blink a light accordingly
* URL: http://www.myrobots.com
*
* Description:
* Intended to be used with an arduiono board along with an XBee
* module for wireless communication.
*
* This sketch allows to retrieve data from the Myrobots.com Server.
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
#define ROBOT_KEY "PASTE YOUR ROBOT KEY"
#define ROBOT_ID 356 //set your robot ID
#define update_delay 15000

//Analogue Sensors
int sensor[] = {A0, A1, A2, A3, A4, A5}; //Analogue sensor pins
String robotKey = ROBOT_KEY;
String status_message = "I am trying to read a feed. What will I get??????????????????????????";

//create object
EasyTransfer ETin; 
EasyTransfer ETout; 

const int ledPin =  13;
int ledState = LOW;             // ledState used to set the LED
unsigned long lastBlink = 0;
unsigned long lastUpdate = 0;  
int blinkPeriod = 1000;

struct ROBOT_DATA_STRUCTURE
{
  char command;
  int robotID;
  char key[17];
  int feeds;
  int data[max_feeds];
  int coordinates[3];
  char status_str[70];
};

//give a name to the group of data
ROBOT_DATA_STRUCTURE RobotTX;
ROBOT_DATA_STRUCTURE RobotRX;

void setup()
{
  Serial.begin(baud_rate);
  //start the library
  ETin.begin(details(RobotRX), &Serial);
  ETout.begin(details(RobotTX), &Serial);
  pinMode(ledPin, OUTPUT);   
  
  robotKey.toCharArray(RobotTX.key, 17); //Set robot key
  status_message.toCharArray(RobotTX.status_str, 70); //Set robot status for easier debugging
  RobotTX.robotID = ROBOT_ID; // Set robot ID
  RobotTX.command = 0; // set command to READ
  
  delay(30);
}

void loop(){

  unsigned long currentMillis = millis(); //keep track of time
 
 //Blink LED in a non-blocking way
  if(currentMillis - lastBlink > blinkPeriod) {
    // save the last time you blinked the LED 
    lastBlink = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
  
   //Retrieve MyRobots Feed Data Periodically
  if(currentMillis - lastUpdate > update_delay){
    ETout.sendData(); //Send Read command
    lastUpdate = currentMillis;
  }
  
  //Check the received data and update the blinking rate
  if(ETin.receiveData()){
    if(RobotRX.robotID == ROBOT_ID) //check the message is adressed to you
      blinkPeriod = RobotRX.data[0];
  }
}
