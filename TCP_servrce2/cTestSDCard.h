#ifndef CARDSD_H
#define CARDSD_H

#include <iostream>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "cTestBase.h"

class CTestSDCard:public CTestBase
{
public:
    CTestSDCard();
    virtual int test(int fd);
private:
    int readFile();
    void CreateDir();
	int createFile();
	int deleteFile();
};

#endif // CARDSD_H
