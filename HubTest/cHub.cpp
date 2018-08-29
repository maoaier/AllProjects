
/*
 * cTestBlueTooth.cpp
 *
 *  Created on: 2018��7��23��
 *      Author: tarena
 */
using namespace std;
#include "cHub.h"
static const char *s_gBlueToothDev="/dev/ttyS4";

CHub::CHub()
{
	m_iFd=0;
}

CHub::~CHub()
{
	//close(m_iFd);
}

int CHub::connectivity(unsigned char dataBuf[],int needWrite)
{
	int wrSize = write(m_iFd, dataBuf,needWrite);
	if (wrSize != needWrite)
	{
		perror("write error");
		close(m_iFd);
		return -1;
	}
	cout<<"wrSize="<<wrSize<<endl;;
	//����ָ��
	usleep(1000*300);
	string res;
	unsigned char recBuf[1024] = { 0 };
	unsigned char readData[1024]={ 0 };
	unsigned long readCnt = 0;
	//��������

	unsigned long i;
	int num=0;
	while (ReadData(recBuf, 1024, readCnt))
	{
		for (i = 0; i != readCnt; i++)
		{
			res += recBuf[i];
			printf("0x%x ",recBuf[i]);
			readData[num]=recBuf[i];
			num++;
		}
	}
	//cout<<"num="<<num;
	//cout<<"\nres= "<<res<<endl;
	if(num==5)
	{
		int i;
		for(i=0;i<5;i++)
		{
			if(readData[i]!=dataBuf[i])
				break;
		}
		if(i==5)
		{
			printf("successful communication\n");
			return 0;
		}
		else
		{
			printf("failure communication\n");
			return -1;
		}
	}
	return -1;
}


//�������ԣ�������������ָ�����ָ��
int CHub::hubTest()
{
	//���ļ�
	m_iFd = open("/dev/ttyS4", O_RDWR|O_NOCTTY|O_NDELAY);
	if (m_iFd<1)
	{
		printf("can not open %s decive file\n",s_gBlueToothDev);
		return -1;
	}
	//���ô���
	if (!SetSeriOpt(m_iFd))
	{
		printf("set seriopt error");
		close(m_iFd);
		return -1;
	}
	cout<<"m_iFd"<<m_iFd<<endl;;
	usleep(1000*200);
	unsigned int number=0x00;
	unsigned char dataBuf[]={0x68,0x00,0x00,0x68,0x16};
	int correctFalg=0;
	Mo_Led aLed;
	aLed.setAllBlack();
	for(int j=17;j<=32;j++)
	{
		printf("please input 485 number\n");
		//scanf("%u",&number);
		number++;
		if(number>32)
			break;
		//dataBuf[1]=number;
		dataBuf[2]=j;
		//printf("dataBuf[2]=%x",dataBuf[2]);
		dataBuf[3]=(dataBuf[0]+dataBuf[2])&0xFF;
		int needWrite=5;
		int value=connectivity(dataBuf,needWrite);
		if(0!=value)
		{
			value=connectivity(dataBuf,needWrite);
		}
		if(0==value)
		{
			printf("Equipment number %d is Yes\n",j);
		}
		else
		{
			correctFalg=-1;
			printf("Equipment number %d is NO\n",j);

		}
		if((10==number)||(20==number)||(32==number))
		{
			if(0==correctFalg)
			{
				switch(number)
				{
					case 10:
						aLed.firstLed();
						break;
					case 20:
						aLed.twoLed();
						break;
					case 32:
						aLed.threeLed();
						break;
					default:
						break;
				}
			}
			correctFalg=0;
		}
	}
	//�ڶ��η���ָ��s
	close(m_iFd);
	return 0;
}


//����������
bool CHub::ReadData(unsigned char *buf, unsigned long bufSize, unsigned long &readSize)
{
	fd_set fs_read;
	struct timeval time;
	FD_ZERO(&fs_read);
	FD_SET(m_iFd, &fs_read);
	memset(buf, 0, bufSize);

	time.tv_sec = 0;
	time.tv_usec = 1000*500;
	readSize = 0;

	//ʹ��selectʵ�ִ��ڵĶ�·ͨ��
	int fs_sel = select(m_iFd + 1, &fs_read, NULL, NULL, &time);
	printf("fs_sel==%d %d\n",fs_sel,m_iFd);
	if(fs_sel)
	{
		int rdSize = read(m_iFd, buf, bufSize);
		if (rdSize < 0)
			return false;
		readSize = rdSize;
	}
	else
	{
		return false;
	}
	return true;
}





//���ô���
bool CHub::SetSeriOpt(int fd)
{
	struct termios options;
	if (tcgetattr(fd, &options) != 0)
	{
		//ErrorLog("bluetooth can not get serial options");
		return false;
	}
	// ���ò�����
	cfsetispeed(&options, B9600);
	cfsetospeed(&options, B9600);

	// �޸����ģʽ��ԭʼ�������
	options.c_cflag |= CLOCAL | CREAD;
	options.c_oflag &= ~OPOST;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	// ��������λ
	// ����У��λ
	// ����ֹͣλ
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	// ���õȴ�ʱ�����С�����ַ�
//	options.c_cc[VTIME] = 1; /* ��ȡһ���ַ��ȴ�1*(1/10)s */
//	options.c_cc[VMIN] = 1; /* ��ȡ�ַ������ٸ���Ϊ1 */
	// �����������������������ݣ����ǲ��ٶ�ȡ ˢ���յ������ݵ��ǲ���
	tcflush(fd,TCIFLUSH);

	if (tcsetattr(fd, TCSANOW, &options) != 0)
	{
		return false;
	}
	return true;
}

