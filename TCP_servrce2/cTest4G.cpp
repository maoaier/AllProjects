/*
 * cTest4G.cpp
 *
 *  Created on: 2018年7月25日
 *      Author: tarena
 */
#include <string>
#include "cTest4G.h"
#define PORT 9000
#define IP "10.124.140.211"

#ifdef __cplusplus
extern "C"
{
	// libled.a函数
	int gpio_register(size_t gpio, int mode);
	int gpio_unregister(size_t gpio);
	int gpio_set_value(size_t gpio, int value);
	int gpio_get_value(size_t gpio);
}
#endif

using namespace std;

int CTest4G::m_Flag=-1;
//打开GPIO口
CTest4G::CTest4G() {
	// TODO Auto-generated constructor stub
	gpio_register(GPIO_4G_IGT, GPIO_MODE_OUTPUT);
	gpio_register(GPIO_4G_RST, GPIO_MODE_OUTPUT);
	m_reset = true;
}

//关闭GPIO口
CTest4G::~CTest4G() {
	gpio_unregister(GPIO_4G_IGT);
	gpio_unregister(GPIO_4G_RST);
	system("/mnt/xt/ppp_gprs_off.srt &");
}

//测试控制和结果判断处理
int CTest4G::test(int fd)
{
	char buf[1024]={0};
	strcpy(buf,"It takes a long time to wait for......");
	sendJSON(DATA,buf);
	setFd(fd);
	int flag=test4G();
	sleep(7);
	if(flag!=-1)
	{
		flag=readFile();
	}
	if(-1==flag)
	{
		CTest4G::m_Flag=-1;
		memset(buf,0,sizeof(buf));
		strcpy(buf,"Failure:4G test cannot connect remote IP ");
		printf("%s\n",buf);
		sendJSON(RESULT,buf);
		memset(buf,0,sizeof(buf));
		strcpy(buf,"4G test END");
		sendJSON(END,buf);
		return -1;
	}
	else
	{
		CTest4G::m_Flag=0;
		memset(buf,0,sizeof(buf));
		strcpy(buf,"Successful:4G test can connect remote IP ");
		printf("%s\n",buf);
		sendJSON(RESULT,buf);
		memset(buf,0,sizeof(buf));
		strcpy(buf,"4G test END");
		sendJSON(END,buf);
		return 0;
	}


}

//socket客服端用来连接内网判断4G模块的状态
int CTest4G::sockClient()
{
	//1.create socket
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
	{
		perror("socket");
		return -1;
	}

	//2.prepare address
	struct sockaddr_in saddr;
	bzero(&saddr,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT);
	saddr.sin_addr.s_addr = inet_addr(IP);
	//3.connect
	if( connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr)) < 0 )
	{
		char buf[1024]={0};

		sprintf(buf,"%d %s",errno ,strerror(errno));
		sendJSON(DATA,buf);
		if(errno==111)
		{
			perror("connect failure");
			return 1;
		}
		perror("connect failure");
		close(sockfd);
		return -1;
	}
	printf("Connection success\n");
	close(sockfd);
	return 1;
}

int CTest4G::readFile()
{
	char buf[1024];
	memset(buf,0,sizeof(buf));
	int fd = open("/mnt/xt/test4G.txt", O_RDWR);
	if( -1 == fd )
	{
		printf("文件打开失败,错误号:%d\n", errno);
		perror( "open" );
		sprintf(buf,"Failure to read open a file.System IO errer  %s",strerror(errno));
		sendJSON(DATA,buf);
		return -1;
	}
	char *fileData=(char *)malloc(1024*1024);
	memset(fileData,0,1024*1024);
	int count;
	count = read( fd,fileData,1024*1024);
	if ( -1 == count )
	{
		printf("文件写入失败\n");
		sprintf(buf,"File read failure %s",strerror(errno));
		sendJSON(DATA,buf);
		free(fileData);
		close( fd );
		return -1;
	}
	string str(fileData);
	if(str.find("IP address")==string::npos)
	{
		memset(buf,0,sizeof(buf));
		strcpy(buf,"No direction IP address");
		sendJSON(DATA,buf);
		free(fileData);
		close( fd );
		return -1;
	}

	printf("count=%d %s",count,fileData);
	free(fileData);
	close( fd );
	return 0;
}



//4G测试函数，用来配置4G参数
int CTest4G::test4G()
{

	char buf[1024];
	// 4G模块断电，上电
	gpio_set_value(GPIO_4G_RST, 0);
	SleepTime(5 * 1000);
	gpio_set_value(GPIO_4G_IGT, 1);
	SleepTime(100);
	gpio_set_value(GPIO_4G_RST, 1);
	SleepTime(100);

	// 4G模块开机信号
	gpio_set_value(GPIO_4G_IGT, 0);
	SleepTime(150);
	gpio_set_value(GPIO_4G_IGT, 1);
	SleepTime(30 * 1000);

	// 打开4g设备
	int fd = 0, i = 3;
	char tmp[250] = { 0 };
	for (; i != 100; i++)
	{
		sprintf(tmp, "/dev/ttyUSB%d", i);
		fd = open(tmp, O_RDWR | O_NOCTTY | O_NDELAY);
		if (fd > 0)
		{
			close(fd);
			break;
		}
	}
	if (i == 100)
	{
		printf("4G module has been rebooted 100 times");
		memset(buf,0,sizeof(buf));
		strcpy(buf,"4G module has been rebooted 100 times,Please check 4G to retest");
		sendJSON(DATA,buf);
		SleepTime(1000);
		return -1;
		//system("reboot");
	}

	printf("/dev/ttyUSB%d is \n",i);
	string apn="apn";
	string apn_user="apn_user";
	string apn_pw="apn_pw";
	printf("\n%d :%s\n",__LINE__,buf);
	usleep(50*1000);
	//运行脚本，配置4G坏境./ppp_gprs_on.srt

	sprintf(tmp, "/mnt/xt/ppp_gprs_on.srt %d 115200 [Default] %s %s %s > /mnt/xt/test4G.txt &",
			i,apn.c_str(),apn_user.c_str(),apn_pw.c_str());
	system(tmp);//>! /mnt/xt/test4G.txt
	sleep(1);//IP address
	//sendJSON(DATA,tmp);
	memset(buf,0,sizeof(buf));
	strcpy(buf,"Wait for the end");
	sendJSON(DATA,buf);
	printf("\n%d: %s\n",__LINE__,buf);
	return 0;
}

//延迟函数
void CTest4G::SleepTime(long ms)
{
	struct timeval tv;
	tv.tv_sec = ms / 1000;
	tv.tv_usec = (ms % 1000) * 1000;
	select(0, NULL, NULL, NULL, &tv);
	//usleep(ms * 1000);
}


