/*
 * client.cpp
 *
 *  Created on: Aug 13, 2016
 *      Author: huoyin
 */

#include <iostream>
#include <assert.h>
using namespace std;

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netdb.h>
#include <string.h>
#include "common.h"
#include "protocol.h"
#include "transfer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int proc_forward(int in, const char *rip, const char *rport);

int main(int argc, char *argv[]) {
	if (argc != 4) {
		cout << "Usage: ./socks_ser port remote_ip remote_port" << endl;
		return 1;
	}
	int lsnr = socket(AF_INET, SOCK_STREAM, 0);
	assert(lsnr >= 0);
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(atoi(argv[1]));

	assert(0 == bind(lsnr, (sockaddr *)&sa, sizeof(sa)));
	assert(0 == listen(lsnr, 5));
	cout << "started...." << endl;

	while(true) {
		sockaddr_in cli_addr;
		socklen_t slen = sizeof(cli_addr);
		int cli = accept(lsnr, (sockaddr *)&cli_addr, &slen);
		cout << "aceepted..." << endl;
		assert(cli != -1);
		pid_t pid = fork();
		if (pid > 0) {
			int status;
			waitpid(pid, &status, 0);
		} else {
			pid_t p2 = fork();
			if (p2 > 0) {
				exit(0);
			} else {
				return proc_forward(cli, argv[2], argv[3]);
			}
		}
	}
}

int proc_forward(int in, const char *rip, const char *rport) {
	int out = socket(AF_INET, SOCK_STREAM, 0);
	assert(out != -1);
	sockaddr_in raddr;
	raddr.sin_family = AF_INET;
	raddr.sin_addr.s_addr = inet_addr(rip);
	raddr.sin_port = htons(atoi(rport));
	assert(0 == connect(out, (sockaddr *)&raddr, sizeof(raddr)));

	//transfer
	ThreadParam tp1 = {in, out, 0};
	ThreadParam tp2 = {out, in, 0};
	assert(0 == pthread_create(&tp2.bro, NULL, transfer, &tp1));
	assert(0 == pthread_create(&tp1.bro, NULL, transfer, &tp2));

	pthread_join(tp1.bro, NULL);
	pthread_join(tp2.bro, NULL);

	return 0;
}
