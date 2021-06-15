#pragma once
#if defined (WIN32)
#include <Winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h> 
#include <arpa/inet.h>
#include <netdb.h>
#endif
struct CSocket
{
	CSocket() { dwConnID = -1; socket = nullptr;  privatedata = nullptr; }
	CSocket(int fd, void *st, void * pri) :dwConnID(fd), socket(st), privatedata(pri) {}
	CSocket(const CSocket& other) { dwConnID = other.dwConnID; socket = other.socket; privatedata = other.privatedata; }
	~CSocket() {}
	bool operator==(const CSocket& other) { return other.dwConnID == dwConnID; }
	bool operator!=(const CSocket& other) { return other.dwConnID != dwConnID; }
	CSocket & operator=(const CSocket& other) { dwConnID = other.dwConnID; socket = other.socket; privatedata = other.privatedata; return *this; }
	int dwConnID;
	void * socket;
	void * privatedata;
};

struct message
{
	message() {}
	message(const message& other ) { pBuff = other.pBuff; nLen = other.nLen;}
	message(unsigned char * p, const unsigned int &len):pBuff(p),nLen(len) {}
	message & operator=(const message& other) { pBuff = other.pBuff; nLen = other.nLen; return *this; }
	unsigned char *pBuff = nullptr;
	unsigned int nLen = 0;
};

class TcpServerListener
{
public:
	virtual int onAccept(const CSocket &socket, const char* remote_host, const int &remote_port, const int &local_port) = 0;
	virtual int onReceive(const CSocket &socket, const unsigned char* pData,const unsigned int &nLen) = 0;
	virtual int onSend(const CSocket &socket, const unsigned char* pData, const unsigned int &nLen) = 0;
	virtual int onError(const CSocket &socket, const unsigned long &nErrorCode) = 0;
	virtual int onClose(const CSocket &socket) = 0;
};

class EventServerSocketInterface
{
public:
	virtual ~EventServerSocketInterface() {}

	// interface methods
	virtual int start(const char* pHost, int nPort = 8080, int nMaxConnections = 100000) = 0;
	virtual int stop() = 0;
	virtual int send(const CSocket &socket, const unsigned char* buffer, const unsigned int &buffer_size) = 0;
	virtual int disconnect(const CSocket &socket) = 0;
};

class TcpClientListener
{
public:
	virtual int onClose(const CSocket &socket) = 0;
	virtual int onError(const CSocket &socket, const unsigned long &nErrorCode) = 0;
	virtual int onSend(const CSocket &socket, const unsigned char* pData, const unsigned int &nLen) = 0;
	virtual int onReceive(const CSocket &socket, const unsigned char* pData, const unsigned int &nLen) = 0;
	virtual int onConnect(const CSocket &socket, const char* remote_host, const int &remote_port, const int &local_port) = 0;
};

class EventClientSocketInterface
{
public:
	virtual ~EventClientSocketInterface() {}

	// interface methods
	virtual int connectServer(const char* host, const unsigned short &port, void * pri) = 0;
	virtual int disconnect(const CSocket &socket) = 0;
	virtual int send(const CSocket &socket, const unsigned char* buffer, const unsigned int &buffer_size) = 0;
};

struct CHttpRequest
{
	CHttpRequest() { type = 1; request = nullptr; }
	CHttpRequest(const int &t, void * req):type(t), request(req) {}
	int type;
	void * request;
};

class HttpServerInterface
{
public:
	virtual ~HttpServerInterface() {}

	// interface methods
	virtual int reply(const CHttpRequest &socket, const int &code, const char *reason, const char *msg) = 0;
	virtual int start(const char* pHost, int nPort = 8080) = 0;
	virtual int stop() = 0;
};


class HttpServerListener
{
public:
	virtual int onHttpRequest(const CHttpRequest &request, const char * uri, const char *body, const unsigned int &size) = 0;
};


