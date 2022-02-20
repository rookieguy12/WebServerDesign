#include "Configuration.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
bool Configuration::SetUpInfo(string configFile)
{
	ifstream* configFileStream = new ifstream(configFile);
	if (configFileStream->is_open())
	{
		*configFileStream >> ipAddress;
		*configFileStream >> portNum;
	}
	else
	{
		std::cerr << "Cannot open the file \" " << configFile << " \"!\n";
        delete configFileStream;
		return false;
	}
    delete configFileStream;
	return true;
}


const char* Configuration::GetIpAddress()
{
	return ipAddress.c_str();
}

int Configuration::GetPortNum()
{
	return portNum;
}