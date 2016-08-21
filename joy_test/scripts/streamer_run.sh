mjpg_streamer -i "/usr/local/lib/input_uvc.so -y -n -f 12 -r 640x480" -o "/usr/local/lib/output_http.so -w /usr/local/www -p 8080"
