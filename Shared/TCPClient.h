#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include "../PluginBase/Plugin.h"

class TcpClient
{
	SOCKET s;
	BYTE Buffering[2048];
	CPlugin * pDispacher;
	HANDLE hRead;
	bool alive;
public:
	TcpClient(CPlugin * target);
	~TcpClient();

	bool Connect(const char * szDN, WORD Port);
	bool isConnected() { return this->alive; }
	void Write(void * Src, int iLen);

	void Disconnect();

	friend DWORD WINAPI thRead(TcpClient * lpVoid);
};