// Protocol.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "Protocol.h"
#include "../Common/Packet.h"

char * szCallBack[] = {
	"OnPacket",
	"OnJSPacket",
	nullptr
};

char * szEvent[] = {
	"OnError",
	nullptr
};


CProtocol::CProtocol()
{
	strcpy_s(this->m_szName, "Protoco");
	this->m_dwVersion = PLUGIN_MAKEVERSION(1, 0, 7, 0);
}


CProtocol::~CProtocol()
{
}

PRESULT CProtocol::Start()
{
	if (this->FindDependency("CONSOLE", PLUGIN_MAKEVERSION(1, 0, 0, 0))) return P_MISSING_DEPENDENCY;
	if (this->FindDependency("MuCore", PLUGIN_MAKEVERSION(1, 0, 0, 0))) return P_MISSING_DEPENDENCY;
	if (this->FindDependency("ObjCore", PLUGIN_MAKEVERSION(1, 0, 0, 0))) return P_MISSING_DEPENDENCY;
	if (this->FindDependency("JSCore", PLUGIN_MAKEVERSION(0, 99, 0, 0))) return P_MISSING_DEPENDENCY;

	return P_OK;
}

PRESULT CProtocol::invoke(int proc, CVar * ArgList, int ArgCount)
{
	switch (proc)
	{
	case CALLBACK_INDEX(0):
		CALLBACK_CHKARG(ArgCount, 3);
		if(this->OnPacket(ArgList[0], ArgList[1], ArgList[2], ArgList[3].operator int(), ArgList[4].operator int()))
			return P_OK;
		return P_NO_IMPLEMENT;

	case CALLBACK_INDEX(1):
		CALLBACK_CHKARG(ArgCount, 2);
		if (this->OnJSPacket(ArgList[0], ArgList[1]))
			return P_OK;
		return P_NO_IMPLEMENT;
	}
	return PRESULT();
}

PRESULT CProtocol::Stop()
{
	return P_OK;
}

bool CProtocol::OnPacket(DWORD dwID, char * pBody, int Len, BYTE Head, bool Encrypt)
{
	switch (Head)
	{
	case PCTL_CODE:
		switch ((BYTE)*pBody)
		{
		case PCC_LOGIN:
		{
			CLOGIN_INFO * pInfo = (CLOGIN_INFO*)(pBody + 1);
			this->Loader->invoke("JSCore", "LoginRequest", 4, pInfo->AccountId, pInfo->Password, dwID, "127.0.0.1");
		}
		return true;
		}
		break;
	}

	return false;
}

bool CProtocol::OnJSPacket(char * bPacket, int Len)
{
	PMMSG_BASE * mixed = (PMMSG_BASE *)bPacket;

	switch (mixed->Operation())
	{
	case PCC_JOINRESULT:
	{
		PMSG_JOINRESULT * p = (PMSG_JOINRESULT*)bPacket;
		printf("Connection to JS Result: %d\n", p->Result);
	}
		break;
	case PCC_LOGIN:
	{
		PMSG_LOGINRESULT * p = (PMSG_LOGINRESULT *)bPacket;
		Packet<PMSG_LINFORESULT> pack(this, 0xC1);
		pack->Code = PCTL_CODE;
		pack->SCode = PCC_LOGIN;
		pack->Result = p->Result;
		this->Loader->invoke("IOCP", "Send", 3, p->Number, (char*)pack, pack.size());
		OBJ_SET(p->Number, "AccountID", p->AccountId);
		OBJ_SET(p->Number, "JoominNumber", p->joominNumber);
	}
		break;
	default:
	{
		char szError[128];
		sprintf_s(szError, "OPCode: %02X Missing", mixed->Operation());
		this->DispCallBack(0, 5, this->m_szName, __FILE__, __LINE__, PLUGIN_ERROR(P_NO_IMPLEMENT), szError);
	}
	}

	return false;
}
