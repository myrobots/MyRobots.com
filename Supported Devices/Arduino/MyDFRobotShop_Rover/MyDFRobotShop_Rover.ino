/***************************************************************
* Title: MyDFRobotShop Rover Control sample code
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
#define ROBOT_KEY "PASTE YOUR KEY HERE"
#define ROBOT_ID 688 //set your robot ID
#define update_delay_max 6000 // max update rate limit
#define update_delay_min 5000 // min update rate limit

//Analogue Sensors
int sensor[] = {A0, A1, A2, A3, A4, A5}; //Analogue sensor pins
String robotKey = ROBOT_KEY;
String status_message = "I am your DFRobotShop Rover. What is your next command?";
long update_delay=15000;


//create object
EasyTransfer ETin; 
EasyTransfer ETout; 

const int ledPin =  13;
int ledState = LOW;             // ledState used to set the LED
unsigned long lastBlink = 0;
unsigned long lastUpdate = 0;  
int blinkPeriod = 1000;

//Define Motors Pins
const int E1 = 6; //M1 Speed Control
const int E2 = 5; //M2 Speed Control
const int M1 = 8; //M1 Direction Control
const int M2 = 7; //M2 Direction Control

//Define Motor Speeds.
int pwmSpeed = 255; //255 is maximum speed

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
  
  for(int i=5; i<=8; i++)
    pinMode(i, OUTPUT);
  
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
    update_delay = random(update_delay_min, update_delay_max); //set update rate to a random value to avoid collitions 

    
  }
  
  //Check the received data and update the blinking rate
  if(ETin.receiveData()){
    if(RobotRX.robotID == ROBOT_ID) //check the message is adressed to you
    {
      blinkPeriod = RobotRX.data[0];
      pwmSpeed = RobotRX.data[1];
      
      switch(RobotRX.data[2]) // Perform an action depending on the command
      {
        case 1://Move Forward
          forward (pwmSpeed, pwmSpeed);
        break;
        
        case 2://Move Backwards
          reverse (pwmSpeed,pwmSpeed);
        break;
        
        case 4://Turn Left
          left (pwmSpeed,pwmSpeed);
        break;
        
        case 3://Turn Right
          right (pwmSpeed,pwmSpeed);
        break;
        
        default:
          stop();
        break;
      }
    }
  }
}

// Motor Driving Fucntions
void stop(void) //Stop
{
  digitalWrite(E1,LOW);
  digitalWrite(E2,LOW);
}

void forward(char a,char b)
{
  analogWrite (E1,a);
  digitalWrite(M1,LOW);
  analogWrite (E2,b);
  digitalWrite(M2,LOW);
}
void reverse (char a,char b)
{
  analogWrite (E1,a);
  digitalWrite(M1,HIGH);
  analogWrite (E2,b);
  digitalWrite(M2,HIGH);
}

void left (char a,char b)
{
  analogWrite (E1,a);
  digitalWrite(M1,HIGH);
  analogWrite (E2,b);
  digitalWrite(M2,LOW);
}

void right (char a,char b)
{
  analogWrite (E1,a);
  digitalWrite(M1,LOW);
  analogWrite (E2,b);
  digitalWrite(M2,HIGH);
}
