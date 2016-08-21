#!/usr/bin/env python

from sensorcontroller.srv import *
import rospy
import serial
import Adafruit_BBIO.UART as UART
import os
import sys
from time import sleep

#os.system("echo BB-UART1 > /sys/devices/platform/bone_capemgr/slots")

#UART.setup("UART1")

ser = serial.Serial(port = '/dev/ttyO1',baudrate = 9600)

if ser < 0:
   print 'Server UART Creation Failed!!'
else:
   print 'Server UART Created, fd = ' + str(ser)

ser.close()
ser.open()
if ser.isOpen():
   print 'Server Serial Ready'
else:
   print 'Server Serial Closed??'

ser.close()

def callback(req):
   option = req.mode

#1 is distance threshold mode
#2 is gradient threshold mode
#3 is ultra sonic distance mode
#4 is all clear check mode
#5 is motor only mode

   if(option == 3):
       ser.open()
       ser.flush()
       ser.write("U")
       #ser.close()
       print 'Sent ultrasonic data request to LCU\n'
       #sleep(.1)
       #ser.open()
       s_data = ser.readline()
       print "Distance is %s cm\n" % s_data
       ser.close()

   if(option == 1):
       ser.open()
       ser.flush()
       ser.write("T")
       #ser.close()
       print 'Set LCU in distance threshold mode\n'
       #sleep(.1)
       #ser.open()
       s_data = ser.readline()
       print "Distance is %s cm\n" % s_data
       ser.close()


   if(option == 5):
       ser.open()
       ser.flush()
       ser.write("M020\n")
       ser.close()
       print 'Sent M020 data to LCU\n'


   if(option == 2):
       ser.open()
       ser.flush()
       ser.write("G")
       #ser.close()
       print 'Set LCU in gradient threshold mode\n'
       #sleep(.1)
       #ser.open()
       s_data = ser.readline()
       print "Distance is %s cm\n" % s_data
       ser.close()

   if(option == 4):
       ser.open()
       ser.flush()
       ser.write("C")
       #ser.close()
       print 'Set LCU in all clear check mode\n'
       #sleep(.1)
       #ser.open()
       s_data = ser.readline()
       print "Output is %s \n" % s_data
       ser.close()

   s_intdata = int(float(s_data))
   return SerialCommResponse(s_intdata)

def create_serial():
   rospy.init_node('create_serial')
   s = rospy.Service('serial_service', SerialComm, callback)
   print "Ready to receive mode.\n"
   rospy.spin()

if __name__ == "__main__":
   create_serial()
