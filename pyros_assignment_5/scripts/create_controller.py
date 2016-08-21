#!/usr/bin/env python

from Tkinter import *
import tkMessageBox
import tkSimpleDialog

import struct
import sys, glob # for listing serial ports

import rospy
from pyros_assignment_5.srv import *

send_mode = None

def create_control_service_client(x):
    global send_mode
    try:
        if send_mode == None:
            rospy.wait_for_service('create_control_service')
            send_mode = rospy.ServiceProxy('create_control_service', create_control_service, True)
        resp1 = send_mode(x)
        return resp1.mode
    except rospy.ServiceException, e:
        rospy.wait_for_service('create_control_service')
        try:
            send_mode = rospy.ServiceProxy('create_control_service', create_control_service, True)
            resp1 = send_mode(x)
            return resp1.mode
        except rospy.ServiceException, e:
            print "Service call failed: %s"%e

connection = None

TEXTWIDTH = 40 # window width, in characters
TEXTHEIGHT = 16 # window height, in lines

VELOCITYCHANGE = 200
ROTATIONCHANGE = 300

helpText = """\
PyROS Assignment 5\n\n
Supported Keys:
P\tPassive
S\tSafe
R\tReset
Space\tPlay Music (beep)
Arrows\tMotion
E\tEnd/Stop
Q\tBump and wheeldrop sensor
If nothing happens after you connect, try pressing 'P' and then 'S' to get into safe mode.
"""

class TetheredDriveApp(Tk):
    # static variables for keyboard callback -- I know, this is icky
    callbackKeyUp = False
    callbackKeyDown = False
    callbackKeyLeft = False
    callbackKeyRight = False
    callbackKeyLastDriveCommand = ''
    send_mode

    def __init__(self):
        Tk.__init__(self)
        self.title("PyROS Assignment 5")
        self.option_add('*tearOff', FALSE)

        self.menubar = Menu()
        self.configure(menu=self.menubar)

        createMenu = Menu(self.menubar, tearoff=False)
        self.menubar.add_command(label="Quit", command=self.onQuit)

        self.text = Text(self, height = TEXTHEIGHT, width = TEXTWIDTH, wrap = WORD)
        self.scroll = Scrollbar(self, command=self.text.yview)
        self.text.configure(yscrollcommand=self.scroll.set)
        self.text.pack(side=LEFT, fill=BOTH, expand=True)
        self.scroll.pack(side=RIGHT, fill=Y)

        self.text.insert(END, helpText)

        self.bind("<Key>", self.callbackKey)
        self.bind("<KeyRelease>", self.callbackKey)
#        rospy.wait_for_service('create_control_service')
#        global send_mode
#        try:
#            send_mode = rospy.ServiceProxy('create_control_service', create_control_service,True)
#        except rospy.ServiceException, e:
#            print "Service call failed: %s"%e

    # A handler for keyboard events. Feel free to add more!
    def callbackKey(self, event):
        k = event.keysym.upper()
        motionChange = False
        modeNum = 0
        if event.type == '2': # KeyPress; need to figure out how to get constant
            if k == 'P':   # Passive
                #self.sendCommandASCII('128')
                modeNum = 1
            elif k == 'S': # Safe
                #self.sendCommandASCII('131')
                modeNum = 2
            elif k == 'F': # Full
                #self.sendCommandASCII('132')
                modeNum = 3
            elif k == 'C': # Clean
                #self.sendCommandASCII('135')
                modeNum = 4
            elif k == 'D': # Dock
                #self.sendCommandASCII('143')
                modeNum = 5
            elif k == 'SPACE': # Beep
                #self.sendCommandASCII('140 3 1 64 16 141 3')
                modeNum = 6
            elif k == 'R': # Reset
                #self.sendCommandASCII('7')
                modeNum = 7
            elif k == 'UP':
                #self.callbackKeyUp = True
                #motionChange = True
                modeNum = 8
            elif k == 'DOWN':
                #self.callbackKeyDown = True
                #motionChange = True
                modeNum = 9
            elif k == 'LEFT':
                #self.callbackKeyLeft = True
                #motionChange = True
                modeNum = 10
            elif k == 'RIGHT':
                #self.callbackKeyRight = True
                #motionChange = True
                modeNum = 11
            elif k == 'Q':
                modeNum = 12
            elif k == 'E':
                modeNum = 13
            else:
                print repr(k), "not handled"
        elif event.type == '3': # KeyRelease; need to figure out how to get constant
            modeNum = 0
            # if k == 'UP':
            #     self.callbackKeyUp = False
            #     motionChange = True
            # elif k == 'DOWN':
            #     self.callbackKeyDown = False
            #     motionChange = True
            # elif k == 'LEFT':
            #     self.callbackKeyLeft = False
            #     motionChange = True
            # elif k == 'RIGHT':
            #     self.callbackKeyRight = False
            #     motionChange = True

        create_control_service_client(modeNum)

    def onQuit(self):
        if tkMessageBox.askyesno('Really?', 'Are you sure you want to quit?'):
            global send_mode
            send_mode.close()
            self.destroy()


if __name__ == "__main__":
    app = TetheredDriveApp()
    app.mainloop()
