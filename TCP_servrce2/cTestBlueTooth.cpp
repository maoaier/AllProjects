/*
 * cTestBlueTooth.cpp
 *
 *  Created on: 2018年7月23日
 *      Author: tarena
 */
using namespace std;
#include "cTestBlueTooth.h"
static const char *s_gBlueToothDev="/dev/ttyS4";

CHub::CHub() {
	// TODO Auto-generated constructor stub
	m_iFd=0;
}

CHub::~CHub() {
	// TODO Auto-generated destructor stub
}

//测试控制和结果判断处理
int CHub::test(int fd)
{
	char buffer[1024]={0};
	setFd(fd);
	int flag=0;
	flag=blueToothTest();
	if(flag==0)
	{
		strcpy(buffer,"Bluetooth test successful");
		sendJSON(RESULT,buffer);
		memset(buffer,0,sizeof(buffer));
		strcpy(buffer,"BlueTooth test END");
		sendJSON(END,buffer);
		return 0;
	}
	else
	{
		strcpy(buffer,"Bluetooth test failed");
		sendJSON(RESULT,buffer);
		memset(buffer,0,sizeof(buffer));
		strcpy(buffer,"BlueTooth test END");
		sendJSON(END,buffer);
		return -1;
	}

}

//蓝牙测试，打开蓝牙，发送指令，接收指令
int CHub::blueToothTest()
{
	char buffer[1024]={0};
	//打开文件
	m_iFd = open("/dev/ttyS4", O_RDWR|O_NOCTTY|O_NDELAY);
	if (m_iFd<1)
	{
		sprintf(buffer,"ad dev can not open %s",s_gBlueToothDev);
		sendJSON(DATA,buffer);
		closeBlue();
		return -1;
	}
	_setGpioValue(GPIO_BT_POWER,1);
	//让蓝牙进入指令模式

	usleep(1000*200);
	_setGpioValue(GPIO_BT_KEY,1);
	usleep(1000*200);
	_setGpioValue(GPIO_BT_POWER,0);
	usleep(1000*200);
	//设置串口
	if (!SetSeriOpt(m_iFd))
	{
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"set seriopt error");
		sendJSON(DATA,buffer);
		close(m_iFd);
		closeBlue();
		return -1;
	}
	usleep(1000*3000);
	char buf[1024];
	memset(buf,0,sizeof(buf));
	sprintf(buf,"AT+VERSION?\r\n");
	//发送指令
	int wrdata=strlen(buf);
	int wrSize = write(m_iFd, buf, strlen(buf));
	if (wrSize != wrdata)
	{
		//printf("ad uart write error, wrsize is %d, errno is %d", wrSize, errno);
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"ad uart write error, wrsize is %d, errno is %d", wrSize, errno);
		sendJSON(DATA,buffer);
		close(m_iFd);
		closeBlue();
		return -1;
	}
	sleep(2);
	memset(buf,0,sizeof(buf));
	string res;
	char recBuf[1024] = { 0 };
	unsigned long readCnt = 0;
	//接收数据
	while (ReadData(recBuf, 1024, readCnt))
	{
		for (unsigned long i = 0; i != readCnt; i++)
			res += recBuf[i];
	}
	cout<<"\nres= "<<res<<endl;
	if(res.find("OK")==string::npos)
	{
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"read data error");
		sendJSON(DATA,buffer);
		closeBlue();
		return -1;
	}
	const char *data=res.data();
	memset(buf,0,sizeof(buf));
	strcpy(buf,data);
	sendJSON(DATA,buf);

	//第二次发送指令s
	memset(buf,0,sizeof(buf));
	sprintf(buf,"AT+NAME?\r\n");
	wrdata=strlen(buf);
	wrSize = write(m_iFd, buf, strlen(buf));
	if (wrSize != wrdata)
	{
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"ad uart write error, wrsize is %d, errno is %d", wrSize, errno);
		sendJSON(DATA,buffer);
		close(m_iFd);
		closeBlue();
		return -1;
	}
	sleep(1);
	memset(buf,0,sizeof(buf));
	string res1;
	memset(recBuf,0,sizeof(recBuf));
	readCnt = 0;
	while (ReadData(recBuf, 1024, readCnt))
	{
		for (unsigned long i = 0; i != readCnt; i++)
			res1 += recBuf[i];
	}
	cout<<"res1= "<<res1<<endl;
	if(res1.find("OK")==string::npos)
	{
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"read data error");
		sendJSON(DATA,buffer);
		closeBlue();
		return -1;
	}
	const char *data1=res1.data();
	memset(buf,0,sizeof(buf));
	strcpy(buf,data1);
	sendJSON(DATA,buf);
	closeBlue();
	return 0;
}
//读蓝牙数据
bool CHub::ReadData(char *buf, unsigned long bufSize, unsigned long &readSize)
{
	fd_set fs_read;
	struct timeval time;
	FD_ZERO(&fs_read);
	FD_SET(m_iFd, &fs_read);
	memset(buf, 0, bufSize);

	time.tv_sec = 1;
	time.tv_usec = 0;
	readSize = 0;

	//使用select实现串口的多路通信
	int fs_sel = select(m_iFd + 1, &fs_read, NULL, NULL, &time);
	printf("fs_sel==%d %d\n",fs_sel,m_iFd);
	if(fs_sel)
	{
		int rdSize = read(m_iFd, buf, bufSize);
		printf("161buf==%s\n",buf);
		if (rdSize < 0)
			return false;
		readSize = rdSize;
		//DebugLog("bluetooth read data %s", buf);
	}
	else
	{
		//ErrorLog("bluetooth read data error");
		return false;
	}
	return true;
}

