#pragma once
#include "..\PluginBase\Plugin.h"
#include "../Common/Packet.h"
#include "../Shared/TCPClient.h"

#define MAX_ACCPW 10

class CJSCore :
	public CPlugin
{
	TcpClient * Connection;
public:
	CJSCore();
	~CJSCore();
	
	// PluginBase
	PRESULT Start();
	PRESULT invoke(int proc, CVar * ArgList, int ArgCount);
	PRESULT Stop();

	void JStart(void);
	void LoginRequest(const char * ID, const char * PW, unsigned short Number, const char * UserIP);
};

// Protocol

#pragma pack(push,1)
struct PMSG_JOIN
{
	BYTE OP;
	BYTE Type;
	WORD Port;
	char Name[50];
	WORD Code;
};
struct LoginReq
{
	BYTE OP;
	char AccountId[MAX_ACCPW];
	char Password[MAX_ACCPW];
	WORD Number;
	char UserIP[17];
};
#pragma pack(pop)