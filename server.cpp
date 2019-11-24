//#include <windows.h>
#include "server.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string> 
//#include <winsock2.h>
#include <ws2tcpip.h>
#include "acquire.h"

#pragma comment (lib, "Ws2_32.lib")

serverClass::serverClass()
{
	setupSocket();
}

void serverClass::setupSocket() {
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
	}
	int enable = 1;
	if (setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&enable, sizeof(int))) {
		printf("SO_REUSEADDR Failed\n");
		WSACleanup();
	}

	//DWORD timeout = SOCKET_READ_TIMEOUT_SEC * 1000;
	/*if (setsockopt(ListenSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout))) {
		printf("SO_RCVTIMEO Failed\n");
		WSACleanup();
	}*/

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
	}

	freeaddrinfo(result);
}

void serverClass::startListen(cameraClass &kuroCam)
{
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
	}
	printf("Waiting for client...\n");
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
	}
	printf("Got connection from client.\n");
	//Connection made
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			//printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0) {
			printf("Waiting for client to reconnect...\n");
			recvbuf[0] = 0;
			ClientSocket = accept(ListenSocket, NULL, NULL);
			if (ClientSocket == INVALID_SOCKET) {
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(ListenSocket);
				WSACleanup();
			}
		}
		else {
			printf("recv failed with error: %d, iresult=%d\n", WSAGetLastError(), iResult);
			closesocket(ClientSocket);
			WSACleanup();
		}
		/*std::cout << recvbuf << std::endl;
		iSendResult = send(ClientSocket, recvbuf, iResult, 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
		}*/

		std::string bufString;
		bufString = std::string(recvbuf);
		std::string subStr = bufString.substr(0, 5);
		//printf("substr = %s\n", subStr.c_str());
		if (!subStr.compare(std::string("acqui"))) {
			int numFrames;
			numFrames = std::stoi(bufString.substr(7, 8), nullptr, 0);
			acquire(kuroCam, numFrames);
			bufString = "";
			recvbuf[0] = 0;
			printf("Listening ...\n\n");
		}
		else if (!subStr.compare(std::string("alive"))) {
			printf(".\n");
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
			}
			//printf("Bytes sent: %d\n", iSendResult);
			sentbuf[0] = 0;
			bufString = "";
			recvbuf[0] = 0;
			//printf("Listening ...\n\n");
		}
		else if (!subStr.compare(std::string("setup"))) {
			printf("Setup\n");
			std::string sendStr = cameraSetupInformation(kuroCam);
			strcpy_s(sentbuf, sizeof(char)*(int)sendStr.length()+1, sendStr.c_str());
			//std::cout << sendStr.length() << std::endl;
			iSendResult = send(ClientSocket, sentbuf, (int)sendStr.length(), 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send setup failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
			}
			//printf("Bytes sent: %d\n", iSendResult);
			sentbuf[0] = 0;
			bufString = "";
			recvbuf[0] = 0;
			printf("Listening ...\n\n");
		}
		else if (!subStr.compare(std::string("clear"))) {
			printf("Got clear command\n");
			this->keepServerRunning = false;
			iResult = -1;
		}

	} while (iResult >= 0);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
	}
}
serverClass::~serverClass()
{
	closesocket(ListenSocket);
	closesocket(ClientSocket);
	WSACleanup();
}