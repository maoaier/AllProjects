//用来处理测试类
#ifndef CTESTPROCESSING_H
#define CTESTPROCESSING_H
#include "cJSON.h"
#include <string.h>
#include "cTestBase.h"
//#include "testDo.h"
#include "cTestDo.h"
#include "cTestDI.h"
#include "cTestSDCard.h"
#include "cTestAD.h"
#include "cTestNetworkInterface.h"
#include "cTestBlueTooth.h"
#include "cTest4G.h"

class CTestProcessing: public CTestBase
{
public:
	int test(int fd) {return 1;}
    CTestProcessing();
    int parsingJSON();
private:
    void testProcess();
    int m_iDOFlag;
    testDo m_cTeDO;
};

#endif // CTESTPROCESSING_H
