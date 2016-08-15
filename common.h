#pragma once
#include<iostream>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>

#pragma pack push
#pragma pack(1)
struct Handler_req
{
	char ver;
	char method_n;
}；

struct Handler_rep
{
	char ver;
	char method;
};

	//VER	CMD	RSV	ATYP	DST.ADDR	DST.PROT
struct Req
{
	char ver;
	char cmd;
	char rsv;
	char atyp;
};

struct Rep
{
	char ver;
	char cmd;
	char rsv;
	char atyp;
	sin_addr dst_addr;
	int dst_port;
};

struct Transfer_sock
{
	int in_sock;
	int out_sock;
	pthread_t tid;
}

int recv_n(int sock,void* buf,size_t len);

int send_n(int sock,void* buf,size_t len);

void enc(void*buf);  
#pragma pack pop
