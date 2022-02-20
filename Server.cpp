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

#include "Server.h"
#include "MyException.h"

using namespace std;
Server::Server() : threadPool(MAXTHREADNUM), requestHandler(), messageSender(){}

int Server::SetNonBlocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

void Server::AddFdToEpollEvent(int epollfd, int fd, uint32_t events, bool oneShot)
{
	epoll_event event;
	event.data.fd = fd;
	event.events = events;
	if (oneShot)
		event.events |= EPOLLONESHOT;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	SetNonBlocking(fd);
}

void Server::ModifyFdInEpollEvent(int epollfd, int fd, uint32_t events, bool oneShot)
{
	epoll_event event;
	event.data.fd = fd;
	event.events = events;
	if (oneShot)
		event.events |= EPOLLONESHOT;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
	SetNonBlocking(fd);
}

void Server::DeleteFdInEpollEvent(int epollfd, int fd)
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
}

Server& Server::GetServer()
{
	static Server server;
	return server;
}

void Server::SetUpConnection(const char *ipAddress, int portNum)
{
	try
	{
		bzero(&serverAddress, sizeof(serverAddress));
		serverAddress.sin_family = AF_INET;
		inet_pton(AF_INET, ipAddress, &serverAddress.sin_addr);
		serverAddress.sin_port = htons(portNum);

		serverSocket = socket(PF_INET, SOCK_STREAM, 0);
		if (serverSocket < 0)
			throw InValidSocketException();
		else
			cout << "Create listen socket successfully!\n";

		if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
			throw SocketBindErrorException();

		if (listen(serverSocket, MAXCONNECTNUM) == -1)
			throw ListenStateErrorException();

		//epoll创建内核事件表
		myEpollFd = epoll_create(MAXCONNECTNUM);
		if (myEpollFd == -1)
			throw EpollCreateErrorException();

		AddFdToEpollEvent(myEpollFd, serverSocket, EPOLLIN | EPOLLET, false);	// serverSocket就注意别设置成oneShot了
	}
	catch (MyException &except)
	{
		cout << except.what();
		except.ExceptionHandler();
	}
}

void Server::InteractWithClient()
{
	cout << "serverSocket: " << serverSocket << endl;
	while (true) {
		int readyFdNum = epoll_wait(myEpollFd, events, MAXEVENTCOUNT, -1);
		if (readyFdNum < 0)
		{
			cout << "epoll failure! Server will close!\n";
			break;
		}
		cout << "it has " << readyFdNum << " ready!\n";
		for (int i = 0; i < readyFdNum; ++i)
		{
			int sockfd = events[i].data.fd;

			// 如果是serverSocketFd，说明获得了一个连接请求，则则新建连接socket，并将文件描述符注册可读就绪事件.
			if (sockfd == serverSocket)
			{
				struct sockaddr_in clientAddress;
				socklen_t clientAddressLength = sizeof(clientAddress);
				int connectSocketFd = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
				AddFdToEpollEvent(myEpollFd, connectSocketFd, EPOLLIN | EPOLLET | EPOLLRDHUP);
			}

			// 判断关闭的放前面，查了下，原因是在Linux2.6.17以后，检测到关闭时，events触发EPOLLIN | EPOLLRDHUP，此前只会触发EPOLLIN
			else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
			{
				DeleteFdInEpollEvent(myEpollFd, sockfd);
				if (socketRequestMap.count(sockfd))
					socketRequestMap.erase(sockfd);
				close(sockfd);
			}
			
			// 如果是连接socketFd的可读事件
			else if (events[i].events & EPOLLIN)
			{
				threadPool.enqueue(
					[&] {
						char *request = requestHandler.ReceiveMessage(sockfd);
                        RequestInfo* analysis = requestHandler.HandleRequest(sockfd, request);
						delete [] request;
                        socketRequestMap[sockfd] = analysis;
                        Server::GetServer().ModifyFdInEpollEvent(myEpollFd, sockfd, EPOLLOUT | EPOLLET | EPOLLRDHUP);// 触发可读事件了，就修改注册可写事件
					});
			}

			// 如果是连接socket的可写事件
			else if (events[i].events & EPOLLOUT)
			{
				threadPool.enqueue(
					[&] {
                        if (socketRequestMap.count(sockfd) == 0)
					        throw RequestInfoLostException();
				        RequestInfo* targetInfo = socketRequestMap[sockfd];
						messageSender.SendFile(*targetInfo);
						socketRequestMap.erase(sockfd);
						targetInfo->~RequestInfo();
						Server::GetServer().ModifyFdInEpollEvent(myEpollFd, sockfd, EPOLLIN | EPOLLET | EPOLLRDHUP);
					});
			}
		}
	}
}

void Server::CloseServer()
{
	exit(1);
}

Server::~Server() {}