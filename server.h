#pragma once
#include <winsock2.h>
#include "cameraClass.h"

#define DEFAULT_BUFLEN 100
#define DEFAULT_PORT "54321"
#define SOCKET_READ_TIMEOUT_SEC 2


class serverClass
{
public:
	serverClass();
	 bool keepServerRunning = true;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	WSADATA wsaData;
	int iResult;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	char sentbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	void startListen(cameraClass &kuroCam);
	void setupSocket();

	~serverClass();
};
