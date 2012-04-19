## By: Carlos Asmat for MyRobots.com
## Creation date: 24/01/2012
##
## Description: Listens to the serial port and posts the received data.
## It uses the Open Serial Protocol: http://www.myrobots.com/wiki/Open_Serial_Protocol

## Import required libraries
import MyRobots, serial, time, urllib, json
from struct import unpack

## Communication parameters
start_byte= ['\x06', '\x85']
read = '\x00' #read command
write = '\x01' #write command
length_table = [1, 2, 17, 2, 2,2,2,2,2,2,2,2, 2,2,2, 70, 1] #data partitionning lengths

robot = MyRobots.Robot('') #define robot object

##################################################################
## Parameters you can edit
##################################################################
times = 10000 ## How many writes to the server
port_name = 'COM21' ## Set it to your prefered port

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
        data_len = port.read()
        raw_data = port.read(ord(data_len)+1)
        checkSum = 0L ^ ord(data_len)

        ## Compute Check Sum
        for c in raw_data[:-1]:
            checkSum = checkSum ^ ord(c)
            
        if (checkSum % 255 != ord(raw_data[-1])):
            print 'Check Sum Failed!'
            break
                
        message = [raw_data[sum(length_table[:i]):sum(length_table[:i])+length_table[i]] for i in range(len(length_table))]

        if (message[0] == write): ## write to the server
            robot.parameters['key'] = message[2].strip('\x00')
            robot.parameters['field1'] = unpack('h', message[3])[0]
            robot.parameters['field2'] = unpack('h', message[4])[0]
            robot.parameters['field3'] = unpack('h', message[5])[0]
            robot.parameters['field4'] = unpack('h', message[6])[0]
            robot.parameters['field5'] = unpack('h', message[7])[0]
            robot.parameters['field6'] = unpack('h', message[8])[0]
            robot.parameters['field7'] = unpack('h', message[9])[0]
            robot.parameters['field8'] = unpack('h', message[10])[0]
            robot.parameters['lat'] = unpack('h', message[11])[0]
            robot.parameters['long'] = unpack('h', message[12])[0]
            robot.parameters['elevation'] = unpack('h', message[13])[0]
            robot.parameters['status'] = message[14].strip('\x00')
            robot.write()
            
        if (message[0] == read): ## read from the server

            ## Create link
            link = 'http://bots.myrobots.com/channels/' + \
                    str(unpack('h', message[1])[0]) + \
                    '/feed/last.json?status=true&location=true&key=' + \
                    message[2].strip('\x00')
                    
            data = json.loads(urllib.urlopen(link).read())
            
            serialMessage = message[1] + message[2]+ \
                            ''.join([data['field' + str(i)][:2] for i in range (1, 9)]) + \
                            data['latitude'][:2] + \
                            data['longitude'][:2] + \
                            data['elevation'][:2] + \
                            data['status'][:70] + u''.join(['\x00' for i in range (71 - len(data['status'][:70]))])

            ## Compute Check Sum
            checkSum = 0L
            for c in serialMessage[2:]:
                checkSum = checkSum ^ ord(c)
            serialMessage = serialMessage
            port.write(0x06)
            port.write(0x85)
            port.white(114)
            port.write(0x70)
            port.write(0x01)
            port.write(serialMessage)
            port.write(checkSum)

port.close()
