## By: Carlos Asmat for MyRobots.com
## Creation date: 24/01/2012
##
## Description: Defines a Robot object to be used with MyRobots.com

## Import required libraries
import httplib, urllib

class Robot:
    """Robot Class defines the parameters of a robot 
and MyRobots.com sync function"""
    # Latest robot data is stored in parameters dictionary
    parameters = {}

    def __init__(self, key):
        """Initialises parameters dictionary and sets teh robot key."""
        self.parameters = {'field1': 0, 'field2': 0, 'field3': 0, 'field4': 0,
                  'field5': 0, 'field6': 0, 'field7': 0, 'field8': 0,
                  'key':'000000000000000', 'status':'operational',
                  'lat':0, 'long':0, 'elevation':0, 'location':'RobotLand'}
        self.parameters['key'] = key
    
    def write(self):
        """Syncs your robot information and prints the server response."""
        params = urllib.urlencode(self.parameters)
        headers = {"Content-type": "application/x-www-form-urlencoded","Accept": "text/plain"}
        conn = httplib.HTTPConnection("bots.myrobots.com")
        conn.request("POST", "/update", params, headers)
        response = conn.getresponse()
        print response.status, response.reason
        data = response.read()
        conn.close()
