// IOCP.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "Iocp.h"

char * szEvents[] = {
	"OnConnect",
	"OnDisconnect",
	"OnData",
	"OnError",
	nullptr
};
char * szProcs[] = {
	"Listen",
	"Close",
	"Send",
	nullptr
};

DWORD WINAPI th_Accept(CIocp * iocp)
{
	sockaddr_in Incoming;
	CVar Args[2];
	int size;
	Client * pAccepted;
	while (true)
	{
		size = sizeof(Incoming);

		for (register UINT i = iocp->LastFree; i < MAX_CLIENTS; i++)
		{
			if (iocp->m_ClientList[i].s != INVALID_SOCKET) continue;

			pAccepted = iocp->m_ClientList + i;
			pAccepted->ID = i;
			iocp->LastFree = max(iocp->LastFree, i);

			break;
		}

		pAccepted->s = accept(iocp->m_sServer, (sockaddr*)&Incoming, &size);
		if (pAccepted->s == INVALID_SOCKET) continue;
		pAccepted->IPAddress = Incoming.sin_addr.S_un.S_addr;
		pAccepted->LifeTime = GetTickCount();

		CreateIoCompletionPort((HANDLE)pAccepted->s, iocp->m_hIocp, (ULONG_PTR)pAccepted, 0);

		char * szAddr = inet_ntoa(Incoming.sin_addr);
		size_t len = strlen(szAddr) + 1;
		Args[0] = pAccepted->ID;
		Args[1] = (char*)malloc(len);
		strcpy_s(Args[1], len, szAddr);

		if (iocp->DispCallBack(0, Args, 2) != P_OK)
		{
			iocp->Close(pAccepted->ID);
		}
		else
		{
			if (WSARecv(
				pAccepted->s,
				*pAccepted,
				1,
				nullptr,
				&pAccepted->dwFlags,
				&pAccepted->wsaOVI,
				nullptr
			) != 0)
			{
				DWORD dwErr = WSAGetLastError();
				if (dwErr != WSA_IO_PENDING)
				{
					CVar Args[5];
					Args[0] = iocp->m_szName;
					Args[1] = __FILE__;
					Args[2] = __LINE__;
					Args[3] = dwErr;
					Args[4] = (void*)nullptr;
					iocp->DispCallBack(3, Args, 5); //Dispatch OnError Event
					iocp->Close(pAccepted->ID);
				}
			}
		}

		free(Args[1]);
	}
}
DWORD WINAPI th_Worker(CIocp * iocp)
{
	DWORD m_dwBytesTransferred;
	Client * client;
	LPOVERLAPPED m_lpOverlapped;
	CVar Args[5];

	while (true)
	{
		if (
			!GetQueuedCompletionStatus(
				iocp->m_hIocp,
				&m_dwBytesTransferred,
				(PULONG_PTR)&client,
				&m_lpOverlapped,
				INFINITE
			)
			)
		{
			DWORD m_dwLastError;
			// Error function
			switch (m_dwLastError = GetLastError())
			{
			case ERROR_IO_PENDING:
				continue;
			default:
				Args[0] = iocp->m_szName;
				Args[1] = __FILE__;
				Args[2] = __LINE__;
				Args[3] = m_dwLastError;
				Args[4] = (void*)nullptr;
				iocp->DispCallBack(3, Args, 5); //Dispatch OnError Event
				return 0;
			}
		}

		if (!m_dwBytesTransferred)
		{
			iocp->Close(client->ID);
		}

		if (m_lpOverlapped == &client->wsaOVI) // Se recibio un paquete
		{
			Args[0] = client->ID;
			Args[1] = client->BufferIn;
			Args[2] = m_dwBytesTransferred;
			if(iocp->DispCallBack(2, Args, 3) != P_OK)
			{
				//iocp->DispCallBack(3, Args, 5); //Dispatch OnError Event
				iocp->Close(client->ID);
			}
			else
			{
				client->dwFlags = 0;
				if (WSARecv(
					client->s,
					*client,
					1,
					nullptr,
					&client->dwFlags,
					&client->wsaOVI,
					nullptr
				) != 0)
				{
					DWORD dwErr = WSAGetLastError();
					if (dwErr != WSA_IO_PENDING)
					{
						Args[0] = iocp->m_szName;
						Args[1] = __FILE__;
						Args[2] = __LINE__;
						Args[3] = dwErr;
						Args[4] = (void*)nullptr;
						iocp->DispCallBack(3, Args, 5); //Dispatch OnError Event
						iocp->Close(client->ID);
					}
				}
			}
		}
	}
}

