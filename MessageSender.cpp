#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/socket.h>

#include "MessageSender.h"
#include "RequestInfo.h"

bool MessageSender::SendFile(RequestInfo& requestInfo)
{
	int sendFileFd = requestInfo.GetFileDescription();
	int socket = requestInfo.GetRequestSocket();

	string& headString = requestInfo.GetHeadMessage();
	char* headMessage = const_cast<char *>(headString.c_str());
	int stringLength = headString.size();
	send(socket, headMessage, stringLength, 0);

	if (sendFileFd == -1)
	{
        cout << "do not find file!\n";
		return false;
	}
	
	struct stat stat_buf;
	fstat(sendFileFd, &stat_buf);
	sendfile(socket, sendFileFd, NULL, stat_buf.st_size);
	cout << "has send " << stat_buf.st_size << "bytes!\n";
	// std::filebuf* tmp = fileStream->rdbuf();
	// streampos size = tmp->pubseekoff(0, fileStream->end, fileStream->in);
	// tmp->pubseekpos(0, fileStream->in);
	// char* buffer = new char[size];
	// tmp->sgetn(buffer, size);
	// send(socket, buffer, size, 0);
    // cout << "has Sent " << size << " bytes!\n";
	// delete[] buffer;
	return true;
}