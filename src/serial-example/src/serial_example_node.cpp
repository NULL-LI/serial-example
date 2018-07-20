/***
 * This example expects the serial port has a loopback on it.
 *
 * Alternatively, you could use an Arduino:
 *
 * <pre>
 *  void setup() {
 *    Serial.begin(<insert your baudrate here>);
 *  }
 *
 *  void loop() {
 *    if (Serial.available()) {
 *      Serial.write(Serial.read());
 *    }
 *  }
 * </pre>
 */

#include <ros/ros.h>
#include <serial/serial.h>
#include <std_msgs/Empty.h>
#include <std_msgs/String.h>
#include <string>

#define USHORT unsigned short int
#define BYTE unsigned char
#define CHAR char
#define UCHAR unsigned char
#define UINT unsigned int
#define DWORD unsigned int
#define PVOID void*
#define ULONG unsigned int
#define INT int
#define UINT32 UINT
#define LPVOID void*
#define BOOL BYTE
#define TRUE 1
#define FALSE 0

using namespace std;
serial::Serial ser;

void write_callback(const std_msgs::String::ConstPtr& msg) {
  ROS_INFO_STREAM("Writing to serial port" << msg->data);
  ser.write(msg->data);
}

int main(int argc, char** argv) {
  /*test of linux data format*/
  //    __int16_t a=-2;
  //    BYTE* testbyteptr=(BYTE*)&a;
  //    printf("high %d  low %d \n",*(testbyteptr+0),*(testbyteptr+1));

  ros::init(argc, argv, "serial_example_node");
  ros::NodeHandle nh;

  ros::Subscriber write_sub = nh.subscribe("write", 1000, write_callback);
  ros::Publisher read_pub = nh.advertise<std_msgs::String>("read", 1000);

  try {
    ser.setPort("/dev/ttyUSB0");
    ser.setBaudrate(9600);
    serial::Timeout to = serial::Timeout::simpleTimeout(1000);
    ser.setTimeout(to);
    ser.open();
  } catch (serial::IOException& e) {
    ROS_ERROR_STREAM("Unable to open port ");
    return -1;
  }

  if (ser.isOpen()) {
    ROS_INFO_STREAM("Serial Port initialized");
  } else {
    return -1;
  }

  ros::Rate loop_rate(5);
  while (ros::ok()) {
    ros::spinOnce();

    if (ser.available()) {
      ROS_INFO_STREAM("Reading from serial port ");
      std::string datastr;
      datastr = ser.read(ser.available());
      printf("bytes: %ld \n", datastr.length());
      BYTE* byteptr = (BYTE*)datastr.data();
      /*print original data*/
      for (int i = 0; i < datastr.length(); i++) {
        //          printf("%d ",*(byteptr+i));
      }

      if (*(byteptr + 0) == 187 && *(byteptr + 1) == 187 &&
          *(byteptr + 2) == 187) {
        BYTE datahex[2] = {*(byteptr + 6), *(byteptr + 5)};
        __int16_t* dataint = (__int16_t*)datahex;

        float datafloat = -*dataint / 100.0;
        printf("data %f \n", datafloat);  // measured data
      }
    } else {
      //            ROS_INFO("ser unavailable()");
    }
    loop_rate.sleep();
  }
}
