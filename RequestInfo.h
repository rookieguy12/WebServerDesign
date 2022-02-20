#ifndef _REQUEST_INFO_H
#define _REQUEST_INFO_H
#include<fstream>
#include<iostream>
using namespace std;
/// 包含对应socket所使用的fd，请求的文件fd，以及构造好的首部字段
class RequestInfo
{
private:
	int fd;
	string headMessageString;
	int connectSocket;
public:
	RequestInfo(int socket) : connectSocket(socket), fd(-1), headMessageString() {}
	
	void SetFileDescription(int fd) {
		this->fd = fd;
	}
	int GetFileDescription() const{
		return fd;
	}

	void SetHeadMessage(string& message) {
		headMessageString = message;
	}

	string& GetHeadMessage(){
		return headMessageString;
	}

	int GetRequestSocket(){
		return connectSocket;
	}
};

#endif // !_REQUEST_INFO_H
