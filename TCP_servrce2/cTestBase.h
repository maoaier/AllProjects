//所以测试的基类，用来保存fd和发送数据，test()也可以虚类处理
#ifndef CTESTBASE_H
#define CTESTBASE_H
#include "cJSON.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <sys/select.h>
#include <errno.h>
#include <vector>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>

class CTestBase
{
public:

	static void setTestFlagBit(int flagBit)
	{
		TestFlagBit=flagBit;
		return ;
	}

	static int getTestFlagBit()
	{
		return TestFlagBit;
	}
    enum
	{
		DATA=0x520139,
		RESULT,
		END
	};
    enum
	{
    	BOTTOMTEST=0x100000,
		TOPTEST,
		NETWORKINGTEST
	};
    void sendJSON(int data, char ch[]);
    virtual int test(int fd){return 0;}
    void setFd(int i);
    CTestBase();
protected:
	int m_iFd;
private:
	static int TestFlagBit;
};




#endif // CTESTBASE_H
