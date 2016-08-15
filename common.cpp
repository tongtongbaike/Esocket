#include"common.h"
int recv_n(int sock,void* buf,size_t len)
{
	const char *left = (const char *)buf;
	size_t size = len;
	while(size>0)
	{
		int ret = recv(sock,left,size,0);
		enc(buf);
		if(ret <= 0)
		{
			break;
		}

		size -= ret;
		left += ret;
	}
	buf[len] = '\0';
	return 1;
}

int send_n(int sock,void* buf,size_t len)
{
	const char *left = (const char *)buf;
	size_t size = len;
	while(size > 0)
	{
		enc(buf);
		int ret = send(sock,left,size,0);
		if(ret < 0)
		{
			break;
		}
		size -= ret;
		left += ret;
	}
	buf[len] = '\0';
	return 1;
}

void enc(void *buf)
{
	char *index = (char *)buf;
	for(int i = 0;i < strlen(buf)+1;++i)
	{
		index[i] ^= 6; 
	}
}
