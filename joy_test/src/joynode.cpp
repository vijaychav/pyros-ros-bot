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
  ros::Publisher vel_pub_;
  ros::Subscriber joy_sub_;
};


TeleopTurtle::TeleopTurtle():
  linear_(1),
  angular_(2)
{

  nh_.param("axis_linear", linear_, linear_);
  nh_.param("axis_angular", angular_, angular_);
  nh_.param("scale_angular", a_scale_, a_scale_);
  nh_.param("scale_linear", l_scale_, l_scale_);


  vel_pub_ = nh_.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 1);


  joy_sub_ = nh_.subscribe<sensor_msgs::Joy>("joy", 10, &TeleopTurtle::joyCallback, this);

}

void TeleopTurtle::joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
/*  geometry_msgs::Twist vel;
  vel.angular = a_scale_*joy->axes[angular_];
  vel.linear = l_scale_*joy->axes[linear_];
  vel_pub_.publish(vel);*/
  geometry_msgs::Twist vel;
  vector<float> ax = joy->axes;

	a_scale_ = 1;
	l_scale_ = 1;
//  vel.angular.x = a_scale_*ax[3];
//  vel.angular.y = a_scale_*ax[4];
		if(abs(ax[1])<0.0005) ax[1]=0;
		if(abs(ax[2])<0.0005) ax[2]=0;
		if(abs(ax[3])<0.0005) ax[3]=0;

  	vel.linear.x = ax[1];
		vel.angular.z = atan2(ax[2],(ax[3]));
//  vel.linear.y = l_scale_*ax[2];

//  vel.linear = joy->axes[linear_];

//  	vel_pub_.publish(vel);  //  Needed for turtlesim 
	ros::NodeHandle n;
	ros::service::waitForService("joy_in");
  ros::ServiceClient client = n.serviceClient<joy_test::JoyIn>("joy_in");
  joy_test::JoyIn srv;
	if(ax[3]>=0)
	{
		srv.request.dir = 1; // Front
	}
	else
	{
		srv.request.dir = 2; // Back
	}
	if(ax[5]==1)
	{
		srv.request.dir = 0;
	}
	if(ax[2]>0)
	{
		srv.request.turn = 1; // Left
	}
	else
	if(ax[2]<0)
	{
		srv.request.turn = 2; // Right
	}
	else
	{
		srv.request.turn = 0; // Dont turn
	}
	if(ax[1]>0)
	{
		srv.request.vel = ax[1];
	}
	else
	if(ax[1]<0)
	{
		srv.request.vel = ax[1];
	}
	else
	{
		srv.request.vel = 0;
	}
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

}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "joynode");
  TeleopTurtle teleop_turtle;

  ros::spin();
}


