#include "stdafx.h"
#include "ByteSender.h"
#include <thread>

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

WCHAR* ByteSender::SetupListener() 
{
	printf("setting up listener\n");

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		StringCchPrintf(status, statusSize, L"getaddrinfo failed with error: %d\n", iResult);
		OutputDebugString(status);
		return status;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		freeaddrinfo(result);
		StringCchPrintf(status, statusSize, L"socket failed with error: %ld\n", WSAGetLastError());
		OutputDebugString(status);
		return status;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(result);
		closesocket(ListenSocket);
		StringCchPrintf(status, statusSize, L"bind failed with error: %d\n", WSAGetLastError());
		OutputDebugString(status);
		return status;
	}

	freeaddrinfo(result);

	printf( "listening sockets\n");

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		closesocket(ListenSocket);
		StringCchPrintf(status, statusSize, L"listen failed with error: %d\n", WSAGetLastError());
		OutputDebugString(status);
		return status;
	}
	//SOCKADDR serverAddr;
	//int serverAddrSize;
	//getsockname(ListenSocket, &serverAddr, &serverAddrSize);
	char hostname[80];
	if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
		StringCchPrintf(status, statusSize, L"get hostname failed\n");
		OutputDebugString(status);
		return status;
	}

	struct hostent *phe = gethostbyname(hostname);
	if (phe == 0) {
		return (L"bad host lookup\n");
	}
	StringCchPrintf(status, statusSize, L"Server setup complete.\n Server address: %d\n", hostname);
	OutputDebugString(status);
	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		StringCchPrintf(status, statusSize, L"address: %d\n", inet_ntoa(addr));
		OutputDebugString(status); 
	}

	
	return status;
}
//
////// AcceptConnections /////////////////////////////////////////////////
//// Spins forever waiting for connections.  For each one that comes in, 
//// we create a thread to handle it and go back to waiting for
//// connections.  If an error occurs, we return.
//void ByteSender::AcceptConnections() {
//	printf("accepting connections\n");
//
//	// Accept a client socket
//	SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
//	if (ClientSocket != INVALID_SOCKET) {
//		//start separate threads to handle multiple clients
//		std::thread t(&ByteSender::ClientHandler, this, ClientSocket);
//		//t.join();
//		//ClientHandler();
//	}
//	else {
//		//printf("accept failed with error: %d\n", WSAGetLastError());
//		//StringCchPrintf(status, statusSize, L"accept failed with error: %d\n", WSAGetLastError());
//		//OutputDebugString(status);
//		//closesocket(ListenSocket);
//		//WSACleanup();
//		return;
//	}
//
//}

//This is the threaded function used to process client requests and send data back while the connection is still valid
void ByteSender::ClientHandler(SOCKET socket) 
{
	SendDataToClient(socket);
	
	// shutdown the connection since we're done
	iResult = shutdown(socket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
	}

	// cleanup
	closesocket(socket);
	//WSACleanup();
	printf("shutting down client handler thread\n");
}

bool ByteSender::SendDataToClient(SOCKET socket) 
{

	// Receive until the peer shuts down the connection
	do {
		printf("trying to receive\n");
		iResult = recv(socket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			//process what the client is receiving:
			//recvbuf[iResult] = 0; //marks the end of the char array (not working right now)
			//printf("Bytes received: %d, content:\"%s\", conten len:%d, msg len: %d\n", iResult, recvbuf,strlen(recvbuf), strlen(Globals::data));

			/*if (strcmp(recvbuf, "sendMeData") == 0) {
				printf("sending back global data");
				iSendResult = send(socket, Globals::data, strlen(Globals::data), 0);
			}
			else */

			
			if (recvbuf[0] == 0x61)
				iSendResult = send(socket, Globals::data, DEFAULT_SENDBUFLEN, 0);

			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError()); 
				return false;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			return false;
		}
	} while (iResult > 0);
	
	return true;
}