//关闭蓝牙
void CHub::closeBlue()
{
	_setGpioValue(GPIO_BT_POWER,GPIO_BT_CLOSE);
	_setGpioValue(GPIO_BT_KEY,0);
}

//设置GPIO口的值
void CHub::_setGpioValue(int gpio,int value)
{
	gpio_register(gpio, GPIO_MODE_OUTPUT);
	usleep(200);
	gpio_set_value(gpio, value);
	usleep(200);
	gpio_unregister(gpio);
	return ;
}

//读取GPIO口的值
int CHub::_readGpioState(int gpio)
{
	gpio_register(gpio,GPIO_MODE_OUTPUT);
	char buf[1024];
	memset(buf,0,sizeof(buf));
	int value;
	value=gpio_get_value(gpio);
	sprintf(buf,"Read gpio %d initial value: %d ", gpio,value);
	sendJSON(DATA,buf);
	gpio_unregister(gpio);
	return value;
}

//配置串口
bool CHub::SetSeriOpt(int fd)
{
	printf("cTestBlueTooth::SetSeriOpt218\n");
	int byteSize=8;
	struct termios options;
	if (tcgetattr(fd, &options) != 0)
	{
		//ErrorLog("bluetooth can not get serial options");
		return false;
	}
	// 设置波特率
	cfsetispeed(&options, B38400);
	cfsetospeed(&options, B38400);
	// 修改输出模式，原始数据输出
	options.c_cflag |= CLOCAL | CREAD;
	options.c_oflag &= ~OPOST;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

	// 设置数据位
	map <int, int> byteMap;
	byteMap[5] = CS5;
	byteMap[6] = CS6;
	byteMap[7] = CS7;
	byteMap[8] = CS8;
	if (byteMap.find(byteSize) == byteMap.end())
	{
		//ErrorLog("bluetooth byteSize = %d error", 8);
		return false;
	}
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= byteMap[byteSize];

	// 设置校验位
	options.c_cflag &= ~PARENB;
	options.c_iflag &= ~INPCK;


	// 设置停止位
	options.c_cflag &= ~CSTOPB;
	// 设置等待时间和最小接收字符
//	options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */
//	options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */
	// 如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读
	tcflush(fd,TCIFLUSH);

	if (tcsetattr(fd, TCSANOW, &options) != 0)
	{
		return false;
	}
	return true;
}

