#ifndef _My_Exception_H_
#define _My_Exception_H_
#include "Server.h"
#pragma region 异常处理
class MyException : public exception
{
public:
	const char *message;
	MyException(const char* description) noexcept : message(description){};
	virtual const char* what() const throw() final
	{
		return message;
	}
	virtual void ExceptionHandler() = 0;
};

class InValidSocketException : public MyException
{
public:
	InValidSocketException() noexcept : MyException("The socket is created unseccessifully!") {}
	virtual void ExceptionHandler() {  }
};

class SocketBindErrorException : public MyException
{
public:
	SocketBindErrorException() noexcept : MyException("The socket is bound unseccessifully!") {}
	virtual void ExceptionHandler() { Server::GetServer().CloseServer();  }
};
class ListenStateErrorException : public MyException
{
public:
	ListenStateErrorException() noexcept : MyException("Set Listen State unseccessifully!") {}
	virtual void ExceptionHandler() { Server::GetServer().CloseServer();}
};

class EpollCreateErrorException : public MyException
{
public:
	EpollCreateErrorException() noexcept : MyException("Create epollfd unseccessifully!") {}
	virtual void ExceptionHandler() { Server::GetServer().CloseServer();}
};
class RequestInfoLostException : public MyException
{
public:
	RequestInfoLostException() noexcept : MyException("RequestInfo doesn't exist in map!") {}
	virtual void ExceptionHandler() { Server::GetServer().CloseServer();}
};
class UnExpectedRequest : public MyException
{
public:
	UnExpectedRequest() noexcept : MyException("Cannot understand the Request!") {}
	virtual void ExceptionHandler() { Server::GetServer().CloseServer(); }
};
#pragma endregion
#endif