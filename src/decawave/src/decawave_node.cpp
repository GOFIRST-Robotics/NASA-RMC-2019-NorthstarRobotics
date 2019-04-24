/*
 * decawave_node.cpp
 * ROS interface to Decawave class
 * VERSION: 1.0
 * Last changed: 2019-04-01
 * Authors: Amalia Schwartzwald <schw1818@umn.edu>
 * Maintainers: Amalia Schwartzwald <schw1818@umn.edu>
 * MIT License
 * Copyright (c) 2018 GOFIRST-Robotics
 */

// ROS Libs
#include <ros/ros.h>
#include <nav_msgs/Odometry.h>

// Native_Libs
#include <string>

// Custom_Libs
#include "decawave/decawave.h"
#include <serial/serial.h>

// Subscribers (inputs)
//    update_timer (Timer)
//      Update loop for reading / querying Decawave
//    sub_name1 (sub_name1_type): sub_name1_TOPIC_VALUE
//      sub_name1_desc

// Publishers (outputs)
//    gps_pub (nav_msgs/Odometry): "odometry/gps"
//      A simulated local position as if GPS UTM

// Parameters (settings)
//    frequency (double): default=50.0
//      The update frequency of the update loop
//    param_name2 (param_name2_type): default=param_name2_default(,param_name1_path)
//    param_name3 (param_name3_type): default=param_name3_default(,param_name1_path)


// ROS Node and Publishers
ros::NodeHandle * nh;
ros::Publisher gps_pub;

// ROS Topics
std::string gps_topic = "odometry/gps";

// ROS Callbacks
void update_callback(const ros::TimerEvent&);

// ROS Params
double frequency = 50.0;
//param_name2_type param_name2 = param_name2_default;
//param_name3_type param_name3 = param_name3_default;

// Global_Vars
Decawave piTag;
coordinate tagPos;

int main(int argc, char** argv){
  // Init ROS
  ros::init(argc, argv, "decawave_node");
  nh = new ros::NodeHandle("~");

  // Subscribers
  ros::Timer update_timer = nh->createTimer(ros::Duration(1.0/frequency), update_callback);

  // Publishers
  gps_pub = nh->advertise<nav_msgs::Odometry>(gps_topic, 10);

  // Params
  nh->param<double>("frequency", frequency);
  //nh->param<param_name2_type>(param_name2_path, param_name2, param_name2_default;
  //nh->param<param_name3_type>(param_name3_path, param_name3, param_name3_default;

  // Initialize
  // get initial decwave data
  for(int i = 0; i < 10; ++i){
    piTag.updateSamples();
  }

  // Spin
  ros::spin();
}


void update_callback(const ros::TimerEvent&){
  nav_msgs::Odometry msg;
  
  // update decawave data
  piTag.updateSamples();

  // get tag position from the decawave
  tagPos = piTag.getPos();

  // put the position data into the message
  msg.pose.pose.position.x = tagPos.x;
  msg.pose.pose.position.y = tagPos.y;

  // fill out message header
  msg.header.stamp = ros::Time::now();
  msg.header.frame_id = "odom";
  msg.child_frame_id = "base_link"; //change to correct part

  gps_pub.publish(msg);
}
