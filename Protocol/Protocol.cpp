// Protocol.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "Protocol.h"
#include "../Common/Packet.h"

enum CallBacks
{
	_OnPacket = CALLBACK_START_INDEX,
	_OnJSPacket,
};

char * szCallBack[] = {
	"OnPacket",
	"OnJSPacket",
	nullptr
};

enum Events
{
	OnError,
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
	case _OnPacket:
		CALLBACK_CHKARG(ArgCount, 3);
		if(this->OnPacket(ArgList[0], ArgList[1], ArgList[2], ArgList[3].operator int(), ArgList[4].operator int()))
			return P_OK;
		return P_NO_IMPLEMENT;

	case _OnJSPacket:
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
	PMMSG_BASE * pMixed = (PMMSG_BASE*)pBody;
	switch (Head)
	{
		case PCTL_CODE:
			switch (pMixed->Body()[0])
			{
			case PCC_LOGIN:
			{
				CLOGIN_INFO * pInfo = (CLOGIN_INFO*)pBody;
				this->Loader->invoke("JSCore", "LoginRequest", 4, pInfo->AccountId, pInfo->Password, dwID, "127.0.0.1");
			}
			return true;
			default:
			{
				char szError[128];
				sprintf_s(szError, "No se encontro el sub comando %02X", pMixed->Body()[0]);
				Error(OnError, PLUGIN_ERROR(P_INVALID_ARG), szError);
			}
			}
			break;
		default:
		{
			char szError[128];
			sprintf_s(szError, "No se encontro el comando %02X", Head);
			Error(OnError, PLUGIN_ERROR(P_INVALID_ARG), szError);
		}
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
			OBJ_SET(p->Number, "ConStatus", 1); // Conectado
			OBJ_SET(p->Number, "Type", 1); // Usuario
		}
			break;
		default:
		{
			char szError[128];
			sprintf_s(szError, "OPCode: %02X Missing", mixed->Operation());
			Error(OnError, PLUGIN_ERROR(P_INVALID_ARG), szError);
		}
	}

	return false;
}
