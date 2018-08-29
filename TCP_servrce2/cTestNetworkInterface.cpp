/*
 * cTestNetworkInterface.cpp
 *
 *  Created on: 2018年7月21日
 *      Author: tarena
 */

#include "cTestNetworkInterface.h"

CTestNetworkInterface::CTestNetworkInterface() {
	// TODO Auto-generated constructor stub
}

//网络接口测试
int CTestNetworkInterface::test(int fd)
{
	setFd(fd);
	char buffer[1024];
	memset(buffer,0,sizeof(buffer));
	strcpy(buffer,"SUCCESSFUL:Network Interface is correct");
	sendJSON(RESULT,buffer);
	memset(buffer,0,sizeof(buffer));
	strcpy(buffer,"Network Interface test end");
	sendJSON(END,buffer);
};


CTestNetworkInterface::~CTestNetworkInterface() {
	// TODO Auto-generated destructor stub
}

