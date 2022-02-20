#ifndef _Configuration_H_
#define _Configuration_H_
#include <string>
class Configuration
{
private:
	std::string ipAddress;
	int portNum;
public:
	bool SetUpInfo(std::string configFile);
	const char* GetIpAddress();
	int GetPortNum();
};
#endif
