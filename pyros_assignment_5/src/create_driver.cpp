#include <iostream>
#include <cstdio>
#include <ros/ros.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <malloc.h>
#include "pyros_assignment_5/create_control_service.h"
 // #include <serial/serial.h>

#define ROTATIONCHANGE 300
#define VELOCITYCHANGE 200

int prev_cmd = 0;
int curr_cmd = 0;
int fd = -1;

using namespace std;

int open_port(void);
int configure_port(int fd);
bool control_callback(pyros_assignment_5::create_control_service::Request  &req, pyros_assignment_5::create_control_service::Response &res);
int tcdrain(int fildes);

int main(int argc, char *argv[])
{
	ros::init(argc, argv, "create_driver");
	ros::NodeHandle n;

	ros::ServiceServer service = n.advertiseService("create_control_service",control_callback);

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

bool control_callback(pyros_assignment_5::create_control_service::Request  &req, pyros_assignment_5::create_control_service::Response &res)
{
	res.mode = req.mode;
	prev_cmd = curr_cmd;
	curr_cmd = res.mode;
	if (curr_cmd == prev_cmd)
	{
		return true;
	}
	int flag = 0;
	int read_f = 0;
	char byte[5];
	char cmd[1];
	char beep[7];
	char sense[2];
	switch(curr_cmd) {
		case 1:
			cout << "Power on\n";
			cmd[0] = 128;
			flag = 1;
			break;
		case 2:
			cout << "Safe mode\n";
			cmd[0] = 131;
			flag = 1;
			break;
		case 6:
			cout << "Beeping\n";
			beep[0] = 140;
			beep[1] = 3;
			beep[2] = 1;
			beep[3] = 64;
			beep[4] = 16;
			beep[5] = 141;
			beep[6] = 3;
			flag = 2;
			break;
		case 7:
			cout << "Reset iCreate2\n";
			cmd[0] = 7;
			flag = 1;
			break;
		case 8:
			cout << "Moving forward at 200 mm/s\n";
			byte[0] = 145;
			byte[1] = 0;
			byte[2] = 200;
			byte[3] = 0;
			byte[4] = 200;
			break;
		case 9:
			cout << "Moving backward at 200 mm/s\n";
			byte[0] = 145;
			byte[1] = 255;
			byte[2] = 56;
			byte[3] = 255;
			byte[4] = 56;
			break;
		case 10:
			cout << "Rotating left\n";
			byte[0] = 145;
			byte[1] = 0;
			byte[2] = 150;
			byte[3] = 255;
			byte[4] = 106;
			break;
		case 11:
			cout << "Rotating right\n";
			byte[0] = 145;
			byte[1] = 255;
			byte[2] = 106;
			byte[3] = 0;
			byte[4] = 150;
			break;
		case 12:
			cout << "Bump and Wheeldrop Status";
			flag = 3;
			read_f = 1;
			sense[0] = 142;
		//	sense[1] = 25; // Battery charge
			sense[1] = 7;
			break;
		case 13:
			cout << "Brake\n";
			byte[0] = 145;
			byte[1] = 0;
			byte[2] = 0;
			byte[3] = 0;
			byte[4] = 0;
			break;
		case 0:
			cout << "Brake\n";
			byte[0] = 145;
			byte[1] = 0;
			byte[2] = 0;
			byte[3] = 0;
			byte[4] = 0;
			break;
		default:
			cout << "I dunno what that is\n";
			//char byte[] = {145, 0, 0, 0, 0};
			break;
	}
	if (flag == 1) {
		write(fd, cmd, sizeof(cmd));
	} else if (flag == 2) {
		write(fd, beep, sizeof(beep));
	} else if ( flag == 3 ) {
		write(fd, sense, sizeof(sense));
	} else {
		write(fd, byte, sizeof(byte));
	}
	tcdrain(fd);
	if ( read_f == 1 )
	{
/* 		read(fd, sense, sizeof(sense));
		uint16_t n = sense[0] | sense[1] << 8;
		cout << n; */

 		read(fd, cmd, sizeof(cmd));
		uint16_t n = cmd[0] & 3 , n1 = cmd[0] & 12;
		cout << " " << n << " " << n1 << " "; 

	}
	return true;
}
