#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

static const std::string OPENCV_WINDOW = "Raw Image window";
static const std::string OPENCV_WINDOW_1 = "Edge Detection";

class Edge_Detector
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  
public:
  Edge_Detector()
    : it_(nh_)
  {
    // Subscribe to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/rgbd_camera/rgb/image_raw", 1, 
      &Edge_Detector::imageCb, this);
    image_pub_ = it_.advertise("/edge_detector/raw_image", 1);

    cv::namedWindow(OPENCV_WINDOW);

  }

  ~Edge_Detector()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {

    cv_bridge::CvImagePtr cv_ptr;
    namespace enc = sensor_msgs::image_encodings;

    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
			cv::imwrite("/home/jon/hiudh.jpg",cv_ptr->image);
			cv::Size patternsize(4,7);
			std::vector<cv::Point2f>corners;
			bool patternfoundl = cv::findChessboardCorners(cv_ptr->image, patternsize, corners);
			cv::drawChessboardCorners(cv_ptr->image, patternsize, corners, patternfoundl);
			std::cout<<corners.at(0).x<<std::endl;
			cv::imshow("abc", cv_ptr->image);
			cv::waitKey(0);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

    // Draw an example circle on the video stream
    //if (cv_ptr->image.rows > 400 && cv_ptr->image.cols > 600){

	detect_edges(cv_ptr->image);
    	image_pub_.publish(cv_ptr->toImageMsg());

	//}
  }
  void detect_edges(cv::Mat img)
  {

   	cv::Mat src, src_gray;
	cv::Mat dst, detected_edges;

	int edgeThresh = 1;
	int lowThreshold = 200;
	int highThreshold =300;
	int kernel_size = 5;

	img.copyTo(src);

	cv::cvtColor( img, src_gray, CV_BGR2GRAY );
        cv::blur( src_gray, detected_edges, cv::Size(5,5) );
	cv::Canny( detected_edges, detected_edges, lowThreshold, highThreshold, kernel_size );

  	dst = cv::Scalar::all(0);
  	img.copyTo( dst, detected_edges);
	dst.copyTo(img);

    	cv::imshow(OPENCV_WINDOW, src);
    	cv::imshow(OPENCV_WINDOW_1, dst);
    	cv::waitKey(3);

  }	
 
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "Edge_Detector");
  Edge_Detector ic;
  ros::spin();
  return 0;
}