/*
 * cTestAD.cpp
 *
 *  Created on: 2018年7月19日
 *      Author: tarena
 */

#include "cTestAD.h"

static const char *s_addev = "/dev/ttyS5";
//不需要数据
static unsigned char crc_array[256] = {
0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83,
0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e,
0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0,
0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d,
0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5,
0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58,
0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6,
0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b,
0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f,
0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92,
0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c,
0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1,
0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49,
0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4,
0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a,
0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7,
0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};

CTestAD::CTestAD() {
	// TODO Auto-generated constructor stub

}

CTestAD::~CTestAD() {
	// TODO Auto-generated destructor stub
}

//AD测试
int CTestAD::testAd()
{
	char buffer[1024]={0};
	//打开文件
	int fd = open(s_addev, O_RDWR|O_NOCTTY|O_NDELAY);
	if (0 == fd)
	{
		printf("ad dev can not open %s\n", s_addev);
		sprintf(buffer,"ad dev can not open %s",s_addev);
		sendJSON(DATA,buffer);
		return -1;
	}
	//设置串口文件
	if (!setseriopt(fd))
	{
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"set seriopt error");
		sendJSON(DATA,buffer);
		return -1;
	}

	//编写指令
	printf("sync now time to stm32\r\n");
	vector<char> buf;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	buf.push_back(0x68);
	buf.push_back(0x9);
	buf.push_back(0x68);
	buf.push_back(0x01);
	buf.push_back(tv.tv_sec);
	buf.push_back(tv.tv_sec >> 8);
	buf.push_back(tv.tv_sec >> 16);
	buf.push_back(tv.tv_sec >> 24);
	buf.push_back(tv.tv_usec);
	buf.push_back(tv.tv_usec >> 8);
	buf.push_back(tv.tv_usec >> 16);
	buf.push_back(tv.tv_usec >> 24);
	int cs = 0;
	//为什么要把所以值都加进去
	for (unsigned int i = 0; i != buf.size(); i++)
	{
		cs += buf[i];
	}
	buf.push_back(cs % 256);
	buf.push_back(0x16);
	//把指令写进去
	unsigned int wrSize = write(fd, &buf[0], buf.size());
	if (wrSize != buf.size())
	{
		printf("ad uart write error, wrsize is %d, errno is %d", wrSize, errno);
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"ad uart write error, wrsize is %d, errno is %d", wrSize, errno);
		sendJSON(DATA,buffer);
		close(fd);
		return -1;
	}
	usleep(100 * 1000);

	printf("begin to read ad data\r\n");
	char readTmp[ONE_WAVE_CNT] = {0};
	for(int i=0;i<10;)
	{
		int flag=0;
		int ret=spicomm_read(readTmp, sizeof(readTmp));
		if (ret != 0)
		{
			printf("ad spicomm_read error, read return is %d\r\n", ret);
			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"ad spicomm_read error, read return is  %d\r\n", ret);
			sendJSON(DATA,buffer);
			return -1;
		}
		bool adValid = false;
		for (int j = 0; j != 20; j++)
		{
			if (readTmp[j] != 0xFF)
			{
				adValid = true;
				break;
			}
		}
		if (!adValid)
		{
			flag++;
			usleep(5 * 1000);
			continue;
		}
		if(flag==10)
		{
			printf("AD 10 times unable to read data \n");
			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"AD 10 times unable to read data \n");
			sendJSON(DATA,buffer);
			return -1;
		}
		i++;
	}

	string adTmp;
	char strTmp[20] = {0};
	//读取数据
	int Flag=0;
	memset(buffer,0,sizeof(buffer));
	strcpy(buffer,"time : UA    UB   UC   IA   IB   IC");
	sendJSON(DATA,buffer);
	while(Flag<10)
	{
		//c从串口读取数据吗？
		int ret = spicomm_read(readTmp, sizeof(readTmp));
		if (ret != 0)
		{
			printf("ad spicomm_read error, read return is %d\r\n", ret);
			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"ad spicomm_read error, read return is %d\r\n", ret);
			sendJSON(DATA,buffer);
			return -1;
		}

		bool adValid = false;
		for (int j = 0; j != 20; j++)
		{
			if (readTmp[j] != 0xFF)
			{
				adValid = true;
				break;
			}
		}
		if (!adValid)
		{
			usleep(5 * 1000);
			continue;
		}

		//解析与拆分返回来的数据
		for(int i=0;i<AD_SAMPLE;i++)
		{
			//ua
			int dataBit=0;
			int tmp = (readTmp[i*18+dataBit] << 16) | (readTmp[i*18+dataBit+1] << 8) | (readTmp[i*18+dataBit+2]);
			if ((readTmp[i*18+dataBit] & 0xFC) != 0)
			{
				// 负数，计算补码
				tmp = ~tmp;
				tmp &= 0x00FFFFFF;
				tmp += 1;
				tmp = -tmp;
			}
			sprintf(strTmp, " IA:%d ", tmp);
			adTmp += strTmp;

			m_strAdAata[Flag].ia[i]=tmp*CURRENTRATIO;
			dataBit+=3;
			//ib
			tmp = (readTmp[i*18+dataBit] << 16) | (readTmp[i*18+dataBit+1] << 8) | (readTmp[i*18+dataBit+2]);
			if ((readTmp[i*18+dataBit] & 0xFC) != 0)
			{
				// 负数，计算补码
				tmp = ~tmp;
				tmp &= 0x00FFFFFF;
				tmp += 1;
				tmp = -tmp;
			}
			m_strAdAata[Flag].ib[i]=tmp*CURRENTRATIO;
			dataBit+=3;
			//ic
			tmp = (readTmp[i*18+dataBit] << 16) | (readTmp[i*18+dataBit+1] << 8) | (readTmp[i*18+dataBit+2]);
			if ((readTmp[i*18+dataBit] & 0xFC) != 0)
			{
				// 负数，计算补码
				tmp = ~tmp;
				tmp &= 0x00FFFFFF;
				tmp += 1;
				tmp = -tmp;
			}
			m_strAdAata[Flag].ic[i]=tmp*CURRENTRATIO;
			dataBit+=3;
			//ia
			tmp = (readTmp[i*18+dataBit] << 16) | (readTmp[i*18+dataBit+1] << 8) | (readTmp[i*18+dataBit+2]);
			if ((readTmp[i*18+dataBit] & 0xFC) != 0)
			{
				// 负数，计算补码
				tmp = ~tmp;
				tmp &= 0x00FFFFFF;
				tmp += 1;
				tmp = -tmp;
			}
			sprintf(strTmp, " UA:%d ", tmp);
			adTmp += strTmp;
			m_strAdAata[Flag].ua[i]=tmp*VOLTAGERATIO;
			dataBit+=3;
			//ib
			tmp = (readTmp[i*18+dataBit] << 16) | (readTmp[i*18+dataBit+1] << 8) | (readTmp[i*18+dataBit+2]);
			if ((readTmp[i*18+dataBit] & 0xFC) != 0)
			{
				// 负数，计算补码
				tmp = ~tmp;
				tmp &= 0x00FFFFFF;
				tmp += 1;
				tmp = -tmp;
			}
			m_strAdAata[Flag].ub[i]=tmp*VOLTAGERATIO;

			dataBit+=3;
			//ic
			tmp = (readTmp[i*18+dataBit] << 16) | (readTmp[i*18+dataBit+1] << 8) | (readTmp[i*18+dataBit+2]);
			if ((readTmp[i*18+dataBit] & 0xFC) != 0)
			{
				// 负数，计算补码
				tmp = ~tmp;
				tmp &= 0x00FFFFFF;
				tmp += 1;
				tmp = -tmp;
			}
			m_strAdAata[Flag].uc[i]=tmp*VOLTAGERATIO;
		}
		//配置成数据发送
		double ua,ub,uc,ia,ib,ic;
		ua=ub=uc=ia=ib=ic=0;
		for(int j=0;j<128;j++)
		{
			ua+=m_strAdAata[Flag].ua[j]*m_strAdAata[Flag].ua[j];
			ub+=m_strAdAata[Flag].ub[j]*m_strAdAata[Flag].ub[j];
			uc+=m_strAdAata[Flag].uc[j]*m_strAdAata[Flag].uc[j];
			ia+=m_strAdAata[Flag].ia[j]*m_strAdAata[Flag].ia[j];
			ib+=m_strAdAata[Flag].ib[j]*m_strAdAata[Flag].ib[j];
			ic+=m_strAdAata[Flag].ic[j]*m_strAdAata[Flag].ic[j];
		}
		ua/=(AD_SAMPLE);
		ub/=(AD_SAMPLE);
		uc/=(AD_SAMPLE);
		ia/=(AD_SAMPLE);
		ib/=(AD_SAMPLE);
		ic/=(AD_SAMPLE);
		ua=sqrt(ua);
		ub=sqrt(ub);
		uc=sqrt(uc);
		ia=sqrt(ia);
		ib=sqrt(ib);
		ic=sqrt(ic);
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"%d : %f %f %f %f %f %f",Flag,ua,ub,uc,ia,ib,ic);
		printf("AD data:\n%s\n",buffer);
		sendJSON(DATA,buffer);
		Flag++;
	}
	close(fd);
	return 0;
}


