#include "ByteSender.h"

#pragma comment(lib, "Ws2_32.lib")

ByteSender::ByteSender() {

	ListenSocket = INVALID_SOCKET;
	init();
}

ByteSender::~ByteSender() {
	// shutdown the send half of the connection since no more data will be sent
	int iResult = shutdown(ListenSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		StringCchPrintf(status, statusSize, L"shutdown failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
	}
}


void  ByteSender::sendBuffer(char* buffer, size_t size)
{
	//get address of client
	char* recBuffer = "";
	int recvlen = recvfrom(ListenSocket, recBuffer, size, 0, (struct sockaddr *)&remoteAddr, &addrLength);
	StringCchPrintf(status, statusSize, L"received %d bytes\n", recvlen);
	OutputDebugString(status);
	if (recvlen > 0) {
		recBuffer[recvlen] = 0;
		StringCchPrintf(status, statusSize, L"received message: \"%s\"\n", recBuffer);
		OutputDebugString(status);
	}
	sendto(ListenSocket, buffer, size, 0, (struct sockaddr *)&remoteAddr, addrLength);
}


WCHAR* ByteSender::init()
{
	/*
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		StringCchPrintf(status, statusSize, L"WSAStartup failed: %d\n", WSAGetLastError());
		return status;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	//hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		StringCchPrintf(status, statusSize, L"getaddrinfo failed: %d\n", WSAGetLastError());
		WSACleanup();
		return status;
	}

	
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		StringCchPrintf(status, statusSize, L"Error at socket(): %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return status;
	}
	*/

	if ((ListenSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket");
		return 0;
	}
	struct sockaddr_in localAddr;
	addrLength = sizeof(remoteAddr);


	memset((char *)&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons((u_short)DEFAULT_PORT);

	if (bind(ListenSocket, (struct sockaddr *)&localAddr, sizeof(localAddr)) < 0) {
		perror("bind failed");
		return 0;
	}
	connecting = true;
	return L"server initialized, sending image data";
}
