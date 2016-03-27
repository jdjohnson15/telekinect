#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <strsafe.h>

#define DEFAULT_PORT "27160"
#define DEFAULT_BUFLEN 512

class ByteSender {

public:
	ByteSender();

	~ByteSender();

	void sendBuffer(char* buffer, size_t size);

private:

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	WCHAR* init();

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
