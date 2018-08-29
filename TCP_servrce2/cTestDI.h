/*
 * ctestdi.h
 *	DI测试
 *  Created on: 2018年7月18日
 *      Author: tarena
 */

#ifndef CTESTDI_H_
#define CTESTDI_H_
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

class CTestDI:public CTestBase
{
public:
	CTestDI();
	virtual int test(int fd);
	~CTestDI();
private:
	void _setGpioValue(int gpio,int value);
	int _readGpioState(int gpio);
};

#endif /* CTESTDI_H_ */
