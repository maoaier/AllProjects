/*
 * testDo.cpp
 *
 *  Created on: 2018年7月17日
 *      Author: tarena
 */

#include "cTestDo.h"

testDo::testDo() {
	// TODO Auto-generated constructor stub
}


void testDo::initGPIO()
{
	int value=0;
	_setGpioValue(GPIO_DO_PORT,value);
}


//DO测试
int testDo::test(int fd)
{
	setFd(fd);
	int value=_readGpioState(GPIO_DO_PORT);
	_setGpioValue(GPIO_DO_PORT,1);
	printf("%s: %d",__FILE__,__LINE__);
//	for(int i=0;i<8;i++)
//	{
//		if(value!=0)
//			value=0;
//		else
//			value=1;
//		_setGpioValue(GPIO_DO_PORT,value);
//		sleep(2);
//	}
	char buf[1024];
	memset(buf,0,sizeof(buf));
	strcpy(buf,"\nERROR:If there is no sound\nSUCCESSFUL if it dripped sound");
	sendJSON(RESULT,buf);
	memset(buf,0,sizeof(buf));
	strcpy(buf,"DO test END");
	sendJSON(END,buf);
	return 0;
}

testDo::~testDo() {
	// TODO Auto-generated destructor stub
}
//设置GPIO口的值
void testDo::_setGpioValue(int gpio,int value)
{

	printf("Please input gpio number: ");
	gpio_register(gpio, GPIO_MODE_OUTPUT);
	gpio_set_value(gpio, value);
	gpio_unregister(gpio);
	return ;
}

//读取GPIO口的值
int testDo::_readGpioState(int gpio)
{
	printf("Please input gpio number: ");
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


