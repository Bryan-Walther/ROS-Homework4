/*
*   Bryan Walther
*   Homework 4
*   February 12, 2020
*
*   Parse Keyboard and send point
*/


#include "ros/ros.h"
#include "geometry_msgs/Point.h"

using namespace std;

int main(int argc, char **argv)
{
	ros::init(argc, argv, "HWturtle_read");
	ros::NodeHandle n;
	ros::Publisher target_pub;	//set up publisher
    geometry_msgs::Point targetPnt; //setup point to send

	target_pub = n.advertise<geometry_msgs::Point>("/goal", 1000);

	while(ros::ok()) 
    {
	    cout << "------------\nEnter x: "; //prompt and read x and y location
	    cin >> targetPnt.x;
	    cout << "Enter y: ";
	    cin >> targetPnt.y;

        target_pub.publish(targetPnt);		//send it!
    }
}