#ifndef _RequestAnalyzer_H_
#define _RequestAnalyzer_H_
#include <vector>
#include <sstream>
#include <iostream>
#include <map>
#include <regex>
#include "RequestInfo.h"
#define READBUFFERLENGTH 4096
using namespace std;

/// <summary>
/// httpVersion: http版本
/// contentTyep: Content-Type
/// filePath: 文件路径
/// </summary>
struct AnalyseResult {
	string httpVersion;
	string contentType;
	string filePath;
};
class RequestAnalyzer
{
private:
	string absolutePath;
	map<string, string> contentTypeMap;
	void GetRequestHeadMessage(RequestInfo& reqeustInfo, AnalyseResult& analyseResult);
	AnalyseResult AnalyseRequest(const char* receiveInfo);
	void InitializeFileTypeMap();
	int SearchLocalFile(string&);
public:
	char* ReceiveMessage(int socket);
	RequestInfo* HandleRequest(int socket, char* message);
	RequestAnalyzer(string filePath = "directoryConfig.txt");
};
#endif