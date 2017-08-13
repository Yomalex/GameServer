#ifdef _DEBUG
#pragma comment(lib,"../Debug/PluginBase.lib")
#else
#pragma comment(lib,"../Release/PluginBase.lib")
#endif

#pragma once


#include "Var.h"
#include "Definitions.h"
#include <vector>

#define DEF_NAME Plugin

#define PLUGIN_ENTRYPOINT(x) extern "C" __declspec(dllexport) x * DEF_NAME(){ return new x(); }
#define CALLBACK_START_INDEX 1000
#define CALLBACK_INDEX(x) (x+CALLBACK_START_INDEX)
// Clase base del plugin

class CPlugin;

struct _tagCBInfo
{
	CPlugin * pPlugin;
	int Proc;
};
class CLoader;

class CPlugin
{
public:

	CPlugin();
	virtual ~CPlugin();

	virtual PRESULT Start() = 0;
	virtual PRESULT invoke(int proc, CVar * ArgList, int ArgCount) = 0;
	virtual PRESULT Stop() = 0;
	
	PRESULT EventList(std::vector<char *> &names);
	PRESULT ProcList(std::vector<char *> &names);
	PRESULT CallBackList(std::vector<char *> &names);
	PRESULT RegCallBack(unsigned int iEvent, CPlugin * plg, int iCBIndex);
	PRESULT DispCallBack(unsigned int iEvent, CVar * Args, int ArgsCount);
	PRESULT UnregCallBacks(CPlugin * plg);
	const char * GetPluginName() { return this->m_szName; }
	void Parent(CLoader* pLoader) { this->Loader = pLoader; };
protected:
	CLoader * Loader;
	char m_szName[8];

	std::vector<char *> szEventListNames;
	std::vector<char *> szCallBackListNames;
	std::vector<char *> szProcListNames;

	std::vector<std::vector<_tagCBInfo>> CallBacks;
};

// Util

std::vector<char *> ppChar2vpChar(char ** szStringArray);