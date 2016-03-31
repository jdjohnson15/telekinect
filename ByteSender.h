#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <strsafe.h>

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

class ByteSender {

public:
	ByteSender();

	~ByteSender();

	WCHAR* init();

	WCHAR* connect();
	WCHAR* sendBuffer(const char*, size_t size);

private:

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	WSADATA wsaData;
	
	char* m_IP;
	char* client_IP;
	SOCKET ListenSocket;
	SOCKET ClientSocket;
	int statusSize = 540;
	WCHAR status[540];
	
};
