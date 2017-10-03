// JSCore.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "JSCore.h"
#include "../PluginBase/Loader.h"

char * szEvents[] =
{
	"OnJSPacket", // Packet, Len
	nullptr
};

char * szProc[] =
{
	"JStart",
	"LoginRequest",
	nullptr
};


CJSCore::CJSCore()
{
	strcpy_s(this->m_szName, "JSCore");
	this->szProcListNames = ppChar2vpChar(szProc);
	this->szEventListNames = ppChar2vpChar(szEvents);

	this->m_dwVersion = PLUGIN_MAKEVERSION(0, 99, 0, 0);
	this->Connection = new TcpClient(this);
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
	return P_OK;
}

void CJSCore::JStart(void)
{
	char szIP[100];
	WORD Port = this->Property("Port");
	strcpy_s(szIP, this->Property("IP"));
	if (this->Connection->Connect(szIP, Port))
	{
		puts("Join Server Alive");
	}
	else puts("Join Server Dead");

	Packet<PMSG_JOIN> pack(this,0xC1);
	pack->OP = 0;
	pack->Type = 1;
	pack->Port = (short)this->Loader->property("MuCore", "GameServerPort");
	strcpy_s(pack->Name, this->Loader->property("MuCore", "GameServerName"));
	pack->Code = (short)this->Loader->property("MuCore", "GameServerCode");
	this->Connection->Write(pack, pack.size());
}

void CJSCore::LoginRequest(const char * ID, const char * PW, unsigned short Number, const char * UserIP)
{
	Packet<LoginReq> pack(this,0xC1);
	pack->OP = 1;
	memcpy(pack->AccountId, ID, MAX_ACCPW);
	memcpy(pack->Password, PW, MAX_ACCPW);
	pack->Number = Number;
	memcpy(pack->UserIP, UserIP, 17);
}
