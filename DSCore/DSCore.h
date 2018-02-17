#pragma once
#include "..\PluginBase\Plugin.h"
#include "../Common/Packet.h"
#include "../Shared/TCPClient.h"
#include "../Shared/Protocol.h"
#include "../Shared/ObjShared.h"

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
	void DataServerGetCharListRequest(int iID);
	void Write(void * Stream, int len);

	bool OnDSPacket(char * Stream, int len);

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
struct SDHP_GETCHARLIST
{
	//WZ_HEAD_B h;	// C1:01
	BYTE OP;
	char Id[10];	// 3
	short Number;	// E
};
#pragma pack(pop)