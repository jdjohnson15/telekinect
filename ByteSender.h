#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <strsafe.h>
#include <vector>
#include "globals.h"

#define DEFAULT_PORT "27160"
#define THREAD_MAX 3

class ByteSender {

public:
	ByteSender();

	~ByteSender();

private:

	struct addrinfo *result = NULL, *ptr = NULL, hints;


	WCHAR* SetupListener();



	void ClientHandler(SOCKET ClientSocket);
	bool SendDataToClient(SOCKET ClientSocket);

	//void AcceptConnections();
	WCHAR* init();
	WSADATA wsaData;
	bool connecting;
	char* m_IP;
	char* client_IP;
	SOCKET ListenSocket;
	SOCKET ClientSockets[THREAD_MAX];
	//SOCKET ClientSocket = INVALID_SOCKET;

	SOCKET_ADDRESS localAddr;
	SOCKET_ADDRESS remoteAddr;
	
	socklen_t addrLength;
	int statusSize = 540;

	int iResult, iSendResult;

	//because of vast difference in buffer size, there is a designated buffer for recieving and sending
	char sendbuf[DEFAULT_SENDBUFLEN];
	int sendbuflen = DEFAULT_SENDBUFLEN;
	char recvbuf[DEFAULT_RECVBUFLEN];
	int recvbuflen = DEFAULT_RECVBUFLEN;

	WCHAR status[540];
	
};
