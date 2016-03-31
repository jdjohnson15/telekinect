#include "ByteSender.h"

#pragma comment(lib, "Ws2_32.lib")

ByteSender::ByteSender() {

	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;
}

ByteSender::~ByteSender() {
	// shutdown the send half of the connection since no more data will be sent
	int iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		StringCchPrintf(status, statusSize, L"shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
	}
}

WCHAR* ByteSender::connect()
{
	int iResult;
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		StringCchPrintf(status, statusSize, L"bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return status;
	}
	freeaddrinfo(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		StringCchPrintf(status, statusSize, L"Listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return status;
	}

	// Accept a client socket
	OutputDebugString(L"attempting handshake\n");
	ClientSocket = accept(ListenSocket, NULL, NULL);
	OutputDebugString(L"handshake successful\n");
	if (ClientSocket == INVALID_SOCKET) {
		StringCchPrintf(status, statusSize, L"accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return status;
	}
	return L"connection successful";
}

WCHAR*  ByteSender::sendBuffer(const char* buffer, size_t size)
{
	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	do {
		
		iResult = send(ClientSocket, buffer, iResult, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			StringCchPrintf(status, statusSize, L"Bytes received: %d\n", iResult);
			OutputDebugString(status);
			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				StringCchPrintf(status, statusSize, L"send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return status;
			}
			printf("Bytes sent: %d\n", iSendResult);
			StringCchPrintf(status, statusSize, L"send failed: %d\n", WSAGetLastError());
		}
		else if (iResult == 0)
		StringCchPrintf(status, statusSize, L"Connection closing...\n");
		else {
			StringCchPrintf(status, statusSize, L"recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return status;
		}

	} while (iResult > 0);

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
	return L"server initialized, waiting for handshake";
}
