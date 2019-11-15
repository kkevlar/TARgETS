#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <optional>
#include <iostream>

#include <string.h>
#include "message.h"
#include "webclient.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "25567"

WSADATA wsaData;
SOCKET ConnectSocket = INVALID_SOCKET;
struct addrinfo* result = NULL,
	* ptr = NULL,
	hints;
char recvbuf[DEFAULT_BUFLEN];
char sendbuf[DEFAULT_BUFLEN];
int iResult;
int recvbuflen = DEFAULT_BUFLEN;

int __cdecl clientbegin(MessageContext* context)
{
	initMessageHandler(context);

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo("localhost"/*"kevinkellar.com"*/, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	

	   
	return 0;
}

int sendbufindex = 0;

void clientMsgWrite(MessageId code, uint8_t* data, int len)
{
	uint8_t sml_len = len;
	clientwrite(&code, 1);
	clientwrite(&sml_len, 1);
	clientwrite(data, len);
}

void clientwrite(uint8_t* bytes, int length)
{
	for (int i = 0; i < length && sendbufindex < sizeof(sendbuf) / sizeof(char); i++, sendbufindex++)
	{
		sendbuf[sendbufindex] = bytes[i];
	}
}

void clientflush()
{
	// Send an initial buffer
	int big = sizeof(sendbuf);
	int amount = 0;
	int sz = sendbufindex;
	amount = sz;
	if (sz > big)
		amount = big;

	if (amount == 0)
	{
		return;
	}

	iResult = send(ConnectSocket, sendbuf, amount , 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
	}

	sendbufindex = 0;
}


 int code = 0;
 int length = -1;
 int bufindex = 0;
 uint8_t buf[256];

void clientread()
{
	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	
	if (iResult > 0)
	{
		for (int i = 0; i < iResult; i++)
		{
			if (code == 0)
			{
				code = recvbuf[i];
				bufindex = 0;
				length = -1;
			}
			else if (length == -1)
			{
				length = recvbuf[i];
				bufindex = 0;
			}
			else if (bufindex < length)
			{
				buf[bufindex++] = recvbuf[i];
			}
			else
			{
				handleMessage(code, buf, length);
				code = 0;
				length = -1;
				bufindex = 0;
				i--;
			}
		}
		
	}
	else if (iResult == 0)
		printf("Connection closed\n");
	else
		printf("recv failed with error: %d\n", WSAGetLastError());
}



void clientend()
{
	// cleanup

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
	}

	closesocket(ConnectSocket);
	WSACleanup();
}

