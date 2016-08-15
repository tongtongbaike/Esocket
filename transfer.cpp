#include "transfer.h"


void* transfer(void *arg)
{
	Transfer_sock* transfer = (Transfer_sock*)arg;
	char str[1024*1024];
	while(true)
	{
		int ret = recv(transfer->in_sock,str,sizeof(str),0);
		if(ret<=0)
		{
			break;
		}
		enc(str);
		ret = send_n(transfer->out_sock,str,sizeof(str),0);
		if(ret < 0)
		{
			break;
		}
	}
	close(transfer->in_sock);
	close(transfer->out_sock);
	pthread_cancel(transfer->tid);
	return NULL;
}
