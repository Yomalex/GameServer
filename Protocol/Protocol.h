#pragma once
#include "..\PluginBase\Plugin.h"
#include "../Shared/Protocol.h"
#include "../Shared/ObjShared.h"
#include "../Shared/ConShared.h"
#include "../Shared/IoShared.h"

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
	bool OnDSPacket(char * Packet, int Len);
};

enum PROTOCOL_CODE
{
	PCTL_CODE = 0xF1,
	PCTL_,
	PCTL_CLIEN_DATA,
	PCTL_LIVECLIENT = 0x0E,
};
enum PCTLCODE_SCODE
{
	// Packet Control Code, Join Result
	PCC_JOINRESULT,
	// Packet Control Code, Login
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
struct CLOGIN_INFO : PBMSG_BASE
{
	BYTE SH;
	char AccountId[10];
	char Password[10];
	DWORD dwTickCount;
	char CliVersion[7];
};
struct PMSG_LOGINRESULT : PBMSG_BASE
{
	BYTE Result;
	WORD Number;
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
struct SDHP_CHARLIST
{
	BYTE Index;	// 0
	char Name[10];	// 1
	BYTE Padding; // B
	WORD Level;	// C
	BYTE Class;	// E
	BYTE CtlCode;	// F
	BYTE dbInventory[48];	// 10
	BYTE DbVersion;	// 40
	BYTE btGuildStatus;	// 41
};
struct SDHP_CHARLISTCOUNT : PWMSG_BASE
{
	short Number;	// 4
	BYTE Count;	// 6
	BYTE Padding;//7
	int DbNumber;	// 8
	BYTE Magumsa;	// C
	char AccountId[11];	// D
	BYTE MoveCnt;	// 18
#ifdef EXPINV
	BYTE ExpandedWarehouse;
#endif
	BYTE Padding2[3];
	SDHP_CHARLIST CharList[1];
};

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Char List Count Packet
*	Direction : GameServer -> Client
*  Code     : 0xC1
*	HeadCode : 0xF3
*	SubCode  : 0x00
*	BASE PACKET - Complete with PMSG_CHARLIST
*/

struct PMSG_CHARLIST
{
	BYTE Index;	// 0
	char Name[10];	// 1
	WORD Level;	// C
	BYTE CtlCode;	// E
	BYTE CharSet[CHAR_SET_SIZE];	// F
	BYTE btGuildStatus;	// 21
};

struct PMSG_CHARLISTCOUNT
{
	//WZ_HEAD_B h;
	BYTE OP;
	BYTE subcode;
	BYTE MaxClass;	// 4
	BYTE MoveCnt;	// 5
	BYTE Count;	// 6
	PMSG_CHARLIST CharList[5];
};

struct PMSG_CHARLIST_ENABLE_CREATION
{
	//WZ_HEAD_B h; // C1:DE:00
	BYTE OP;
	BYTE subcode; // 0
	BYTE EnableClass;// 4
};

struct PMSG_SERVERTIME_SEND
{
	//PBMSG_HEAD2 h; //C1:LEN:FA:02:SYSTEMTIME
	BYTE OP;
	BYTE subcode;
	SYSTEMTIME Time;
};
#pragma pack(pop)