gst-launch-0.10 v4l2src ! image/jpeg,framerate=30/1,width=640,height=480 ! tcpserversink host=192.168.43.188 port=9991
