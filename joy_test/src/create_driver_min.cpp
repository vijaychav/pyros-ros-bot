#include <iostream>
#include <cstdio>
#include <ros/ros.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <malloc.h>
#include <joy_test/JoyIn.h>

 // #include <serial/serial.h>

#define ROTATIONCHANGE 300
#define VELOCITYCHANGE 200

int prev_cmd = 0;
int curr_cmd = 0;
int fd = -1;

using namespace std;

int open_port(void);
int configure_port(int fd);
bool control_callback(joy_test::JoyIn::Request  &req, joy_test::JoyIn::Response &res);


int main(int argc, char *argv[])
{
	ros::init(argc, argv, "create_driver_final");
	ros::NodeHandle n;

	ros::ServiceServer service = n.advertiseService("joy_in",control_callback);

	fd = open_port();
	configure_port(fd);

	ros::spin();

	close(fd);
	return 0;
}

int open_port(void)
{
	int fd;
	fd = open("/dev/ttyUSB0", O_RDWR);

	if(fd == -1) {
		printf("open_port: Unable to open /dev/ttyUSB0. \n");
	} else {
		fcntl(fd, F_SETFL, 0);
		printf("port is open.\n");
	}
	return(fd);
}

int configure_port(int fd)
{
	struct termios port_settings;

	cfsetispeed(&port_settings, B115200);
	cfsetospeed(&port_settings, B115200);

	port_settings.c_cflag &= ~PARENB;		// set no parity
	port_settings.c_cflag &= ~CSTOPB;		// set stop bits to 1
	port_settings.c_cflag &= ~CRTSCTS;		// disable hardware flow control
	port_settings.c_cflag &= ~CSIZE;		// set databits to 8
	port_settings.c_cflag |= CS8;

	port_settings.c_iflag &= ~(IXON | IXOFF | IXANY); // disable software flow control

	tcsetattr(fd, TCSANOW, &port_settings);    // apply the settings to the port
	return(fd);

}

bool control_callback(joy_test::JoyIn::Request  &req, joy_test::JoyIn::Response &res)
{
	res.success = 1; 


	int flag = req.flag;
//	ROS_INFO("%ld",req.stream.size());
	vector<uint8_t> v = req.stream;
	if (flag == 1) {
		char cmd[1];
		copy(v.begin(),v.end(),cmd);
		write(fd, cmd, sizeof(cmd));
	} else if (flag == 2) {
		char beep[7];
		copy(v.begin(),v.end(),beep);
		write(fd, beep, sizeof(beep));
	} else if (flag == 3) {
		char song[2];
		copy(v.begin(),v.end(),song);
		write(fd, song, sizeof(song));
	} else {
		char byte[5];
		copy(v.begin(),v.end(),byte);
		write(fd, byte, sizeof(byte));
	}
	return true;
}
