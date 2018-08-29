/*
 * cTestBlueTooth.h
 *
 *  Created on: 2018Äê7ÔÂ23ÈÕ
 *      Author: tarena
 */

#ifndef CHUB_H_
#define CHUB_H_
#include "mo_led.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <termios.h>



class CHub
{
public:
	CHub();
	~CHub();
	int hubTest();
	int connectivity(unsigned char dataBuf[],int needWrite);
private:
	bool SetSeriOpt(int fd);
	bool ReadData(unsigned char *buf, unsigned long bufSize, unsigned long &readSize);
	int m_iFd;
};

#endif /* CHUB_H_ */
