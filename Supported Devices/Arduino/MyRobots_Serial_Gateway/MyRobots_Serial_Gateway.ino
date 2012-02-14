/***************************************************************
* Title: MyRobots Gateway
* Authors: MyRobots.com, based on ThingSpeak Original Sample Code
* Date: 14/12/2010
* Licence: GPL v3
* Description: Ethernet-to-serial communication Gateway.
* URL: http://www.myrobots.com
*
* Description:
* Intended to be used with the an arduiono board supplemented by
* an Etehrnet shield and XBee connectivity.
*
* This sketch allows to send information from a robot or a sensor
* to the Myrobots.com Server. The information can come wirelessly
* via Xbee.
*
* This sketch requires the EasyTransfer Library available he:
* http://www.billporter.info/easytransfer-arduino-library/
* 
***************************************************************/

#include <SPI.h>
#include <Ethernet.h>
#include <EasyTransfer.h>

//Useful constants
#define max_feeds 8  //Number of feeds per robot.
#define max_fails 3

//Communication
#define baud_rate 19200

// Local Network Settings
//See the paper lable for the MAC address
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x42, 0x60 }; // Must be unique on local network

// MyRobots.com Settings
char address[] = "bots.myrobots.com";

// Variable Setup
long lastConnectionTime = 0;
boolean lastConnected = false;
int failedCounter = 0;

//Create data structure for wireless serial transfer
EasyTransfer ET; 

// Initialize Arduino Ethernet Client
EthernetClient client;

struct RECEIVE_DATA_STRUCTURE
{
  char command;
  char key[17];
  int data[max_feeds];
  int coordinates[3];
  char status_str[70];
};

RECEIVE_DATA_STRUCTURE Robot;

void setup()
{
  //Setup serial communication
  Serial.begin(baud_rate);
  
  //Setup Etehrnet
  startEthernet();
  
  ET.begin(details(Robot), &Serial);
  
  pinMode(13, OUTPUT);
  
  delay(100);
}

void loop()
{  
  // Print Update Response to Serial Monitor
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // Disconnect from MyRobots
  if (!client.connected() && lastConnected)
  {
    Serial.println("\n\n...disconnected\n\n");
    client.stop();
  }
  
  if(Serial.available())
  {
      if(ET.receiveData())
      {
        digitalWrite(13, HIGH);
        updateServer();
      }
  }
  
  // Check if Arduino Ethernet needs to be restarted
  if (failedCounter > max_fails ) {startEthernet();}
  
  lastConnected = client.connected();
}

void updateServer()
{
  if (client.connect(address, 80))
  { 
    client.print("GET /update?key=" +String(Robot.key)+"&");
    //Send the sensor data;
    for (int n=1; n < max_feeds+1; n++)
    {
      client.print("field");
      client.print(n);
      client.print("=");
      client.print(String(Robot.data[n-1]));
      client.print("&");
    }
    
    client.print("lat");
    client.print("=");
    client.print(String(Robot.coordinates[0]));
    client.print("&");
    
    client.print("long");
    client.print("=");
    client.print(String(Robot.coordinates[1]));
    client.print("&");
    
    client.print("elevation");
    client.print("=");
    client.print(String(Robot.coordinates[2]));
    client.print("&");
    
    client.print("status");
    client.print("=");
    client.print(String(Robot.status_str));
    
    client.print(" HTTP/1.0\n");
    client.println();
    
    lastConnectionTime = millis();
    
    if (client.connected())
    {
      Serial.println("\n\nConnecting to MyRobots.com...\n\n");
      
      failedCounter = 0;
      digitalWrite(13, HIGH);
    }
    else
    {
      failedCounter++;
      digitalWrite(13, LOW);
      Serial.println("\n\nConnection to MyRobots.com failed ("+String(failedCounter, DEC)+")\n\n");
    }
    
  }
  else
  {
    failedCounter++;
    digitalWrite(13, LOW);
    Serial.println("\n\nConnection to MyRobots.com failed ("+String(failedCounter, DEC)+")\n\n");
    
    lastConnectionTime = millis();
  }
}

void resetEthernetShield()
{
  Serial.println("\n\nResetting Ethernet Connection.\n\n"); 
  
  startEthernet();
}

void startEthernet()
{
  
  client.stop();

  Serial.println("\n\nConnecting Arduino to network...\n\n");
  delay(1000);
  
  // Connect to network and obtain an IP address using DHCP
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("\n\nDHCP Failed, reset Arduino to try again\n\n");
  }
  else
  {
    Serial.println("\n\nArduino connected to network using DHCP\n\n");
  }
  
  delay(1000);
}


