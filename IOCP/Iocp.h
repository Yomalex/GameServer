#pragma once
#define MAX_THREADS 4
#define MAX_CLIENTS 1000
#define MAX_BUFFERS 1024*2

struct Client
{
	DWORD ID;
	SOCKET s;
	DWORD IPAddress;
	DWORD LifeTime;
	char BufferIn[MAX_BUFFERS];

	//WSA
	WSABUF wsaBuf;
	DWORD dwFlags;
	WSAOVERLAPPED wsaOVI;
	WSAOVERLAPPED wsaOVO;

	Client()
	{
		wsaBuf.buf = BufferIn;
		wsaBuf.len = MAX_BUFFERS;
	}

	operator WSABUF*()
	{
		return &wsaBuf;
	}
};

class CIocp: public CPlugin
{
public:
	CIocp();
	~CIocp();

	// Plugin Base
	PRESULT Start();
	PRESULT Stop();
	PRESULT invoke(int proc, CVar * ArgList, int ArgCount);

	// IOCP
	bool Listen(short wPort);
	void Close(UINT ID);
	void Send(UINT ID, void*Pointer, UINT uiLen);

	friend DWORD WINAPI th_Accept(CIocp * iocp);
	friend DWORD WINAPI th_Worker(CIocp * iocp);

private:
	HANDLE m_hMutex;
	HANDLE m_hIocp;
	HANDLE m_hIoWorkers[MAX_THREADS];
	HANDLE m_hIoAccept;

	UINT LastFree;
	Client m_ClientList[MAX_CLIENTS];

	// Winsock
	WSADATA m_wsData;
	SOCKET m_sServer;
};

