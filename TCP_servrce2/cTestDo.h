/*
 * testDo.h
 *
 *  Created on: 2018年7月17日
 *      Author: tarena
 */

#ifndef CTESTDO_H_
#define CTESTDO_H_

//#include <sys/socket.h>
//#include <sys/select.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <unistd.h>
//#include <stdio.h>
//#include <errno.h>
//#include <stdlib.h>
//#include <string>
//#include <string.h>
//#include <vector>
//#include <fcntl.h>
//#include <termios.h>
//#include <sys/ioctl.h>
//#include <sys/time.h>
//#include <errno.h>
//#include <stdarg.h>
//#include <sys/stat.h>
//#include <dirent.h>
//#include <iostream>
//#include <fstream>
//#include <math.h>
//#include <iostream>
#include "cTestBase.h"

#define GPIO_MODE_INPUT    0
#define GPIO_MODE_OUTPUT   1
#define GPIO_DO_PORT	   43
#define GPIO_DI_PORT	   44


#ifdef __cplusplus
extern "C"
{
	// libgpio.a函数
	int gpio_register(size_t gpio, int mode);
	int gpio_unregister(size_t gpio);
	int gpio_set_value(size_t gpio, int value);
	int gpio_get_value(size_t gpio);
};
#endif


using namespace std;

class testDo:public CTestBase
{

public:
	virtual int test(int fd);
	testDo();
	void initGPIO();
	~testDo();
private:
	void _setGpioValue(int gpio,int value);
	int _readGpioState(int gpio);


};

#endif /* CTESTDO_H_ */
