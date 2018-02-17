// DSCore.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "DSCore.h"
#include "../Shared/ConShared.h"


char * szEvents[] =
{
	"OnDSPacket", // Packet, Len
	"OnError",
	nullptr
};
char * szProc[] =
{
	"DStart",
	"DataServerGetCharListRequest",
	nullptr
};
char * szCallBacks[]=
{
	"OnDSPacket",
	nullptr,
};
enum CallBacks
{
	_OnDSPacket = CALLBACK_START_INDEX,
};

CDSCore * CDSCore::Instance;

CDSCore::CDSCore()
{
	strcpy_s(this->m_szName, "DSCore");
	this->szProcListNames = ppChar2vpChar(szProc);
	this->szEventListNames = ppChar2vpChar(szEvents);
	this->szCallBackListNames = ppChar2vpChar(szCallBacks);

	this->m_dwVersion = PLUGIN_MAKEVERSION(0, 99, 0, 0);
	this->ConnectionDS1 = new TcpClient(this);
	this->ConnectionDS2 = new TcpClient(this);

	this->Instance = this;
}


CDSCore::~CDSCore()
{
	delete this->ConnectionDS1;
	delete this->ConnectionDS2;
}

PRESULT CDSCore::Start()
{
	if (this->FindDependency("MuCore", PLUGIN_MAKEVERSION(1, 0, 0, 0))) return P_MISSING_DEPENDENCY;

	return P_OK;
}

PRESULT CDSCore::invoke(int proc, CVar * ArgList, int ArgCount)
{
	switch (proc)
	{
	case 0: this->DStart(); return P_OK;
	case 1: this->DataServerGetCharListRequest(*ArgList); return P_OK;
	case _OnDSPacket: if(this->OnDSPacket(*ArgList, ArgList[1])) return P_OK;
	}
	return P_NO_IMPLEMENT;
}

PRESULT CDSCore::Stop()
{
	TerminateThread(this->hThreads[0], 0);
	TerminateThread(this->hThreads[1], 0);
	return P_OK;
}

void CDSCore::DStart()
{
	this->hThreads[0] = CreateThread(nullptr, 0, CDSCore::ThConnect, (void*)0, 0, nullptr);
	this->hThreads[1] = CreateThread(nullptr, 0, CDSCore::ThConnect, (void*)1, 0, nullptr);
}

void CDSCore::DataServerGetCharListRequest(int iID)
{
	CVar value;
	Obj_GET(iID, "AccountID", &value);

	Packet<SDHP_GETCHARLIST> pack(this, 0xC1);
	pack->OP = 0x01;
	strcpy_s(pack->Id, value);
	pack->Number = iID;

	printf_s("[DS] Character list request for '%s'\n", pack->Id);
	this->Write(pack, pack.size());
}

void CDSCore::Write(void * Stream, int len)
{
	static int last = 1;
	if (last && !this->ConnectionDS1->isConnected())
		last = 0;
	else if (!last && !this->ConnectionDS2->isConnected())
		last = 1;

	if (last)
	{
		if (!this->ConnectionDS1->isConnected())
		{
			Error(1, 0, "All DS discconecteds");
			return;
		}
		this->ConnectionDS1->Write(Stream, len);
		last = 0;
	}
	else
	{
		if (!this->ConnectionDS2->isConnected())
		{
			Error(1, 0, "All DS discconecteds");
			return;
		}
		last = 1;
		this->ConnectionDS2->Write(Stream, len);
	}
}

bool CDSCore::OnDSPacket(char * Stream, int len)
{
	PMMSG_BASE * mixed = (PMMSG_BASE *)Stream;

	switch (mixed->Operation())
	{
	case 0:
	{
		PMSG_JOINRESULT * p = (PMSG_JOINRESULT*)Stream;
		printf("Connection to DS Result: %d\n", p->Result);
	}
	break;
	default:
		return false;
	}
	return true;
}

DWORD CDSCore::ThConnect(VOID * lpVoid)
{
	std::string Ip;
	WORD port;
	WORD ds;
	TcpClient * Client;
	if (lpVoid==nullptr)
	{
		Client = CDSCore::Instance->ConnectionDS1;
		Ip = CDSCore::Instance->Property("IP1");
		port = CDSCore::Instance->Property("Port1");
		ds = 1;
	}
	else
	{
		Client = CDSCore::Instance->ConnectionDS2;
		Ip = CDSCore::Instance->Property("IP2");
		port = CDSCore::Instance->Property("Port2");
		ds = 2;
	}
	while (true)
	{
		printf_s("[DS%d]Trying to connect to '%s:%d'\n", ds, Ip.c_str(), port);
		while (!Client->Connect(Ip.c_str(), port))
		{
			Sleep(5000);
		}

		printf_s("[DS%d]connected to '%s:%d'\n", ds, Ip.c_str(), port);

		Packet<PMSG_JOIN> join(CDSCore::Instance,0xC1);
		join->OP = 0;
		join->Type = 1;
		join->Port = (short)CDSCore::Instance->Loader->property("MuCore", "GameServerPort");
		strcpy_s(join->Name, CDSCore::Instance->Loader->property("MuCore", "GameServerName"));
		join->Code = (short)CDSCore::Instance->Loader->property("MuCore", "GameServerCode");
		Client->Write(join, join.size());

		while (Client->isConnected())
		{
			Sleep(1000);
		}
	}
	return 0;
}
