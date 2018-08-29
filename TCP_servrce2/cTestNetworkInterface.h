/*
 * cTestNetworkInterface.h
 *
 *  Created on: 2018Äê7ÔÂ21ÈÕ
 *      Author: tarena
 */

#ifndef CTESTNETWORKINTERFACE_H_
#define CTESTNETWORKINTERFACE_H_
#include "cTestBase.h"

class CTestNetworkInterface:public CTestBase
{
public:

	virtual int test(int fd);
	CTestNetworkInterface();
	~CTestNetworkInterface();
};

#endif /* CTESTNETWORKINTERFACE_H_ */
