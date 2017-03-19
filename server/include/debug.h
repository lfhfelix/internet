#ifndef __DEBUG_H__
#define __DEBUG_H__

//#define DEBUG

#define SMS_SERIAL		("/dev/ttyUSB0")			//指定SMS串口
#define M0_SERIAL		("/dev/ttyUSB1")            //指定M0串口
#define CAMERA_USB		("/dev/video0")

/*指定M0模块ID*/
#define M0_RECV_SELCET		((env_temp.id == 0x05) || (env_temp.id == 0x06))
#define M0_SEND_SELCET		(0x05)  			//只能指定一个

#endif