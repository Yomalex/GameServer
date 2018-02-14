#pragma once
#include "..\PluginBase\Plugin.h"
#include "../Common/Packet.h"
#include "../Shared/TCPClient.h"
class CDSCore :
	public CPlugin
{
	TcpClient * ConnectionDS1, *ConnectionDS2;
	HANDLE hThreads[2];
	static CDSCore * Instance;
public:
	CDSCore();
	~CDSCore();

	// PluginBase
	PRESULT Start();
	PRESULT invoke(int proc, CVar * ArgList, int ArgCount);
	PRESULT Stop();

	void DStart();

	static DWORD WINAPI ThConnect(VOID*);
};

#pragma pack(push,1)
struct PMSG_JOIN
{
	BYTE OP;
	BYTE Type;
	WORD Port;
	char Name[50];
	WORD Code;
};
#pragma pack(pop)