CIocp::CIocp()
{
	ZeroMemory(this->m_szName, sizeof(this->m_szName));
	strcpy_s(this->m_szName, "IOCP");
	this->m_dwVersion = PLUGIN_MAKEVERSION(1, 0, 0, 0);
	this->szEventListNames = ppChar2vpChar(szEvents);
	//this->szCallBackListNames = szCallBacks;
	this->szProcListNames = ppChar2vpChar(szProcs);

	for (register UINT i = 0; i < MAX_CLIENTS; i++)
	{
		this->m_ClientList[i].s = INVALID_SOCKET;
	}
}

CIocp::~CIocp()
{
}

PRESULT CIocp::Start()
{
	// NT Kernetl
	this->m_hMutex = CreateMutex(nullptr, TRUE, TEXT("IocpMutex"));
	this->m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

	PRESULT result = P_OK;

	for (int i = 0; i<MAX_THREADS; i++)
	{
		this->m_hIoWorkers[i] = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)th_Worker, this, 0, nullptr);
		if (this->m_hIoWorkers[i] == INVALID_HANDLE_VALUE) result = P_ERROR;
	}

	// Winsock Part
	WSAStartup(MAKEWORD(2, 2), &this->m_wsData);
	this->m_sServer = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, nullptr, 0, WSA_FLAG_OVERLAPPED);

	return result;
}

PRESULT CIocp::Stop()
{
	for (register UINT i = 0; i < MAX_CLIENTS; i++)
	{
		this->Close(i);
	}

	TerminateThread(this->m_hIoAccept, 0);
	closesocket(this->m_sServer);
	WSACleanup();

	for (int i = 0; i<MAX_THREADS; i++)
	{
		TerminateThread(this->m_hIoWorkers[i], 0);
		CloseHandle(this->m_hIoWorkers[i]);
		this->m_hIoWorkers[i] = INVALID_HANDLE_VALUE;
	}

	CloseHandle(this->m_hIocp);
	CloseHandle(this->m_hMutex);

	return P_OK;
}

PRESULT CIocp::invoke(int proc, CVar * ArgList, int ArgCount)
{
	switch (proc)
	{
	case 0:
		if (ArgCount != 1) return P_INVALID_ARG;
		this->Listen(*ArgList);
		break;
	case 1:
		if (ArgCount != 1) return P_INVALID_ARG;
		this->Close(*ArgList);
		break;
	case 2:
		if (ArgCount != 3) return P_INVALID_ARG;
		this->Send(ArgList[0], ArgList[1], ArgList[2]);
		break;
	default:
		return P_INVALID_ARG;
	}
	return P_OK;
}

bool CIocp::Listen(short wPort)
{
	sockaddr_in ServerInfo = { 0 };
	ServerInfo.sin_addr.s_addr = INADDR_ANY;
	ServerInfo.sin_family = AF_INET;
	ServerInfo.sin_port = htons(wPort);

	bind(this->m_sServer, (const sockaddr*)&ServerInfo, sizeof(ServerInfo));
	listen(this->m_sServer, MAX_THREADS);

	this->m_hIoAccept = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)th_Accept, this, 0, nullptr);

	if (this->m_hIoAccept == INVALID_HANDLE_VALUE) return false;

	return false;
}

void CIocp::Close(UINT ID)
{
	if (this->m_ClientList[ID].s == INVALID_SOCKET) return;

	CVar Args[4];
	Args[0] = ID;
	Args[1] = GetTickCount() - this->m_ClientList[ID].LifeTime;

	this->DispCallBack(1, Args, 2);
	closesocket(this->m_ClientList[ID].s);
	this->m_ClientList[ID].s = INVALID_SOCKET;
	this->LastFree = min(this->LastFree, ID);
}

void CIocp::Send(UINT ID, void * Pointer, UINT uiLen)
{
	WSABUF buf;
	buf.buf = (char*)Pointer;
	buf.len = uiLen;
	if (WSASend(
		this->m_ClientList[ID].s,
		&buf,
		1,
		nullptr,
		0,
		&this->m_ClientList[ID].wsaOVO,
		nullptr) != 0)
	{
		DWORD dwErr = WSAGetLastError();
		if (dwErr != WSA_IO_PENDING)
		{
			CVar Args[5];
			Args[0] = this->m_szName;
			Args[1] = __FILE__;
			Args[2] = __LINE__;
			Args[3] = dwErr;
			Args[4] = (void*)nullptr;
			this->DispCallBack(3, Args, 5); //Dispatch OnError Event
			this->Close(ID);
		}
	}
}
