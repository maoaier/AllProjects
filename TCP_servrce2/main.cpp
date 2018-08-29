#include "main.h"
#include <stdio.h>
#include "cTestBase.h"
using namespace std;

enum
{
	BOTTOMTEST=0x100000,
	TOPTEST,
	NETWORKINGTEST
};



int main()
{
	//CTestBase aTestBase;
	//CTestBase::setTestFlagBit(BOTTOMTEST);
	Mo_Led aLed;
	aLed.setAllBlack();
	int stateFalg=0;
	printf("ps500 test start\n");
	int fd = open(FILENAME, O_RDWR);
	if( -1 == fd )
	{
		printf("文件打开失败,错误号:%d\n", errno);
		perror( "open" );
		return -1;
	}

	char fileData[2048];
	memset(fileData,0,2048);
	int count = read( fd,fileData,2048);
	if ( -1 == count )
	{
		printf("读文件文件失败 %s %d\n",__FILE__,__LINE__);
		//sleep(1);
		close( fd );
		return -1;
	}
	//printf("count=%d %s\n",count,fileData);
	cJSON* pJson = cJSON_Parse(fileData);
	if(pJson == NULL)
	{
		printf("fileData=%s\n",fileData);
		printf("Json Parse failed.\n");
		return -1;
	}
	cJSON* pSub1 = cJSON_GetObjectItem(pJson,"BOTTOMTEST");
	cJSON* pSub2 = cJSON_GetObjectItem(pJson,"TOPTEST");
	cJSON* pSub3 = cJSON_GetObjectItem(pJson,"NETWORKINGTEST");
	CTestBase *testbase;
	if(pSub1!=NULL)
	{
		CTestBase::setTestFlagBit(BOTTOMTEST);
		printf("DBOTTOMTEST : %s\n", pSub1->valuestring);
		testbase=new CTestAD;
		stateFalg=testbase->test(0);
		if(stateFalg==0)
		{
			printf("***********************************AD is right******************************\n");
			aLed.setCorrectLed();
		}
		else
		{
			printf("***********************************AD is wrong*****************************\n");
			aLed.setErrorLed();
		}
		delete testbase;
	}
	if(pSub2!=NULL)
	{
		int state=0;
		CTestBase::setTestFlagBit(TOPTEST);
		printf("TOPTEST : %s\n", pSub2->valuestring);

		//cTest4G
		testbase=new CTest4G;
		state=testbase->test(0);
		aLed.setAllBlack();
		sleep(1);
		if(state==0)
		{
			printf("\nCTest4G is right\n");
			aLed.setCorrectLed();
			delete testbase;

		}
		else
		{
			stateFalg=state;
			printf("\ncTest4G is wrong\n");
			aLed.setErrorLed();
			delete testbase;
		}

		testbase=new CTestSDCard;
		state=testbase->test(0);
		aLed.setAllBlack();
		sleep(1);
		if(state==0)
		{
			printf("\nSDCard is right\n");
			aLed.setCorrectLed();
			delete testbase;

		}
		else
		{
			stateFalg=state;
			printf("\nSDCard is wrong\n");
			aLed.setErrorLed();
			delete testbase;
			return 0;
		}

		//CTestBlueTooth
		testbase=new CHub;
		state=testbase->test(0);
		aLed.setAllBlack();
		sleep(1);
		if(state==0)
		{
			printf("\nBlueTooth is right\n");
			aLed.setCorrectLed();
			delete testbase;

		}
		else
		{
			stateFalg=state;
			printf("\nBlueTooth is wrong\n");
			aLed.setErrorLed();
			delete testbase;
		}

		//DItest
		testbase=new testDo;
		state=testbase->test(0);
		aLed.setAllBlack();
		sleep(1);
		if(state==0)
		{
			printf("\nCTestDo is right\n");
			aLed.setCorrectLed();
			delete testbase;

		}
		else
		{
			stateFalg=state;
			printf("\nCTestDo is wrong\n");
			aLed.setErrorLed();
			delete testbase;
		}

		//
		testbase=new CTestDI;
		state=testbase->test(0);
		aLed.setAllBlack();
		sleep(1);
		if(state!=0)
		{
			printf("\nCTestDI is right\n");
			aLed.setCorrectLed();
			delete testbase;

		}
		else
		{
			//stateFalg=state;
			printf("\nCTestDI is wrong\n");
			aLed.setErrorLed();
			delete testbase;
		}
	}
	if(pSub3!=NULL)
	{
		CTestBase::setTestFlagBit(NETWORKINGTEST);
		printf("NETWORKINGTEST : %d\n", pSub3->valueint);
		CTcpSock tcpserver;
		bool bo=tcpserver.sockInit(PORT);
		if(bo==false)
		{
			printf("sock run\n");
			return -1;
		}
		printf("sock run\n");
		tcpserver.sockRun();
		printf("end\n");
		return 0;
	}
	if(stateFalg==0)
	{
		system("cp /mnt/xt/rc.local /etc/init.d/rc.local");
		//system("reboot");
		aLed.setOpenAll();
		printf("ps500 test is success");
	}
	else
	{
		aLed.setEndLight();
	}
	printf("PS500 Test End\n");
}


