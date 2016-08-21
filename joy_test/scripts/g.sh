TARGET=192.168.43.188
GST="gst-launch-1.0 -vv"
SRC=v4l2src
FIL1="video/x-raw,width=320,height=240,framerate=30/1"
ENC="rtpvrawpay"
DST="udpsink host=$TARGET port=13375"
PIPELINE="$SRC ! $FIL1 ! $ENC ! $DST"
$GST $PIPELINE
