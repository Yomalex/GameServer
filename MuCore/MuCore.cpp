// MuCore.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "MuCore.h"
#include "../PluginBase/Loader.h"


char * szProc[] = {
	"",
	nullptr
};
char * szEvents[] =
{
	"OnPacket", // Index, Packet, Len
	nullptr
};
char * szCallBack[] = {
	"OnConnect",
	"OnData",
	"OnDisconnect",
	nullptr
};

CMuCore::CMuCore()
{
	this->szCallBackListNames = ppChar2vpChar(szCallBack);
	this->szProcListNames = ppChar2vpChar(szProc);
	strcpy_s(this->m_szName, "MuCore");
	this->m_dwVersion = PLUGIN_MAKEVERSION(1, 0, 0, 0);
}


CMuCore::~CMuCore()
{
}

PRESULT CMuCore::Start()
{
	if (this->FindDependency("CONSOLE", PLUGIN_MAKEVERSION(1, 0, 0, 0))) return P_MISSING_DEPENDENCY;
	if (this->FindDependency("IOCP", PLUGIN_MAKEVERSION(1, 0, 0, 0))) return P_MISSING_DEPENDENCY;

	return P_OK;
}

PRESULT CMuCore::invoke(int proc, CVar * ArgList, int ArgCount)
{
	switch (proc)
	{
	case CALLBACK_INDEX(0): // On Connect
		CALLBACK_CHKARG(ArgCount,2);
		return this->OnConnect(ArgList[0], ArgList[1]);
	case CALLBACK_INDEX(1): // On Packet
		CALLBACK_CHKARG(ArgCount, 3);
		this->OnData(ArgList[0], ArgList[1], ArgList[2]);
		return P_OK;
	case CALLBACK_INDEX(2): // On Disconnect
		CALLBACK_CHKARG(ArgCount, 2);
		return this->OnDisconnect(ArgList[0], ArgList[1]);
	}
	return P_NO_IMPLEMENT;
}

PRESULT CMuCore::Stop()
{
	return P_OK;
}

PRESULT CMuCore::OnConnect(DWORD dwIndex, char * szIP)
{
	char szMessage[1024];
	sprintf_s(szMessage, "Connected %s on Index %d", szIP, dwIndex);
	this->Loader->invoke("CONSOLE", "Message", 3, 7, this->m_szName, szMessage);
	this->SCPJoinResult(dwIndex, 0);
	return P_OK;
}

void CMuCore::OnData(DWORD dwIndex, char * Buffer, int iSize)
{
	if (iSize <= 0) return;

	Packet<void> incoming(this);
	CVar Args[7];
	Args[0] = dwIndex;
	do
	{
		incoming = (BYTE*)Buffer;
		if (!incoming.size())
		{
			this->Loader->invoke("CONSOLE", "Message", 3, 12, this->m_szName, "Bad Packet");
			this->Loader->invoke("IOCP", "Close", 1, dwIndex);
			return;
		}
		Buffer += incoming.size();
		iSize -= incoming.size();
		Args[1] = incoming.body()+1;
		Args[2] = incoming.size();
		Args[3] = incoming.code();
		Args[4] = incoming.encrypt();
		this->DispCallBack(0, Args, 5);
	} while (iSize > 0);
}

PRESULT CMuCore::OnDisconnect(DWORD dwIndex, DWORD dwLifeTime)
{
	char szMessage[1024];
	sprintf_s(szMessage, "Disonnected %d was alive %fseg", dwIndex, (float)dwLifeTime/1000.0f);
	this->Loader->invoke("CONSOLE", "Message", 3, 7, this->m_szName, szMessage);
	return P_OK;
}

#pragma pack(push,1)
struct PMSG_JOINRESULT
{
	//WZ_HEAD_B h; // C1:F1
	BYTE code;
	BYTE scode; // 3
	BYTE result; // 4
	BYTE NumberH; // 5
	BYTE NumberL; // 6
	BYTE CliVersion[5]; // 7
};
#pragma pack(pop)

// Envio de respuesta de union al servidor
void CMuCore::SCPJoinResult(int iID, int result)
{
	Packet<PMSG_JOINRESULT> pack(this,0xC1);
	pack->code = 0xF1;
	pack->scode = 0;
	pack->result = result;
	pack->NumberH = (iID & 0xff00) >> 8;
	pack->NumberL = iID & 0xff;
	memcpy(pack->CliVersion, this->Property("CliVersion"), 5);

	this->Loader->invoke("IOCP", "Send", 3, iID, (char*)pack, pack.size());
}
