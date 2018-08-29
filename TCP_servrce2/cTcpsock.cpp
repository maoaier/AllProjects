#include "cTcpsock.h"

CTcpSock::CTcpSock()
{
}

CTcpSock::~CTcpSock()
{
	close(m_iSockfd);
}

//socket初始化
bool CTcpSock::sockInit(int port)
{
	m_iSockfd = socket(AF_INET, SOCK_STREAM, 0);   //AF_INET   IPV4   ;SOCK_STREAM   TCP
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(port);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int val = 1;
	int ret = setsockopt(m_iSockfd,SOL_SOCKET,SO_REUSEADDR,(void *)&val,sizeof(int));
	if(ret == -1)
	{
		printf("setsockopt");
		exit(1);
	}

	/*bind*/
	if(bind(m_iSockfd, (struct sockaddr* ) &server_sockaddr, sizeof(server_sockaddr))==-1) {
		perror("bind:\n");
		return false;
	}

	/*listen*/
	if(listen(m_iSockfd, QUEUE) == -1) {
		perror("listen");
		return  false;
	}
	FD_ZERO(&m_FdRfds);
	FD_SET(m_iSockfd,&m_FdRfds);
	m_iMaxfd=m_iSockfd;
	return true;
}

//套接字监听客服端的信息并接收指令
void CTcpSock::sockRun()
{
	while(1)
	{
		m_Fdtmpfds=m_FdRfds;
		m_StrTv.tv_sec=5;
		m_StrTv.tv_usec=0;
		int ret=select(m_iMaxfd+1,&m_Fdtmpfds,NULL,NULL,&m_StrTv);
		if(ret<0)
		{
		   printf("[%s %s %d]: 666\n", __FILE__, __func__, __LINE__);
		   perror("select");
		}
		else
			if(0==ret)
			{
			   //printf("服务端没有任何输入信息，并且客户端也没有信息到来，waiting...\n");
			   continue;
			}
			else
				if(ret>0)
				{
				   for(int i=0;i<m_iMaxfd+1;i++)
				   {
					   if(FD_ISSET(i,&m_Fdtmpfds))
					   {
						   if(i==m_iSockfd)
						   {
							   struct sockaddr_in caddr;
							   socklen_t len=sizeof(caddr);
							   int confd=accept(i,(struct sockaddr*)&caddr,&len);
							   if(confd<0)
							   {
								   printf("[%s %s %d]: 666\n", __FILE__, __func__, __LINE__);
								   perror("accept");
							   }
							   printf("confd=%d\n",confd);
							   printf("client IP:%s\n",inet_ntoa(caddr.sin_addr));
							   FD_SET(confd,&m_FdRfds);
							   if(m_iMaxfd<confd)
							   {
								   m_iMaxfd=confd;
							   }
						   }
						   else
						   {

							   setFd(i);
							   int flag=parsingJSON();
							   if(flag==-1)
							   {
								   FD_CLR(i,&m_FdRfds);
							   }
							   else
							   if(flag==-2)
							   {
								   break;
							   }

						   }

					   }
				   }
				}
   }

}

void CTcpSock::sockClear()
{
    close(m_iSockfd);
}

