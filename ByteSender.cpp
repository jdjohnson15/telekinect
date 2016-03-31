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
	WSACleanup();
}


void  ByteSender::sendBuffer(char* buffer, size_t size)
{
	//get address of client
	char recBuffer[1024];
	int recvlen = recvfrom(ListenSocket, recBuffer, sizeof(recBuffer), 0, (struct sockaddr *)&remoteAddr, &addrLength);
	//StringCchPrintf(status, statusSize, L"received %d bytes\n", recvlen);
	//OutputDebugString(status);
	if (recvlen > 0) {
		recBuffer[recvlen] = 0;
		StringCchPrintf(status, statusSize, L"received message: \"%s\"\n", recBuffer);
		OutputDebugString(status);
	}
	sendto(ListenSocket, recBuffer, sizeof(recBuffer), 0, (struct sockaddr *)&remoteAddr, addrLength);
}


WCHAR* ByteSender::init()
{
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		StringCchPrintf(status, statusSize, L"WSAStartup failed: %d\n", WSAGetLastError());
		return status;
	}

	char buffer[256];
	ZeroMemory(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(DEFAULT_PORT);
	//hints.ai_flags = AI_PASSIVE;

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		OutputDebugString(L"ERROR on binding\n");

	listen(sockfd, 5);
	

	clientLength = sizeof(client_addr);
	//while (newsockfd < 0) {
		OutputDebugString(L"waiting on accept\n");
		newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &clientLength);
	//}

	memset(buffer, 0, 2555);

	int n = recv(newsockfd, buffer, 2554, MSG_WAITALL);

	if (n < 0)
		OutputDebugString(L"ERROR reading from socket");

	StringCchPrintf(status, statusSize, L"received message: \"%s\"\n", buffer);
	OutputDebugString(status);


	n = send(newsockfd, "I got your message", 18, MSG_DONTROUTE);

	if (n < 0)
		OutputDebugString(L"ERROR writing to socket");

	closesocket(newsockfd);
	closesocket(sockfd);

	return L"server initialized, sending image data";
}

