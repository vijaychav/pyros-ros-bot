#include <ros/ros.h>
#include<geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <joy_test/JoyIn.h>
#include <joy_test/Target.h>
#include <sensorcontroller/SerialComm.h>

using namespace std;
int x_mid = 320;
int thresh = 1;
float fwd_thresh = 1;


bool reach_callback(joy_test::Target::Request  &req, joy_test::Target::Response &res)
{

//  	vel.linear.x = ax[1];
//		vel.angular.z = atan2(ax[2],(ax[3]));

	ros::NodeHandle n;
	ros::service::waitForService("joy_in");
  ros::ServiceClient client = n.serviceClient<joy_test::JoyIn>("joy_in");
  joy_test::JoyIn srv;
	char h=65,l=65;
	long int vel = 0;
	int max_vel = 200;
	int diff = 0;
	int thresh = 100;
	if(req.mode ==1 || req.mode ==2)
	{
		diff = req.target - x_mid;
		thresh = 100;
	}
	else
	{	
		diff = req.target;
	}

	vector<uint8_t> stream;
	if(req.mode == 1 || req.mode == 2)
	{
		cout << diff	<< thresh << endl;
		ROS_INFO("%d",diff);
		if(diff<(-thresh))
		{
			cout << "Rotating left\n";
			stream.clear();
			stream.push_back(145);
			stream.push_back(0);
			stream.push_back(5);
			stream.push_back(255);
			stream.push_back(251);
			srv.request.stream = stream;
		  if (client.call(srv))
  		{
    		ROS_INFO("Success: %d", srv.response.success);
  		}
  		else
  		{
    		ROS_ERROR("Failed to call service joy_in_1");
	    	return false;
  		}
			
/*		 usleep(500000);
			stream.clear();	
			stream.push_back(145);
			stream.push_back(0);
			stream.push_back(0);
			stream.push_back(0);
			stream.push_back(0);
			srv.request.stream = stream;
	  	if (client.call(srv))
			{
	  		ROS_INFO("Success: %d", srv.response.success);
			}
			else
			{
  			ROS_ERROR("Failed to call service joy_in2_1");
		  	return false;
			}
*/

		}
		else if(diff>thresh)
		{
			cout << "Rotating right\n";
			stream.clear();
			stream.push_back(145);
			stream.push_back(255);
			stream.push_back(251);
			stream.push_back(0);
			stream.push_back(5);
			srv.request.stream = stream;
		  if (client.call(srv))
  		{
    		ROS_INFO("Success: %d", srv.response.success);
  		}
  		else
  		{
    		ROS_ERROR("Failed to call service joy_in3_1");
	    	return false;
  		}

	/*	  usleep(500000);
			stream.clear();	
			stream.push_back(145);
			stream.push_back(0);
			stream.push_back(0);
			stream.push_back(0);
			stream.push_back(0);
			srv.request.stream = stream;
	  	if (client.call(srv))
			{
	  		ROS_INFO("Success: %d", srv.response.success);
			}
			else
			{
  			ROS_ERROR("Failed to call service joy_in2_1");
		  	return false;
			}
		*/	
			stream.clear();	
			stream.push_back(141);
			stream.push_back(req.mode);
			srv.request.flag = 3;	
			srv.request.stream = stream;
	  	if (client.call(srv))
			{
	  		ROS_INFO("Success: %d", srv.response.success);
			}
			else
			{
  			ROS_ERROR("Failed to call service joy_in2_1");
		  	return false;
			}
		}
		else  
		{
			vel = (long int)(max_vel);
			cout << "Moving at " << vel << " mm/s\n";
			l = (vel&0x00ff);
			h = ((vel>>8)&0x00ff);
			stream.clear();
			stream.push_back(145);
			stream.push_back(h);
			stream.push_back(l);
			stream.push_back(h);
			stream.push_back(l);
			srv.request.stream = stream;
	  	if (client.call(srv))
			{
	  		ROS_INFO("Success: %d", srv.response.success);
			}
			else
			{
  			ROS_ERROR("Failed to call service joy_in1");
		  	return false;
			}
			
			int safe = 1;
			ros::NodeHandle dist_n;
		  ros::ServiceClient client_dist = dist_n.serviceClient<sensorcontroller::SerialComm>("serial_service");
			sensorcontroller::SerialComm srv_dist;
			srv_dist.request.mode = 1;

			if (client_dist.call(srv_dist))
			{
				safe = srv_dist.response.s_intdata;
				ROS_INFO("Safe: %d", safe);
			}
			else
			{
				ROS_ERROR("Failed to call service serial_service");
				return false;
			}
		
			stream.clear();	
			stream.push_back(145);
			stream.push_back(0);
			stream.push_back(0);
			stream.push_back(0);
			stream.push_back(0);
			srv.request.stream = stream;
	  	if (client.call(srv))
			{
	  		ROS_INFO("Success: %d", srv.response.success);
			}
			else
			{
  			ROS_ERROR("Failed to call service joy_in2");
		  	return false;
			}
		}
	}
	else
	{
		ros::NodeHandle dist_n;
		ros::ServiceClient client_dist = dist_n.serviceClient<sensorcontroller::SerialComm>("serial_service");
		sensorcontroller::SerialComm srv_dist;
		srv_dist.request.mode = 3;
		int dist = 0;
		if (client_dist.call(srv_dist))
		{
			dist = srv_dist.response.s_intdata;
		}
		else
		{
			ROS_ERROR("Failed to call service serial_service");
			return false;
		}
		while(true)
		{
			
		if (client_dist.call(srv_dist))
		{
			diff = srv_dist.response.s_intdata - dist;
			thresh = 10;
			ROS_INFO("Diff: %d", diff);
		}
		else
		{
			ROS_ERROR("Failed to call service serial_service");
			return false;
		}
		
		ROS_INFO("%d",diff);
	
		if(diff<thresh)
		{
			cout << "Rotating right\n";
			stream.clear();
			stream.push_back(145);
			stream.push_back(255);
			stream.push_back(251);
			stream.push_back(0);
			stream.push_back(5);
			srv.request.stream = stream;
		  if (client.call(srv))
  		{
    		ROS_INFO("Success: %d", srv.response.success);
  		}
  		else
  		{
    		ROS_ERROR("Failed to call service joy_in");
	    	return false;
  		}
		}
		else  
		{


			int safe = 1;
			ros::NodeHandle dist_n;
		  ros::ServiceClient client_dist = dist_n.serviceClient<sensorcontroller::SerialComm>("serial_service");
			sensorcontroller::SerialComm srv_dist;
			
			int turn = -1;

			
			srv_dist.request.mode = 4;

			while(turn)
			{
				if(turn == 1)
				{
					cout << "Rotating left\n";
					stream.clear();
					stream.push_back(145);
					stream.push_back(0);
					stream.push_back(5);
					stream.push_back(255);
					stream.push_back(251);
					srv.request.stream = stream;
				  if (client.call(srv))
		  		{
		    		ROS_INFO("Success: %d", srv.response.success);
		  		}
		  		else
		  		{
		    		ROS_ERROR("Failed to call service joy_in");
			    	return false;
		  		}
		/*
				usleep(500000);
					stream.clear();	
					stream.push_back(145);
					stream.push_back(0);
					stream.push_back(0);
					stream.push_back(0);
					stream.push_back(0);
					srv.request.stream = stream;
			  	if (client.call(srv))
					{
			  		ROS_INFO("Success: %d", srv.response.success);
					}
					else
					{
		  			ROS_ERROR("Failed to call service joy_in2");
				  	return false;
					}
		*/
		
				}
				else if(turn==-1)
				{
					cout << "Rotating right\n";
					stream.clear();
					stream.push_back(145);
					stream.push_back(255);
					stream.push_back(254);
					stream.push_back(0);
					stream.push_back(2);
					srv.request.stream = stream;
				  if (client.call(srv))
		  		{
		    		ROS_INFO("Success: %d", srv.response.success);
		  		}
		  		else
		  		{
		    		ROS_ERROR("Failed to call service joy_in");
			    	return false;
		  		}
			/*		
					usleep(500000);	
					stream.clear();	
					stream.push_back(145);
					stream.push_back(0);
					stream.push_back(0);
					stream.push_back(0);
					stream.push_back(0);
					srv.request.stream = stream;
			  	if (client.call(srv))
					{
			  		ROS_INFO("Success: %d", srv.response.success);
					}
					else
					{
		  			ROS_ERROR("Failed to call service joy_in2");
				  	return false;
					}
		*/
				}

				if (client_dist.call(srv_dist))
				{
					turn = srv_dist.response.s_intdata;
					ROS_INFO("Turn: %d", turn);
				}
					else
				{
					ROS_ERROR("Failed to call service serial_service");
					return false;
				}
			}

			srv_dist.request.mode = 1;

			if (client_dist.call(srv_dist))
			{
				safe = srv_dist.response.s_intdata;
				ROS_INFO("Safe: %d", safe);
			}
				else
			{
				ROS_ERROR("Failed to call service serial_service");
				return false;
			}
			
			stream.clear();
			stream.push_back(145);
			stream.push_back(0);
			stream.push_back(0);
			stream.push_back(0);
			stream.push_back(0);
			srv.request.stream = stream;
	  	if (client.call(srv))
			{
	  		ROS_INFO("Success: %d", srv.response.success);
			}
			else
			{
  			ROS_ERROR("Failed to call service joy_in");
		  	return false;
			}
			break;
		}
		}		
	}
//  srv.request.b = atoll(argv[2]);
//sleep(1);
	return true;
}


int main(int argc, char** argv)
{
	ros::init(argc, argv, "reach_target_node");
	ros::NodeHandle n;

	ros::ServiceServer service = n.advertiseService("reach_target",reach_callback);
	ros::spin();
	return 0;
}


