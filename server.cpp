#include<iostream>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<wait.h>
#include<pthreead.h>
#include<signal.h>
#include<unistd.h>
#include<assert.h>
#include"common.h"
using namespace std;

int pro_deal(int req_sock);

int main(int argc,char *argv[])
{
	if(argc != 2)
	{
		cout << "server start ./server port" << endl;
		return -1;
	}

	int listen_sock = sock(AF_INET,SOCK_STREAM,0);

	signal(SIGPIPR,SIG_IGN);
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(argv[1]);
	server_addr.sin_addr.s_addr = INADDR_ANY;	
	
	assert(0 == bind(listen_sock,(sockaddr *)&server_addr),sizeof(server_addr));

	assert(0 == listen(listen_sock,5));

	sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	assert(0 < (req_sock = accept(listen_sock,(sockaddr *)&client_addr,&len)));

	pid_t pid = fork();
	if(pid < 0)
	{
		cout << "fork pid  error"<< endl;
		exit(-1);
	}
	else if(pid == 0)
	{
		pid_t deal = fork();
		if(deal < 0)
		{
			cout<<" fork deal error"<< endl;
			exit(0);
		}
		else if(deal == 0)
		{
			return pro_deal(req_sock);
		}
		else 
		{
			exit(-2);
		}
	}
	else
	{
		waitpid(pid,NULL,0);
	}

	return 0;
}

int pro_deal(int req_sock)
{
	//接收握手请求
	Handler_req web_req;
	assert(0 < recv_n(req_sock,&web_req,sizeof(web_req)));
	assert(web_req.ver == 5);
	char methods[256];
	assert(0 < recv_n(req_sock,methods,web_req.method_n));
	assert(0 == methods[0]);

	//回复握手确认
	Handler_rep web_rep;
	web_rep.ver = 5;
	web_rep.method = 0;
	assert(0 < send_n(req_sock,&web_rep,sizeof(web_rep)));
	assert(web_rep.ver == 5 && web_rep.cmd == 1&& web_rep.rsv == 0)
	//获取请求
	Req request;
	assert(0 < recv_n(req_sock,&request,sizeof(request)));

	char req_addr[1024];
	int req_port;
	
	sockaddr_in object_server;
	object_server.sin_family = AF_INET;
	object_server.sin_port = htons(request.);
	
	if(request.atyp == 1)
	{
		assert(0 < recv_n(req_sock,req_addr,4));
		object_server.sin_addr.s_addr = htonl(atoi(req_addr));
	}
	else if(request.atyp == 3)
	{
		int addr_len;
		hostent *host;
		char str[256];
		assert(0 < recv_n(req_sock.&addr_len,1));
		assert(0 < recv_n(req_sock,req_addr,addr_len));
		host = gethostbyname(req_addr);
		assert(host && host->h_addrtype == AF_INET && host->h_length >= 1);
		inet_ntop(host->h_addrtype,host->h_addr,str,sizeof(str));
		object_server.sin_addr.s_addr = inet_addr(str);
	}
	assert(0 < recv_n(req_sock,&req_port,4));
	objec_server.sin_port = htons(req_port);
	//connect
	int object_sock = socket(AF_INET,SOCK_STREAM,0);
	assert(1 != object_sock);
	assert(0 == connect(object_sock,(sockaddr*)&object_server,sizeof(object_server)));
	//返回请求消息确认.
	Rep reply;
	reply.ver = 5;
	reply.cmd = 0;
	reply.rsv = 0;
	reply.atyp = 1;
	reply.dst_addr = object_server,sinaddr;
	reply.dst_port = object.sin_port;

	assert(0 < send_n(req_sock,&reply,sizeof(reply)));
	
	//transfer
	Transfer_sock tp1 = {req_sock,object_sock,0};
	Transfer_sock tp2 = {object_sock,req_sock,0};
	assert(0 == pthread_create(&tp2->tid,NULL,transfer,&tp1));
	assert(0 == pthread_create(&tp1->tid,NULL,transfer,&tp2));

	pthread_join(tp1->tid);
	pthread_join(tp2->tid);
	
	return 0;

}

