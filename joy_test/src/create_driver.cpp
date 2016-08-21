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

int i=0;	
while(i<2)
{
	if(i==0)
	{
		if(req.turn==1)
		{
			i++;		
			curr_cmd = 10;// left
		}
		else
		if(req.turn==2)
		{
			i++;
			curr_cmd = 11; // right
		}
		else
		{
			// do nothing
		}
		if(req.dir==0)
		{
			curr_cmd =1;
		}
	}
	else
	{
		if(req.dir==1)
		{
			curr_cmd = 8; // front
		}
		else
		if(req.dir==2)
		{
			curr_cmd = 9; // back;
		}
		else
		{
			curr_cmd =2;
		}
	}
	int flag = 0;
	char byte[5];
	char cmd[1];
	char beep[7];
	char h=65,l=65;
	long int vel = 0;
	int max_vel = 200;
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
			vel = (long int)(req.vel*max_vel);
			cout << "Moving forward at " << vel << " mm/s\n";
			l = (vel&0x00ff);
			h = ((vel>>8)&0x00ff);
			byte[0] = 145;
			byte[1] = h;
			byte[2] = l;
			byte[3] = h;
			byte[4] = l;
			break;
		case 9:
			vel = (long int)(-req.vel*max_vel);
			l = (vel&0x00ff);
			h = ((vel>>8)&0x00ff);
			cout << "Moving backward at " << vel << " mm/s\n";
			byte[0] = 145;
			byte[1] = h;
			byte[2] = l;
			byte[3] = h;
			byte[4] = l;
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
	} else {
		write(fd, byte, sizeof(byte));
	}
	i++;
}
	return true;
}
