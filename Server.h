#ifndef _Server_H_
#define _Server_H_
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>


#include <sstream>
#include <iostream>
#include <map>
#include <exception>
#include "ThreadPool.h"
#include "RequestAnalyzer.h"
#include "MessageSender.h"

#define MAXCONNECTNUM 8
#define MAXTHREADNUM 4
#define MAXEVENTCOUNT 10000

using namespace std;
class Server
{
private:
	ThreadPool threadPool;
	RequestAnalyzer requestHandler;
	MessageSender messageSender;

	map<int, RequestInfo *> socketRequestMap;

	bool isServerClosed;

	int serverSocket;
	struct sockaddr_in serverAddress;

	int myEpollFd;
	epoll_event events[MAXEVENTCOUNT];					//内核事件表
private:
	Server();
	int SetNonBlocking(int fd);
public:
	static Server& GetServer();
	Server(const Server&)=delete;
	Server(const Server&&)=delete;

	void SetUpConnection(const char* IPAddress, int portNum);
	void InteractWithClient();
	void CloseServer();

	void AddFdToEpollEvent(int epollfd, int fd, uint32_t events, bool oneShort = true);
	void ModifyFdInEpollEvent(int epollfd, int fd, uint32_t events, bool oneShot = true);
	void DeleteFdInEpollEvent(int epollfd, int fd);
	~Server();
};
#endif