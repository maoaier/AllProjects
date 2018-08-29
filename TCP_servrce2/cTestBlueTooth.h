/*
 * cTestBlueTooth.h
 *
 *  Created on: 2018年7月23日
 *      Author: tarena
 */

#ifndef CTESTBLUETOOTH_H_
#define CTESTBLUETOOTH_H_
#include "cTestBase.h"
#include <map>

#define GPIO_MODE_INPUT    0
#define GPIO_MODE_OUTPUT   1
#define GPIO_BT_KEY 48 //blue toooth PIO11
#define GPIO_BT_POWER 22 //BlueTooth power
#define GPIO_BT_OPEN 0
#define GPIO_BT_CLOSE 1



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

class CHub: public CTestBase
{
public:
	CHub();
	virtual int test(int fd);
	~CHub();
private:
	int blueToothTest();
	void _setGpioValue(int gpio,int value);
	int _readGpioState(int gpio);
	void closeBlue();
	bool SetSeriOpt(int fd);
	bool ReadData(char *buf, unsigned long bufSize, unsigned long &readSize);
	int m_iFd;
};

#endif /* CTESTBLUETOOTH_H_ */
