## By: Carlos Asmat for MyRobots.com
## Creation date: 24/01/2012
##
## Description: Listens to the serial port and posts the received data.
## It uses the Open Serial Protocol: http://www.myrobots.com/wiki/Open_Serial_Protocol

## Import required libraries
import MyRobots, serial, time
from struct import unpack

## Communication parameters
start_byte= ['\x06', '\x85']
read = '\x00' #read command
write = '\x01' #write command
length_table = [1, 17, 2,2,2,2,2,2,2,2, 2,2,2, 70] #data partitionning lengths

robot = MyRobots.Robot('') #define robot object

##################################################################
## Parameters you can edit
##################################################################
times = 10000 ## How many writes to the server
port_name = 'COM3' ## Set it to your prefered port

##################################################################
## Reading serial Robot data and sending it to Myrobots.com
##################################################################
#open serial port
port = serial.Serial(port_name, 19200, timeout=1)

##for i in range(times):
n=0
while(True):
    print n
    n=n+1
    while(port.read() != start_byte[0]):
        time.sleep (0.1)

    if (port.read() == start_byte[1]):
        time.sleep (0.05)
        message = port.read(ord(port.read()))
        message = [message[sum(length_table[:i]):sum(length_table[:i])+length_table[i]] for i in range(len(length_table))]
        if (message[0] == write): ## write to the server
            robot.parameters['key'] = message[1].strip('\x00')
            robot.parameters['field1'] = unpack('h', message[2])[0]
            robot.parameters['field2'] = unpack('h', message[3])[0]
            robot.parameters['field3'] = unpack('h', message[4])[0]
            robot.parameters['field4'] = unpack('h', message[5])[0]
            robot.parameters['field5'] = unpack('h', message[6])[0]
            robot.parameters['field6'] = unpack('h', message[7])[0]
            robot.parameters['field7'] = unpack('h', message[8])[0]
            robot.parameters['field8'] = unpack('h', message[9])[0]
            robot.parameters['lat'] = message[10]
            robot.parameters['long'] = message[11]
            robot.parameters['elevation'] = message[12]
            robot.parameters['status'] = message[13].strip('\x00')
            robot.write()
        if (message[0] == read): ## read from the server
            pass ## not implemented yet.

port.close()
