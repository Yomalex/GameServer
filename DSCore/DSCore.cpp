// DSCore.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "DSCore.h"


char * szEvents[] =
{
	"OnDSPacket", // Packet, Len
	nullptr
};
char * szProc[] =
{
	"DStart",
	nullptr
};

CDSCore * CDSCore::Instance;

CDSCore::CDSCore()
{
	strcpy_s(this->m_szName, "DSCore");
	this->szProcListNames = ppChar2vpChar(szProc);
	this->szEventListNames = ppChar2vpChar(szEvents);

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
