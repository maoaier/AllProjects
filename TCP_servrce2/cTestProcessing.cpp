#include "cTestProcessing.h"

CTestProcessing::CTestProcessing()
{

	m_cTeDO.initGPIO();
	m_iDOFlag=0;
}

void CTestProcessing::testProcess()
{
	if(m_iDOFlag!=0)
	{
		m_cTeDO.initGPIO();
		m_iDOFlag==0;
	}
}
//解析测试指令，并执行相应的测试函数
int CTestProcessing::parsingJSON()
{
	testProcess();
	char buffer[1024];
	char buf[1024];
	memset(buffer, 0 ,sizeof(buffer));
	int len = recv(m_iFd, buffer, sizeof(buffer), 0);
	if(strcmp(buffer, "exit\n") == 0)
		return -2;
	printf("%s\n", buffer);
    cJSON* pJson = cJSON_Parse(buffer);
    if(pJson == NULL)
    {
    	printf("buffer=%s\n",buffer);
        printf("Json Parse failed.\n");
        return -1;
    }
    cJSON* pSub1 = cJSON_GetObjectItem(pJson,"DOTEST");
    //cJSON* pSub2 = cJSON_GetObjectItem(pJson,"ELECTRICCURRENT");
    cJSON* pSub3 = cJSON_GetObjectItem(pJson,"BLUETOOTH");
    cJSON* pSub4 = cJSON_GetObjectItem(pJson,"DITEST");
    cJSON* pSub5 = cJSON_GetObjectItem(pJson,"ADCURRENTANDVOLTAGE");
    cJSON* pSub6 = cJSON_GetObjectItem(pJson,"MODULE4G");
    cJSON* pSub7 = cJSON_GetObjectItem(pJson,"SDCARD");
    cJSON* pSub8 = cJSON_GetObjectItem(pJson,"NETWORKINTERFACE");
    CTestBase *testbase;
    if(pSub1!=NULL)
    {
		printf("DOTEST : %d\n", pSub1->valueint);
		strcpy(buf,"DOTEST start......");
		sendJSON(DATA,buf);
		testbase=new testDo;

//		testDo TeDO;
//		TeDO.test(m_iFd);
		m_iDOFlag=1;
    }
//    if(pSub2!=NULL)
//    {
//      printf("ELECTRICCURRENT : %d\n", pSub2->valueint);
//      strcpy(buf,"ELECTRICCURRENT start......");
//      sendJSON(DATA,buf);
//    }
    if(pSub3!=NULL)
    {
		printf("BLUETOOTH : %d\n", pSub3->valueint);
		strcpy(buf,"BLUETOOTH start......");
		sendJSON(DATA,buf);
		testbase=new CHub;
//		cTestBlueTooth aBluetooth;
//		int btFd=aBluetooth.test(m_iFd);

    }
    if(pSub4!=NULL)
    {
		printf("DITEST : %d\n", pSub4->valueint);
		strcpy(buf,"DOTEST start......");
		sendJSON(DATA,buf);
		testbase=new CTestDI;
//		CTestDI ctdi;
//		ctdi.test(m_iFd);

    }
    if(pSub5!=NULL)
    {
		printf("ADCURRENTANDVOLTAGE : %d\n", pSub5->valueint);
		strcpy(buf,"ADCURRENTANDVOLTAGE start......");
		sendJSON(DATA,buf);
		testbase=new CTestAD;
//		cTestAD testad;
//		testad.test(m_iFd);
    }
    if(pSub6!=NULL)
    {
		printf("MODULE4G : %d\n", pSub6->valueint);
		strcpy(buf,"MODULE4G start......");
		sendJSON(DATA,buf);
		memset(buf,0,sizeof(buf));
		strcpy(buf,"It takes a long time to wait for......");
		sendJSON(DATA,buf);
		testbase=new CTest4G;
    }
    if(pSub7!=NULL)
    {
		printf("SDCARD : %d\n", pSub7->valueint);
		strcpy(buf,"SDCARD start......");
		sendJSON(DATA,buf);
		testbase=new CTestSDCard;
//		CardSD sd;
//		sd.test(m_iFd);

    }
    if(pSub8!=NULL)
    {
		printf("NETWORKINTERFACE : %d\n", pSub8->valueint);
		strcpy(buf,"NETWORKINTERFACE start......");
		sendJSON(DATA,buf);
		testbase=new CTestNetworkInterface;
//		cTestNetworkInterface cNetwork;
//		cNetwork.test(m_iFd);
    }
    testbase->test(m_iFd);
    delete testbase;
    return 0;
}

