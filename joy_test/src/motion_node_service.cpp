#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

static const std::string MOG2_WINDOW = "Video Output";
class imageCapture
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;

public:
  imageCapture()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/webcam/image_raw", 1, &imageCapture::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);

    ROS_INFO("Init");
    cv::namedWindow(MOG2_WINDOW);
    //ros::spinOnce();
    //pMOG2(5, 10, true); //MOG2 approach,
  }

  ~imageCapture()
  {
    cv::destroyWindow(MOG2_WINDOW);
  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {

    //ROS_INFO("I am here too");
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }


		cv::cvtColor(cv_ptr->image, cv_ptr->image, CV_BGR2GRAY);
		cv::waitKey(3);

    // Output modified video stream
		imshow(MOG2_WINDOW,cv_ptr->image);
    image_pub_.publish(cv_ptr->toImageMsg());
  }

};

int main(int argc, char **argv){
	ros::init(argc, argv, "motion_node_final");
	imageCapture modder;
	ros::spin();

	return 0;
}


