/*
*   Bryan Walther
*   Homework 4
*   February 12, 2020
*
*   move the turtle from the read location to target point
*/


#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Point.h"
#include "turtlesim/Pose.h"

turtlesim::Pose turtlesim_pose; //turtle location
geometry_msgs::Point targetPnt; //target point
bool start = false;

const double PI = 3.14159265359;	//MMmmmm.... pie....

double getTargetAngle () {//Compare current heading and angle-to-target
	double target_angle_radians = atan2((targetPnt.y - turtlesim_pose.y),(targetPnt.x - turtlesim_pose.x));   //calculate angle-to-target
    double relative_angle_radians = target_angle_radians - turtlesim_pose.theta;							//calculate the actual relative angle

	if (relative_angle_radians > PI)									//safety catch, if the angle is too large or too small, use the equivalent angle within [-PI,PI)
		relative_angle_radians = relative_angle_radians - 2*PI;
	else if (relative_angle_radians < -PI)
		relative_angle_radians = relative_angle_radians + 2*PI;
    return(relative_angle_radians);//send it
}
double getTargetDistance(){//compare distance between current location and target
	double relative_distance = sqrt(pow(targetPnt.x-turtlesim_pose.x,2)+pow(targetPnt.y-turtlesim_pose.y,2)); //distance formula
	return(relative_distance);//send it
}

void pointCallback(const geometry_msgs::Point& target) //hear when new command is sent
{
    targetPnt.x = target.x;
    targetPnt.y = target.y;
	start = true; //first run will set true, then keep setting true but do nothing really.
}

void poseCallback(const turtlesim::Pose::ConstPtr& pose_msg){ //get current x, y, orientation
	turtlesim_pose.x=pose_msg->x;
	turtlesim_pose.y=pose_msg->y;
	turtlesim_pose.theta=pose_msg->theta; // current absolute orientation of the turtle
}

int main(int argc, char **argv) //main
{
	ros::init(argc, argv, "HWturtle_goto");
	ros::NodeHandle n;

	ros::Publisher velocity_publisher; //setup publishers and subscribers
	ros::Subscriber pose_subscriber;
	ros::Subscriber target_sub;	
	
	velocity_publisher = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 1000);
	pose_subscriber = n.subscribe("/turtle1/pose", 10, poseCallback);
    target_sub = n.subscribe("/goal",10,pointCallback);

	geometry_msgs::Twist vel_msg;	//Writer values
	ros::Rate loop_rate(360);

	double relativeAngle, relativeDist; //Constant definition for Proportional controller
	const float angleMult = 2;
	const float distMult = 0.7;
	
	while (ros::ok())
	{
		ros::spinOnce();//Read data
		if(start){		//I was having issues with a start up value, so I ignore the movement commands until first command
			vel_msg.linear.x = 0; //speed;
			vel_msg.linear.y = 0;
			vel_msg.linear.z = 0;
			vel_msg.angular.x = 0;
			vel_msg.angular.y = 0;
			vel_msg.angular.z = 0; //Angular Velocity

			relativeAngle = getTargetAngle();
			relativeDist = getTargetDistance();
			if(relativeAngle > 0.001 || relativeAngle < -0.001) //Turn until angle is "close enough", within 0.001 radians of the target
				vel_msg.angular.z = relativeAngle*angleMult;	//angular velocity based of relative angle and a constant multiplyer
			
			if (relativeDist > 0.001)							//Travel until distance is within 0.001 units
				vel_msg.linear.x = relativeDist*distMult;		//speed based of relative distance and a constant

			velocity_publisher.publish(vel_msg);				//write it!
		}
		loop_rate.sleep();										//Wait timer
	}
	return 0;
}
