#include "stdafx.h"
#include "TCPClient.h"

char * szETcp_Error = "OnError";
int iETcp_Error;

DWORD WINAPI thRead(TcpClient * tcp)
{
	int readed;
	CVar Args1[2];
	CVar Args[5];
	Args[0] = tcp->pDispacher->GetPluginName();
	Args[4] = "";
	Args1[0] = tcp->Buffering;
	tcp->alive = true;
	while ( (readed = recv(tcp->s, (char*)tcp->Buffering, ARRAYSIZE(tcp->Buffering), 0)) > 0 )
	{
		Args1[1] = readed;
		if (tcp->pDispacher->DispCallBack(0, Args1, 2)!=P_OK)
		{
			Args[1] = __FILE__;
			Args[2] = __LINE__;
			Args[4] = "Callback cant be finded";
			tcp->pDispacher->DispCallBack(iETcp_Error, Args, 5);
		}
	}
	tcp->Disconnect();
	return 0;
}

TcpClient::TcpClient(CPlugin * target)
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	this->pDispacher = target;
	this->alive = false;

	iETcp_Error = target->AddEvent(szETcp_Error);
}

TcpClient::~TcpClient()
{
	WSACleanup();
}

bool TcpClient::Connect(const char * szDN, WORD Port)
{
	hostent * he = gethostbyname(szDN);
	if (he == nullptr) return false;

	this->s = socket(he->h_addrtype, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in target;
	target.sin_family = he->h_addrtype;
	target.sin_addr.s_addr = *(u_long*)he->h_addr_list[0];
	target.sin_port = htons(Port);

	int Trys = 3;
	CVar Args[5];
	Args[0] = this->pDispacher->GetPluginName();
	Args[4] = "";

	while ( connect(this->s, (sockaddr*)&target, sizeof(target)) == SOCKET_ERROR)
	{
		Args[3] = WSAGetLastError();
		switch (WSAGetLastError())
		{
		case WSAECONNREFUSED:
		case WSAETIMEDOUT: {
			Args[1] = __FILE__;
			Args[2] = __LINE__;
			this->pDispacher->DispCallBack(iETcp_Error, Args, 5);
				if (Trys-- < 0)
				{
					closesocket(this->s);
					return false;
				}
			Sleep(1000);
		}
			break;
		default:
			Args[1] = __FILE__;
			Args[2] = __LINE__;
			this->pDispacher->DispCallBack(iETcp_Error, Args, 5);
			return false;
		}
	}

	this->hRead = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)thRead, this, 0, nullptr);

	while (!this->alive) Sleep(100);

	return true;
}

void TcpClient::Write(void * Src, int iLen)
{
	if (!this->alive)
	{
		CVar Args[5];
		Args[0] = this->pDispacher->GetPluginName();
		Args[4] = "[TCP] Connection Dead";
		Args[1] = __FILE__;
		Args[2] = __LINE__;
		Args[3] = WSAGetLastError();
		this->pDispacher->DispCallBack(iETcp_Error, Args, 5);
		return;
	}
	if (iLen != send(this->s, (const char *)Src, iLen, 0))
	{
		CVar Args[5];
		Args[0] = this->pDispacher->GetPluginName();
		Args[4] = "";
		Args[1] = __FILE__;
		Args[2] = __LINE__;
		Args[3] = WSAGetLastError();
		this->pDispacher->DispCallBack(iETcp_Error, Args, 5);
		this->Disconnect();
	}
}

void TcpClient::Disconnect()
{
	this->alive = false;
	closesocket(this->s);
	TerminateThread(this->hRead, 0);
}
