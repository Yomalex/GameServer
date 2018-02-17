// JSCore.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "JSCore.h"
#include "../PluginBase/Loader.h"
#include "../Shared/ConShared.h"

char * szEvents[] =
{
	"OnJSPacket", // Packet, Len
	"OnError",
	nullptr
};

char * szProc[] =
{
	"JStart",
	"LoginRequest",
	nullptr
};

CJSCore * CJSCore::instance;


CJSCore::CJSCore()
{
	strcpy_s(this->m_szName, "JSCore");
	this->szProcListNames = ppChar2vpChar(szProc);
	this->szEventListNames = ppChar2vpChar(szEvents);

	this->m_dwVersion = PLUGIN_MAKEVERSION(0, 99, 0, 0);
	this->Connection = new TcpClient(this);
	this->instance = this;
}


CJSCore::~CJSCore()
{
	delete this->Connection;
}

PRESULT CJSCore::Start()
{
	if (this->FindDependency("MuCore", PLUGIN_MAKEVERSION(1, 0, 0, 0))) return P_MISSING_DEPENDENCY;

	return P_OK;
}

PRESULT CJSCore::invoke(int proc, CVar * ArgList, int ArgCount)
{
	switch (proc)
	{
	case 0:
		this->JStart();
		return P_OK;
	case 1:
		CALLBACK_CHKARG(ArgCount, 4);
		this->LoginRequest(ArgList[0], ArgList[1], ArgList[2], ArgList[3]);
		return P_OK;
	}
	return P_OK;
}

PRESULT CJSCore::Stop()
{
	TerminateThread(hThread, 0);
	return P_OK;
}

void CJSCore::JStart(void)
{
	hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)CJSCore::ThConnect, nullptr, 0, nullptr);
}

void CJSCore::LoginRequest(const char * ID, const char * PW, unsigned short Number, const char * UserIP)
{
	Packet<LoginReq> pack(this,0xC1);
	pack->OP = 1;
	memcpy(pack->AccountId, ID, MAX_ACCPW);
	memcpy(pack->Password, PW, MAX_ACCPW);
	pack->Number = Number;
	memcpy(pack->UserIP, UserIP, 17);

	printf_s("[JS] Login request for (%d)\n", pack->Number);

	this->Connection->Write(pack, pack.size());
}

DWORD CJSCore::ThConnect(VOID * lpVoid)
{
	char szIP[100];
	while ( true )
	{
		WORD Port = instance->Property("Port");
		strcpy_s(szIP, instance->Property("IP"));
		while (!instance->Connection->Connect(szIP, Port))
		{
			Sleep(5000);
		}


		Packet<PMSG_JOIN> pack(instance, 0xC1);
		pack->OP = 0;
		pack->Type = 1;
		pack->Port = (short)instance->Loader->property("MuCore", "GameServerPort");
		strcpy_s(pack->Name, instance->Loader->property("MuCore", "GameServerName"));
		pack->Code = (short)instance->Loader->property("MuCore", "GameServerCode");
		instance->Connection->Write(pack, pack.size());

		while (instance->Connection->isConnected())
		{
			Sleep(1000);
		}

		Msg2(instance, ConColors::Con_Red, "Connection with JoinServer Lost");
	}
}
