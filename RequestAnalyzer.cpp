#include "RequestAnalyzer.h"
#include "RequestInfo.h"
#include "sys/socket.h"
#include <fcntl.h>
char* RequestAnalyzer::ReceiveMessage(int socket)
{
	int receiveNum;
	char* receiveBuffer = new char[READBUFFERLENGTH];
	memset(receiveBuffer, '\0', READBUFFERLENGTH);
	receiveNum = recv(socket, receiveBuffer, READBUFFERLENGTH, 0);
	cout << "currentSocket: " << socket << endl;
	cout << "The info is : \n" << receiveBuffer;
	return receiveBuffer;
}
RequestInfo* RequestAnalyzer::HandleRequest(int socket, char *message)
{
	RequestInfo *requestInfo = new RequestInfo(socket);
	AnalyseResult result = AnalyseRequest(message);										
	int fileFd = SearchLocalFile(result.filePath);							
	requestInfo->SetFileDescription(fileFd);												
	GetRequestHeadMessage(*requestInfo, result);											
	return requestInfo;
}
RequestAnalyzer::RequestAnalyzer(string filePath)
{
	ifstream filePathStream(filePath);
	if (filePathStream.is_open())
	{
		filePathStream >> absolutePath;
	}
	InitializeFileTypeMap();
}

void RequestAnalyzer::InitializeFileTypeMap()
{
	contentTypeMap["png"] = "image/png";
	contentTypeMap["jpg"] = "image/jpeg";
	contentTypeMap["html"] = "text/html";
	contentTypeMap["mp3"] = "audio/mp3";
}

int RequestAnalyzer::SearchLocalFile(string& path)
{
	string realPath = absolutePath + path;
	int fileFd = open(realPath.c_str(), O_RDONLY);
	if (fileFd == -1)
	{
		cout << "do not find target file!\n";
	}
	return fileFd;
	// ifstream* file = new ifstream(realPath, ifstream::in | ios::binary);
	// if (!file->is_open())
	// {
	// 	cout << "do not find target file!\n";
	// 	delete file;
	// 	return nullptr;
	// }
	// return file;
}

void RequestAnalyzer::GetRequestHeadMessage(RequestInfo& requestInfo, AnalyseResult& analyseResult)
{
	stringstream remsg;
	if (requestInfo.GetFileDescription() != -1)
	{
		remsg << analyseResult.httpVersion << " 200 OK\r\n"
			<< "Connection: close\r\n"
			<< "Server: MyLocal\r\n"
			<< "Content Type: " + analyseResult.contentType << "\r\n\r\n";
	}
	else {
		remsg << analyseResult.httpVersion << " 404 Not Found\r\n"
			<< "Connection: close\r\n"
			<< "Server: MyLocal\r\n"
			<< "Content Type: text/html\r\n\r\n";
	}
    
	string remsgString = remsg.str();
    cout << "head Message is : \n" << remsgString << endl;
	requestInfo.SetHeadMessage(remsgString);
	return;
}

AnalyseResult RequestAnalyzer::AnalyseRequest(const char* receiveInfo)
{
	int index = 0;
	while (receiveInfo[index++] != '\n');
	string targetString(receiveInfo, index);
	AnalyseResult result;
	regex filePath("/\\w+.\\w+");
	regex httpVersion("HTTP/\\w.\\w");
	cout << targetString;
	smatch matchResult;
	if (regex_search(targetString, matchResult, filePath))
	{
		result.filePath = matchResult.str(0);
        cout << result.filePath << endl;
	}
	if (regex_search(targetString, matchResult, httpVersion))
	{
		result.httpVersion = matchResult.str(0);
        cout << result.httpVersion << endl;
	}
	string fileType = result.filePath.substr(result.filePath.find_last_of('.') + 1);
	if (contentTypeMap.count(fileType) != 0)
		result.contentType = contentTypeMap[fileType];
	return result;
}