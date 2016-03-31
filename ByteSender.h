#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <strsafe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


#define DEFAULT_PORT 27160
#define DEFAULT_BUFLEN 512

class ByteSender {

public:
	ByteSender();

	~ByteSender();

	void sendBuffer(char* buffer, size_t size);

	

private:

	WCHAR* init();

	struct sockaddr_in serv_addr, client_addr;
	int sockfd, newsockfd;
	socklen_t clientLength;

	WSADATA wsaData;
	bool connecting;
	char* m_IP;
	char* client_IP;
	SOCKET ListenSocket;
	SOCKET_ADDRESS localAddr;
	SOCKET_ADDRESS remoteAddr;
	socklen_t addrLength;
	int statusSize = 540;
	WCHAR status[540];
	
};
