#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <mutex>
#include <optional>

#include <string.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "message.h"
#include "webclient.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

#define RECV_BUFLEN 512
#define SEND_BUFLEN (512 * 16)

#define DEFAULT_PORT "25567"

WSADATA wsaData;
SOCKET ConnectSocket = INVALID_SOCKET;
struct addrinfo *result = NULL, *ptr = NULL, hints;
char recvbuf[RECV_BUFLEN];
char sendbuf[SEND_BUFLEN];
int iResult;
int recvbuflen = RECV_BUFLEN;

int __cdecl clientbegin(MessageContext* context)
{
    initMessageHandler(context);

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("localhost" /*"kevinkellar.com"*/, DEFAULT_PORT,
                          &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        // Create a SOCKET for connecting to server
        ConnectSocket =
            socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    return 0;
}

int sendbufindex = 0;
std::mutex mtx_write;

void clientMsgWrite(MessageId code, uint8_t* data, int len)
{
    uint8_t sml_len = len;
    clientwrite(&code, 1);
    clientwrite(&sml_len, 1);
    clientwrite(data, len);
}

void clientwrite(uint8_t* bytes, int length)
{
    mtx_write.lock();
    for (int i = 0; i < length && sendbufindex < sizeof(sendbuf) / sizeof(char);
         i++, sendbufindex++)
    {
        sendbuf[sendbufindex] = bytes[i];
    }
    mtx_write.unlock();
}

void clientflush()
{
    // Send an initial buffer

    int big = sizeof(sendbuf);
    int amount = 0;

    if (sendbufindex == 0) return;

    mtx_write.lock();
    int sz = sendbufindex;
    amount = sz;
    if (sz > big) amount = big;

    if (amount == 0)
    {
        goto end;
    }

    iResult = send(ConnectSocket, sendbuf, amount, 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
    }

end:
    sendbufindex = 0;
    mtx_write.unlock();
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

bool keepReading;

void repeatedRead()
{
	keepReading = 1;
    while (keepReading)
    {
        clientread();
		 clientflush();
    }
}

void stopRepeatedRead()
{
	keepReading = 0;
}

bool keepWriting;

void repeatedWrite()
{
    keepWriting = 1;
    while (keepWriting)
    {
        clientflush();
    }
}

void stopRepeatedWrite() { keepWriting = 0; }


void clientend()
{
    // cleanup

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
    }

    closesocket(ConnectSocket);
    WSACleanup();
}
