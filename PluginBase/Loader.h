#ifdef _DEBUG
#pragma comment(lib,"../Debug/PluginBase.lib")
#else
#pragma comment(lib,"../Release/PluginBase.lib")
#endif

#pragma once
#include "Var.h"
#include "Definitions.h"
#include "Plugin.h"
#include <vector>
#include <Windows.h>

struct PLUGIN_INFO
{
	HMODULE hModule;
	union
	{
		char Name[8];
		__int64 UID;
	};
	CPlugin * pLink;
	std::vector<char *> ProcList;
	std::vector<char *> CBacList;
};
struct EVENT_INFO
{
	char szPlugin[8];
	unsigned int iEvent;
	char * szEvent;
};

class CLoader
{
	std::vector<PLUGIN_INFO> Plugins;
	std::vector<EVENT_INFO> Events;
public:
	CLoader();
	~CLoader();

	PRESULT Load(const char * szFileName);
	PRESULT invoke(const char * szPlugin, const char * szFunction, CVar * Args, int ArgCount);
	PRESULT Free(const char * szPluginName);
	PRESULT Free();

private:
	PRESULT Link(const char * szEvent, CPlugin * plg, int iCBIndex);
	PRESULT Unlink(CPlugin * plg);
	PRESULT FindAndLink(const char * szCallBack);
	PLUGIN_INFO* GetPluginInfo(const char * szPlugin);
	PLUGIN_INFO* GetPluginInfo(HMODULE hModule);
};
