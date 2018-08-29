/*
 * cTestAD.h
 *
 *  Created on: 2018年7月19日
 *      Author: tarena
 *      //读取AD端口的电流数据和电压数据，并与实际数据做比较
 */

#ifndef CTESTAD_H_
#define CTESTAD_H_
#include "cTestBase.h"
#include <iostream>
#include <math.h>
using namespace std;

#define ONE_WAVE_CNT 2314
#define AD_SAMPLE 128
#define VOLTAGERATIO (0.024548*0.1249966)
#define CURRENTRATIO (0.001366*0.1249966)

#ifdef __cplusplus
extern "C"
{
	// libspicomm.a函数
	int spicomm_read(char *rxbuf, int len);
}
#endif

typedef struct _AdData
{
	float ua[AD_SAMPLE];
	float ub[AD_SAMPLE];
	float uc[AD_SAMPLE];
	float ia[AD_SAMPLE];
	float ib[AD_SAMPLE];
	float ic[AD_SAMPLE];
}AdData;


class CTestAD:public CTestBase
{
public:
	CTestAD();
	virtual int test(int fd);
	~CTestAD();
private:
	int analyticData();
	int testAd();
	unsigned char CRC8_Table(unsigned char *p, int counter);
	bool setseriopt(int fd);
	AdData m_strAdAata[10];
};

#endif /* CTESTAD_H_ */
