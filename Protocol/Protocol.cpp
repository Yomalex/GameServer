// Protocol.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "Protocol.h"
#include "../Common/Packet.h"
#include "CCharSet.h"

enum CallBacks
{
	_OnPacket = CALLBACK_START_INDEX,
	_OnJSPacket,
	_OnDSPacket,
};

char * szCallBack[] = {
	"OnPacket",
	"OnJSPacket",
	"OnDSPacket",
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

	this->szCallBackListNames = ppChar2vpChar(szCallBack);
	this->szEventListNames = ppChar2vpChar(szEvent);
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
		CALLBACK_CHKARG(ArgCount, 5);
		if(this->OnPacket(ArgList[0], ArgList[1], ArgList[2], ArgList[3].operator int(), ArgList[4].operator int()))
			return P_OK;
		return P_NO_IMPLEMENT;

	case _OnJSPacket:
		CALLBACK_CHKARG(ArgCount, 2);
		if (this->OnJSPacket(ArgList[0], ArgList[1]))
			return P_OK;
		return P_NO_IMPLEMENT;

	case _OnDSPacket:
		CALLBACK_CHKARG(ArgCount, 2);
		if (this->OnDSPacket(ArgList[0], ArgList[1]))
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
	char szError[128];
	sprintf_s(szError, "Packet %02X:%02X:%02X:%02X", pMixed->bHead.Head, Len, pMixed->Operation(), pMixed->Body()[0]);
	Message(7, szError);
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
					sprintf_s(szError, "No se encontro el sub comando %02X:%02X Size:%d", pMixed->Operation(), pMixed->Body()[0], Len);
					Error(OnError, PLUGIN_ERROR(P_INVALID_ARG), szError);
				}
			}
			break;
		case PCTL_LIVECLIENT:
			return true;
		case PCTL_CLIEN_DATA:
			switch (pMixed->Body()[0])
			{
			case 0x00:
				this->Loader->invoke("DSCore", "DataServerGetCharListRequest", 1, dwID);
				break;
			/*case 0x01:
				break;*/
			default:
				{
					sprintf_s(szError, "No se encontro el sub comando %02X:%02X Size:%d", pMixed->Operation(), pMixed->Body()[0], Len);
					Error(OnError, PLUGIN_ERROR(P_INVALID_ARG), szError);
				}
				return false;
			}
			return true;
		default:
		{
			sprintf_s(szError, "No se encontro el comando %02X, Size:%d", Head, Len);
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
			return true;
		case PCC_LOGIN:
		{
			PMSG_LOGINRESULT * p = (PMSG_LOGINRESULT *)bPacket;
			Packet<PMSG_LINFORESULT> pack(this, 0xC1);
			pack->Code = PCTL_CODE;
			pack->SCode = PCC_LOGIN;
			pack->Result = p->Result;
			if (pack->Result == 1 || pack->Result == 15)
			{
				OBJ_SET(p->Number, "AccountID", p->AccountId);
				OBJ_SET(p->Number, "JoominNumber", p->joominNumber);
				OBJ_SET(p->Number, "ConStatus", 1); // Conectado
				OBJ_SET(p->Number, "Type", 1); // Usuario
			}
			else if (pack->Result == 0)
			{
				pack->Result = 2;
			}
			this->Loader->invoke("IOCP", "Send", 3, p->Number, (char*)pack, pack.size());
		}
		return true;
		default:
		{
			char szError[128];
			sprintf_s(szError, "OPCode: %02X Missing", mixed->Operation());
			Error(OnError, PLUGIN_ERROR(P_INVALID_ARG), szError);
		}
	}

	return false;
}

bool CProtocol::OnDSPacket(char * Stream, int Len)
{
	PMMSG_BASE * mixed = (PMMSG_BASE *)Stream;

	switch (mixed->Operation())
	{
	case 1:
	{
		SDHP_CHARLISTCOUNT * pList = (SDHP_CHARLISTCOUNT*)Stream;

		Packet<PMSG_CHARLISTCOUNT> pack(this, 0xC1);
		pack->OP = 0xF3;
		pack->subcode = 0;
		pack->Count = pList->Count;
		OBJ_SET(pList->Number, "Magumsa", pList->Magumsa);
		pack->MaxClass = pList->Magumsa + 3;
		pack->MoveCnt = pList->MoveCnt;

		Packet<PMSG_CHARLIST_ENABLE_CREATION> pack2(this, 0xC1);
		pack2->OP = 0xDE;
		pack2->subcode = 0;
		pack2->EnableClass = 4;

		Send(pList->Number, pack2, pack2.size());

		CCharSet cs;
		for (UINT i =0; i < pList->Count; i++)
		{
			cs.ProcessInventory4(pList->CharList[i].dbInventory);
			cs.SetServerClass(pList->CharList[i].Class);
			pack->CharList[i].Index = pList->CharList[i].Index;
			pack->CharList[i].Level = pList->CharList[i].Level;
			pack->CharList[i].CtlCode = pList->CharList[i].CtlCode;
			pack->CharList[i].btGuildStatus = pList->CharList[i].btGuildStatus;
			memcpy(pack->CharList[i].Name, pList->CharList[i].Name, sizeof(pack->CharList[i].Name));
			memcpy(pack->CharList[i].CharSet, cs.SmallInventory, sizeof(pack->CharList[i].CharSet));
		}

		Send(pList->Number, pack, pack.size() - (5 - pList->Count)*sizeof(PMSG_CHARLIST));

		Packet<PMSG_SERVERTIME_SEND> pack3(this, 0xC3);
		pack3->OP = 0xFA;
		pack3->subcode = 0x02;
		GetSystemTime(&pack3->Time);

		CVar ret;
		Obj_GET(pList->Number, "Serial_Out", &ret);
		pack3.Serial = (int)ret;

		Send(pList->Number, pack3, pack3.size());
		OBJ_SET(pList->Number, "Serial_Out", pack3.Serial+1);
	}
	break;
	default:
		return false;
	}
	return true;
}
