//TCP服务器，用来连接客服端和接收客服端的测试指令，清除未使用的端口，把命令交给Ctestprocess处理
#ifndef CTCPSOCK_H
#define CTCPSOCK_H
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include "cJSON.h"
#include "cTestProcessing.h"
#define QUEUE 20

class CTcpSock:public CTestProcessing
{
public:
    CTcpSock();
    ~CTcpSock();
    bool sockInit(int PORT);
    void sockRun();
    void sockClear();
private:
    fd_set m_FdRfds,m_Fdtmpfds;
    int m_iRetval, m_iMaxfd;     //选择器
    int m_iSockfd;
    struct timeval m_StrTv;
};
#endif // CTCPSOCK_H
