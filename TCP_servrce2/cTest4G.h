/*
 * cTest4G.h
 *
 *  Created on: 2018年7月25日
 *      Author: tarena
 *      //打开4G模块，使用脚本配置4G模块参数，通过socket连接
 *
 */

#ifndef CTEST4G_H_
#define CTEST4G_H_
#include "cTestBase.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>


#define GPIO_4G_IGT     112  /*PD16*/
#define GPIO_4G_RST     114  /*PD18*/
#define GPIO_MODE_INPUT    0
#define GPIO_MODE_OUTPUT   1
#define GPIO_MODE_INT_FALL 2
#define GPIO_MODE_INT_RISE 3
#define GPIO_MODE_INT_BOTH 4
#define FOURG_BUFSIZE	1024



class CTest4G:public CTestBase
{
public:
	virtual int test(int fd);
	CTest4G();
	~CTest4G();
	void SleepTime(long ms);
	int test4G();
	static int m_Flag;
protected:
	int readFile();
	int sockClient();
	bool m_reset;

private:
	//nt m_iSockfd;
};

#endif /* CTEST4G_H_ */
