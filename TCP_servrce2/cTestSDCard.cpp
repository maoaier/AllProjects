/*
 * cardsd.cpp
 *
 *  Created on: 2018年7月16日
 *      Author: tarena
 */
#include "cTestSDCard.h"

CTestSDCard::CTestSDCard()
{

}

void CTestSDCard::CreateDir()
{
    char DirName[256];
    strcpy(DirName,"./SDtest");
    if(access(DirName,0)==-1)
    {
        if(mkdir(DirName,0777))
        {
            printf("creat file bag failed!!!");
        }
    }
    else
    {
    	system("rm -rf ./SDtest");
		if(mkdir(DirName,0777))
		{
			printf("creat file bag failed!!!");
		}
    }

}

//SD卡测试
int CTestSDCard::test(int fd)
{
	setFd(fd);
	char resultBuf[1024];
	memset(resultBuf,0,sizeof(resultBuf));
	char endBuf[1024];
	memset(endBuf,0,sizeof(endBuf));
    CreateDir();
    int in;
    int flag=0;
    in=createFile();
    if(in!=0)
    {
    	flag=-1;
    }else
    {
    	in=readFile();
    	if(in!=0)
    	{
    		flag=-1;
    	}
    	else
    	{
    		in=deleteFile();
    		if(in!=0)
    		{
    			flag=-1;
    		}
    	}
    }
    if(flag==-1)
    {
    	sprintf(resultBuf,"errer:Failure of SD card test");
		sendJSON(RESULT,resultBuf);
		printf("errer:Failure of SD card test\n");
		sprintf(endBuf,"SD card test end");
		sendJSON(END,endBuf);
		printf("SD test Success\n");
		return -1;
    }
    else
    {
    	sprintf(resultBuf,"succes:Success of SD card test");
		sendJSON(RESULT,resultBuf);
		printf("succes:Success of SD card test\n");
		sprintf(endBuf,"SD card test end");
		sendJSON(END,endBuf);
		printf("SD test Success\n");
		return 0;
    }

}

//读文件
int CTestSDCard::readFile()
{
	char fileName[520];
	char errerBuf[1024];
	memset(errerBuf,0,sizeof(errerBuf));
	memset(fileName,0,sizeof(fileName));
	for(int i=0;i<100;i++)
	{
		sprintf(fileName,"SDtest/%d.txt",i);
		int fd = -1;
		fd = open(fileName, O_RDWR);
		if( -1 == fd )
		{
			printf("文件打开失败,错误号:%d\n", errno);
			perror( "open" );
			sprintf(errerBuf,"Failure to read open a file.System IO errer  %s",strerror(errno));
			sendJSON(DATA,errerBuf);
			return -1;
		}
		char *fileData=(char *)malloc(1024*1024);
		memset(fileData,0,1024*1024);
		int needWrite=1024*1024;
		int alreadyRead=0;
		int count;
		while(1)
		{
			count = read( fd,fileData+alreadyRead,needWrite);
			if ( -1 == count ) {
				printf("文件写入失败\n");
				sprintf(errerBuf,"File read failure");
				sendJSON(DATA,errerBuf);
				//sleep(1);
				free(fileData);
				close( fd );
				return -1;
			}else {
				alreadyRead+=count;
				needWrite-=count;
				if(needWrite<1)
					break;
				//printf("文件写入成功，实际写入的字节数目为:%d\n", count);
			}
		}
		if(alreadyRead!=1024*1024)
		{
			sprintf(errerBuf,"File read number failure, number:%d",alreadyRead);
			sendJSON(DATA,errerBuf);
			//sleep(1);
			free(fileData);
			close( fd );
			return -1;
		}

		char *compareData=(char *)malloc(1024*1024);
		memset(compareData,48,1024*1024);
		if(strncmp(fileData,compareData,1024*1024)!=0)
		{
			sprintf(errerBuf,"File DATA failure file:%s",fileName);
			sendJSON(DATA,errerBuf);
			sleep(1);
			free(compareData);
			free(fileData);
			close( fd );
			return -1;
		}
		free(compareData);
		free(fileData);
		close( fd );

	}
	return 0;
}

//创建100个文件并写人1M的数据
int CTestSDCard::createFile()
{
	char fileName[520];
	char errerBuf[1024];
	memset(errerBuf,0,sizeof(errerBuf));
	memset(fileName,0,sizeof(fileName));
	for(int i=0;i<100;i++)
	{
		sprintf(fileName,"SDtest/%d.txt",i);
		int fd = -1;
		fd = open(fileName, O_RDWR | O_CREAT ,0777 );
		if( -1 == fd ) {
			printf("文件打开失败,错误号:%d\n", errno);
			perror( "open" );
			sprintf(errerBuf,"Failure to create a file.System IO errer code %s",strerror(errno));
			sendJSON(DATA,errerBuf);
			//sleep(1);
			return -1;
		}
		char *fileData=(char *)malloc(1024*1024);
		memset(fileData,48,1024*1024);
		int needWrite=1024*1024;
		int count;
		while(1)
		{
			count = write( fd,fileData,needWrite);
			if ( -1 == count )
			{
				printf("文件写入失败\n");
				sprintf(errerBuf,"File Write failure");
				sendJSON(DATA,errerBuf);
				free(fileData);
				close( fd );
				return -1;
			}
			else
			{
				needWrite-=count;
				if(needWrite<1)
					break;
				//printf("文件写入成功，实际写入的字节数目为:%d\n", count);
			}
		}
		free(fileData);
		close( fd );
	}
	return 0;
}

//最后删除所有的文件
int CTestSDCard::deleteFile()
{
	char errerBuf[1024];
	memset(errerBuf,0,sizeof(errerBuf));
	//sleep(3);
	char fileName[520];
	memset(fileName,0,sizeof(fileName));
	for(int i=0;i<100;i++)
	{
		sprintf(fileName,"SDtest/%d.txt",i);
		if(remove(fileName)!=0)
		{
			 printf("Could not delete the file %s \n",fileName);
			 sprintf(errerBuf,"Could not delete the file %s .remove errer %s",fileName,strerror(errno));
			 sendJSON(DATA,errerBuf);
			 return -1;
		}
	}
	return 0;
}






