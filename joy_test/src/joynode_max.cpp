#include <ros/ros.h>
#include<geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <joy_test/JoyIn.h>

using namespace std;
class TeleopTurtle
{
public:
  TeleopTurtle();

private:
  void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);
  
  ros::NodeHandle nh_;

  int linear_, angular_;
  double l_scale_, a_scale_;
//  ros::Publisher vel_pub_;
  ros::Subscriber joy_sub_;
};


TeleopTurtle::TeleopTurtle()
{

  //vel_pub_ = nh_.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 1);
  joy_sub_ = nh_.subscribe<sensor_msgs::Joy>("joy", 10, &TeleopTurtle::joyCallback, this);

}

void TeleopTurtle::joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
  vector<float> ax = joy->axes;
	vector<int> b = joy->buttons;
		if(abs(ax[1])<0.0005) ax[1]=0;
		if(abs(ax[2])<0.0005) ax[2]=0;
		if(abs(ax[3])<0.0005) ax[3]=0;

//  	vel.linear.x = ax[1];
//		vel.angular.z = atan2(ax[2],(ax[3]));

	ros::NodeHandle n;
	ros::service::waitForService("joy_in");
  ros::ServiceClient client = n.serviceClient<joy_test::JoyIn>("joy_in");
  joy_test::JoyIn srv;
	char h=65,l=65;
	long int vel = 0;
	int max_vel = 200;
	vector<uint8_t> stream;
	{
		if(b[0]==1)
		{
			cout << "Power on\n";
			stream.push_back(128);
			srv.request.flag = 1;
		}
		else if(b[1]==1)
		{
			cout << "Safe mode\n";
			stream.push_back(131);
			srv.request.flag = 1;
		}
		else if(b[2]==1)
		{
			cout << "Beeping\n";
			stream.push_back(140);
			stream.push_back(3);
			stream.push_back(1);
			stream.push_back(6);
			stream.push_back(16);
			stream.push_back(14);
			stream.push_back(3);
			srv.request.flag = 2;
		}
		else if(b[3]==1)
		{
			cout << "Reset iCreate2\n";
			stream.push_back(7);
			srv.request.flag = 1;	
		}
		else if(b[4]==1)
		{
			cout << "Power down\n";
			stream.push_back(133);
			srv.request.flag = 1;	
		}
		else if(b[5]==1)
		{
			cout << "Dock\n";
			stream.push_back(143);
			srv.request.flag = 1;	
		}
		else if(b[6]==1)
		{
			cout << "Play song\n";
			stream.push_back(141);
			stream.push_back(1);
			srv.request.flag = 3;	
		}
		else if(ax[2]>0)
		{
			cout << "Rotating left\n";
			stream.push_back(145);
			stream.push_back(0);
			stream.push_back(150);
			stream.push_back(255);
			stream.push_back(106);
		}
		else if(ax[2]<0)
		{
			cout << "Rotating right\n";
			stream.push_back(145);
			stream.push_back(255);
			stream.push_back(106);
			stream.push_back(0);
			stream.push_back(150);
		}
		else  
		{
			vel = (long int)(ax[1]*max_vel);
			cout << "Moving at " << vel << " mm/s\n";
			l = (vel&0x00ff);
			h = ((vel>>8)&0x00ff);
			stream.push_back(145);
			stream.push_back(h);
			stream.push_back(l);
			stream.push_back(h);
			stream.push_back(l);
		}
	}
	srv.request.stream = stream;
//  srv.request.b = atoll(argv[2]);
  if (client.call(srv))
  {
    ROS_INFO("Success: %d", srv.response.success);
  }
  else
  {
    ROS_ERROR("Failed to call service joy_in");
    return;
  }
 
//sleep(1);
	ROS_INFO("%f %f %f %f %f %f %ld",ax[0],ax[1],ax[2],ax[3],ax[4],ax[5],ax.size());
	ROS_INFO("%ld %ld %ld %ld  %ld",b[0],b[1],b[2],b[3],b.size());

}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "joynode");
  TeleopTurtle teleop_turtle;

  ros::spin();
}


