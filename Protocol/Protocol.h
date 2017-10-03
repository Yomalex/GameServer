#pragma once
#include "C:\Users\Yomar\Documents\GitHub\GameServer\PluginBase\Plugin.h"
#include "../Shared/Protocol.h"
#include "../Shared/ObjShared.h"

class CProtocol :
	public CPlugin
{
public:
	CProtocol();
	~CProtocol();

	// PluginBase
	PRESULT Start();
	PRESULT invoke(int proc, CVar * ArgList, int ArgCount);
	PRESULT Stop();

	bool OnPacket(DWORD dwID, char * Packet, int Len, BYTE Head, bool Encrypt);
	bool OnJSPacket(char * Packet, int Len);
};

enum PROTOCOL_CODE
{
	PCTL_CODE = 0xF1
};
enum PCTLCODE_SCODE
{
	PCC_JOINRESULT,
	PCC_LOGIN,
};
enum LOGIN_RESULTS // PCTLCODE_SCODE::LOGIN
{
	LR_NONE,
	LR_OK,
	LR_BAD_ID_PW,
	LR_ALREADY_CONNECTED,
	LR_BANNED,
	LR_OLD_CLIENT,
	LR_CONN_ERROR,
	LR_MAX_TRY,
	LR_PAY_MISSING,
	LR_ENDED_DAYS,
	LR_ENDED_TIME,

};

#pragma pack(push,1)
struct PMSG_JOINRESULT : PBMSG_BASE
{
	BYTE Result;
	DWORD ItemCount;
};
struct CLOGIN_INFO
{
	char AccountId[10];
	char Password[10];
	DWORD dwTickCount;
	char CliVersion[7];
};
struct PMSG_LOGINRESULT : PBMSG_BASE
{
	BYTE Result;
	DWORD Number;
	char AccountId[10];
	int JSNumber;
	int DB;
	BYTE joominNumber[8];
};
struct PMSG_LINFORESULT // Clase packet
{
	BYTE Code;
	BYTE SCode;
	BYTE Result;
	enum
	{
		R_OK = 1,
		R_ID_OR_PW_ERR,
		R_ALREADY_CONNECTED,
		R_SERVER_FULL,
		R_BANNED,
		R_OLD_CLIENT_VERSION,
		R_CONN_ERR,
		R_MAX_TRYS,
		R_BILL_MISSING,
	};
};
#pragma pack(pop)