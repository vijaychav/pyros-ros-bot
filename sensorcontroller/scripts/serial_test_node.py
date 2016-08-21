#!/usr/bin/env python

import sys
import rospy
from sensorcontroller.srv import *

def serial_test_node(mode):
    rospy.wait_for_service('serial_service')
    try:
        serial_service = rospy.ServiceProxy('serial_service', SerialComm)
        resp1 = serial_service(mode)
        return resp1.s_intdata
    except rospy.ServiceException, e:
        print "Service call failed: %s"%e

def usage():
    return "%s [mode]"%sys.argv[0]

if __name__ == "__main__":
    if len(sys.argv) == 2:
        mode = int(sys.argv[1])
    else:
        print usage()
        sys.exit(1)
    print "Requesting %s"%mode
    print "%s = %s"%(mode, serial_test_node(mode))
