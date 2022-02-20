#include "Server.h"
#include "Configuration.h"
#include "MyException.h"
#include <iostream>
using namespace std;
int main() {
	static Configuration config;
	config.SetUpInfo("config.txt");
	const char* IPAddress = config.GetIpAddress();
	int portNum = config.GetPortNum();
	Server& server = Server::GetServer();
	server.SetUpConnection(IPAddress, portNum);
	try
	{
		server.InteractWithClient();
	}
	catch (MyException& except)
	{
		cout << except.what();
		except.ExceptionHandler();
	}
}