//计算开方均根参数得到值
int CTestAD::analyticData()
{
	double ua,ub,uc,ia,ib,ic;
	ua=ub=uc=ia=ib=ic=0;
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<AD_SAMPLE;j++)
		{
			ua+=m_strAdAata[i].ua[j]*m_strAdAata[i].ua[j];
			ub+=m_strAdAata[i].ub[j]*m_strAdAata[i].ub[j];
			uc+=m_strAdAata[i].uc[j]*m_strAdAata[i].uc[j];
			ia+=m_strAdAata[i].ia[j]*m_strAdAata[i].ia[j];
			ib+=m_strAdAata[i].ib[j]*m_strAdAata[i].ib[j];
			ic+=m_strAdAata[i].ic[j]*m_strAdAata[i].ic[j];
		}
	}
	ua/=(AD_SAMPLE*10);
	ub/=(AD_SAMPLE*10);
	uc/=(AD_SAMPLE*10);
	ia/=(AD_SAMPLE*10);
	ib/=(AD_SAMPLE*10);
	ic/=(AD_SAMPLE*10);
	ua=sqrt(ua);
	ub=sqrt(ub);
	uc=sqrt(uc);
	ia=sqrt(ia);
	ib=sqrt(ib);
	ic=sqrt(ic);
	char buf[1024]={0};
	sprintf(buf,"\nUA:%f  UB:%f  UC:%f\nIA:%f  IB:%f  IC:%f",ua,ub,uc,ia,ib,ic);
	sendJSON(DATA,buf);
	printf("%s\n",buf);
	if((ua>200.0)&&(ua<240)&&(ub>200.0)&&(ub<240)&&(uc>200.0)&&(uc<240))
	{
		return 0;
	}
	else
	{
		return -1;
	}

}

