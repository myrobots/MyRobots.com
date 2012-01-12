## By: Carlos Asmat for MyRobots.com
## Creation date: 29/12/2011
##
## Description: Simple script to define a Robot object,
## instantiate many of them, fill them with random data
## and syncronysze them with MyRobots.com

## Import required libraries
import httplib, urllib, random, time

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
    
    def sync(self):
        """Syncs your robot information and prints teh server response."""
        params = urllib.urlencode(self.parameters)
        headers = {"Content-type": "application/x-www-form-urlencoded","Accept": "text/plain"}
        conn = httplib.HTTPConnection("bots.myrobots.com")
        conn.request("POST", "/update", params, headers)
        response = conn.getresponse()
        print response.status, response.reason
        data = response.read()
        conn.close()

##################################################################
## Parameters you can edit
##################################################################

## List of status messages, define your own.
## They will be randomly selected and sent to MyRobots.com
status_msg = ['Happy',
              'Sad',
              'Cleaning',
              'Charging',
              'Stuck',
              'Confused',
              "Here I am, brain the size of a planet, \
and they ask me to take you to the bridge. \
Call that job satisfaction, 'cause I don't.",
              "I think you ought to know I'm feeling very depressed.",
              "I've calculated your chance of survival, \
but I don't think you'll like it. ",
              "Hasta la vista baby.",
              "EXTREMINATE!",
              "DELETE!",
              "MyRobots.com is Live!! Finaly robots can ge a social life",
              "Come with me if you want to live"]

## Add the keys for your robots.
## Obtain the keys at MyRobots.com
keys = ['YOUR_KEY_HERE',
        'YOUR_KEY_HERE',
        'YOUR_KEY_HERE',
        'YOUR_KEY_HERE']

## Set the location
dec_lat = 45.69645
dec_long = -73.92606
location = 'Home sweet home'

## Set the time between updates
period = 60;

## Set the number of times you want your robots to be updated
times = 100


##################################################################
## Sending Robot data to Myrobots.com
##################################################################

## Period shuld be > 30s.
if period < 30:
    period = 30

## Instantiate a robot for every key in keys.
robots = [Robot(k) for k in keys]


## Make several updates (as many as specified in times).
for i in range(times):
    print i ## Print itteration number for debugging purposes.
    for r in robots:
        ## Fill parameters with random data
        r.parameters['field1'] = random.randint(10, 40)
        r.parameters['field2'] = random.random()
        r.parameters['field3'] = random.randint(-30, 30)
        r.parameters['field4'] = random.randint(0, 1)
        r.parameters['field5'] = random.randint(0, 1)
        r.parameters['field6'] = random.randint(10, 40)
        r.parameters['field7'] = random.randint(10, 40)
        r.parameters['field8'] = random.randint(10, 40)
        r.parameters['status'] = random.choice(status_msg)
        r.parameters['lat'] = dec_lat
        r.parameters['long'] = dec_long
        r.parameters['elevation'] = random.randint(30, 50)
        r.parameters['location'] = location

        ## Print the robot key for debugging purposes.
        print r.parameters['key']

        ## send robot data to MyRobots.com
        r.sync()

    print '\n\n' ## Print white space number for debugging purposes.
    time.sleep(period)