WCHAR* ByteSender::init()
{	
#pragma region Previously Working Code
	//if ((ListenSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	//	perror("cannot create socket");
	//	return 0;
	//}
	//struct sockaddr_in localAddr;
	//addrLength = sizeof(remoteAddr);


	//memset((char *)&localAddr, 0, sizeof(localAddr));
	//localAddr.sin_family = AF_INET;
	//localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//localAddr.sin_port = htons((u_short)DEFAULT_PORT);

	//if (bind(ListenSocket, (struct sockaddr *)&localAddr, sizeof(localAddr)) < 0) {
	//	perror("bind failed");
	//	return 0;
	//}
	//connecting = true;
#pragma endregion Previously Working Code

#pragma region One Time server
	//int iResult;
	//// Initialize Winsock
	//iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//if (iResult != 0) {
	//	//printf("WSAStartup failed: %d\n", iResult);
	//	StringCchPrintf(status, statusSize, L"WSAStartup failed: %d\n", iResult);
	//	OutputDebugString(status);
	//	return status;
	//}

	//ZeroMemory(&hints, sizeof(hints));
	//hints.ai_family = AF_INET;
	//hints.ai_socktype = SOCK_STREAM;
	//hints.ai_protocol = IPPROTO_TCP;
	//hints.ai_flags = AI_PASSIVE;

	//// Resolve the server address and port
	//iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	//if (iResult != 0) {
	//	//printf("getaddrinfo failed with error: %d\n", iResult);
	//	StringCchPrintf(status, statusSize, L"getaddrinfo failed with error: %d\n", iResult);
	//	OutputDebugString(status);
	//	printf("getaddrinfo failed with error: %d\n", iResult);
	//	//WSACleanup();
	//	return status;
	//}

	//// Create a SOCKET for connecting to server
	//ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//if (ListenSocket == INVALID_SOCKET) {
	//	printf("socket failed with error: %ld\n", WSAGetLastError());
	//	StringCchPrintf(status, statusSize, L"socket failed with error : %ld\n", WSAGetLastError());
	//	OutputDebugString(status);
	//	freeaddrinfo(result);
	//	//WSACleanup();
	//	return status;
	//}

	//// Setup the TCP listening socket
	//iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	//if (iResult == SOCKET_ERROR) {
	//	printf("bind failed with error: %d\n", WSAGetLastError());
	//	StringCchPrintf(status, statusSize, L"bind failed with error: %d\n", WSAGetLastError());
	//	OutputDebugString(status);
	//	freeaddrinfo(result);
	//	closesocket(ListenSocket);
	//	//WSACleanup();
	//	return status;
	//}

	//freeaddrinfo(result);

	//printf("listening sockets\n");

	//iResult = listen(ListenSocket, SOMAXCONN);
	//if (iResult == SOCKET_ERROR) {
	//	//printf("listen failed with error: %d\n", WSAGetLastError());
	//	StringCchPrintf(status, statusSize, L"listen failed with error: %d\n", WSAGetLastError());
	//	OutputDebugString(status);
	//	closesocket(ListenSocket);
	//	//WSACleanup();
	//	return status;
	//}

	//printf("accepting connections\n");

	////Accept a client socket
	//ClientSocket = accept(ListenSocket, NULL, NULL);
	//if (ClientSocket == INVALID_SOCKET) {
	//	//printf("accept failed with error: %d\n", WSAGetLastError());
	//	StringCchPrintf(status, statusSize, L"accept failed with error: %d\n", WSAGetLastError());
	//	OutputDebugString(status);
	//	closesocket(ListenSocket);
	//	WSACleanup();
	//	return status;
	//}

	//printf("connection made, closing server socket");
	//// No longer need server socket
	//closesocket(ListenSocket);

	//printf("Begin receiving\n");
	//std::string parsedStr = "";
	// //Receive until the peer shuts down the connection
	//do {
	//	printf("trying to receive\n");
	//	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	//	if (iResult > 0) {
	//		//printf("Bytes received: %d, content:\"%s\", data:%s \n", iResult, recvbuf,Globals::data);
	//		parsedStr = recvbuf;
	//		recvbuf[iResult] = 0; //marks the end of the char array
	//		printf("received: %s,  length: %d, is sendMeData: %s\n",parsedStr, iResult, (parsedStr == "sendMeData")? "true":"false");
	//		//process what the client is receiving:
	//		//char* dataToSend = (recvbuf == "sendMeData") ?  Globals::data : recvbuf;
	//		// Echo the buffer back to the sender
	//		iSendResult = send(ClientSocket, recvbuf, iResult, 0);
	//		if (iSendResult == SOCKET_ERROR) {
	//			//printf("send failed with error: %d\n", WSAGetLastError());
	//			StringCchPrintf(status, statusSize, L"Bytes failed with error: %d\n", WSAGetLastError());
	//			OutputDebugString(status);
	//			closesocket(ClientSocket);
	//			WSACleanup();
	//			return status;
	//		}
	//		printf("Bytes sent: %d\n", iSendResult);
	//	}
	//	else if (iResult == 0)
	//		printf("Connection closing...\n");
	//	else {
	//		//printf("recv failed with error: %d\n", WSAGetLastError());
	//		StringCchPrintf(status, statusSize, L"recv failed with error: %d\n", WSAGetLastError());
	//		OutputDebugString(status);
	//		closesocket(ClientSocket);
	//		WSACleanup();
	//		return status;
	//	}

	//} while (iResult > 0);

	//// shutdown the connection since we're done
	//iResult = shutdown(ClientSocket, SD_SEND);
	//if (iResult == SOCKET_ERROR) {
	//	//printf("shutdown failed with error: %d\n", WSAGetLastError());
	//	StringCchPrintf(status, statusSize, L"shutdown failed with error: %d\n", WSAGetLastError());
	//	OutputDebugString(status);
	//	closesocket(ClientSocket);
	//	WSACleanup();
	//	return status;
	//}

	//// cleanup
	//closesocket(ClientSocket);
	//WSACleanup();
#pragma endregion One Time server

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return status;
	}

	SetupListener();
	if (ListenSocket == INVALID_SOCKET) {
		StringCchPrintf(status, statusSize, L"listen setup failed with error: %d\n", WSAGetLastError());
		OutputDebugString(status);
		return status;
	}

	int threadCount = 0;
	std::thread t[THREAD_MAX];

	while (threadCount < THREAD_MAX) {
		StringCchPrintf(status, statusSize, L"accepting connections\n");
		OutputDebugString(status);

		// Accept a client socket
		ClientSockets[threadCount] = accept(ListenSocket, NULL, NULL); 
		if (ClientSockets[threadCount] != INVALID_SOCKET) {
			OutputDebugString(L"client connected\n");
			//start separate threads to handle multiple clients
			t[threadCount] = std::thread(&ByteSender::ClientHandler, this, ClientSockets[threadCount]);
			//t.join();
			//ClientHandler();
			threadCount++;
		}
		else {
			StringCchPrintf(status, statusSize, L"accept failed with error: %d\n", WSAGetLastError());
			OutputDebugString(status);
		}
		
		
		StringCchPrintf(status, statusSize, L"Accept connections restart...\n");
		OutputDebugString(status);
	}

	//handles up to 3 concurrent connections and then stop after all threads finish.
	for (int i = 0; i < THREAD_MAX; i++) 
	{
		t[i].join();
	}

	WSACleanup();
	return L"server shutdown";
}