//AD测试控制和判断
int CTestAD::test(int sockfd)
{
	char buffer[1024];
	printf("Enter the AD test\n");
	setFd(sockfd);
	int Flag=0;
	if(testAd()!=0)
	{
		Flag=-1;
	}
	else
	{
		if(analyticData()!=0)
		Flag=-1;
	}

	if(Flag!=0)
	{
		memset(buffer,0,sizeof(buffer));
		strcpy(buffer,"Failure:AD test ");
		sendJSON(RESULT,buffer);
		memset(buffer,0,sizeof(buffer));
		strcpy(buffer,"AD test end");
		sendJSON(END,buffer);
		return -1;
	}
	else
	{
		memset(buffer,0,sizeof(buffer));
		strcpy(buffer,"Success:AD test");
		sendJSON(RESULT,buffer);
		memset(buffer,0,sizeof(buffer));
		strcpy(buffer,"AD test end");
		sendJSON(END,buffer);
		return 0;
	}
}

//没实现的功能，不需要了解
unsigned char CTestAD::CRC8_Table(unsigned char *p, int counter)
{
	unsigned char crc8 = 0;
	for( ; counter > 0; counter--)
	{
		crc8 = crc_array[crc8^*p]; //查表得到CRC码
		p++;
	}
	return crc8;
}

//设置串口文件
bool CTestAD::setseriopt(int fd)
{
	struct termios options;
	if (tcgetattr(fd, &options) != 0)
	{
		printf("ad dev can not get serial options \r\n");
		return false;
	}
	// 设置波特率
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);

	options.c_cflag |= CLOCAL | CREAD;

	// 设置数据位
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	// 设置偶校验位
	options.c_cflag |= PARENB;
	options.c_cflag &= ~PARODD;

	// 设置停止位
	options.c_cflag &= ~CSTOPB;

	// 修改输出模式，原始数据输出
	options.c_oflag &= ~OPOST;
	options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	// 设置等待时间和最小接收字符
//	options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */
//	options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */
	// 如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读
	tcflush(fd,TCIFLUSH);

	if (tcsetattr(fd, TCSANOW, &options) != 0)
	{
		printf("ad dev can not set serial options \r\n");
		return false;
	}
	return true;
}




