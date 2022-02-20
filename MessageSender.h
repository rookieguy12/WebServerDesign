#ifndef _MESSAGE_SENDER_H_
#define _MESSAGE_SENDER_H_
class RequestInfo;
class MessageSender
{
public:
	bool SendFile(RequestInfo& requestInfo);
};
#endif