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
#define PLUGIN_ERROR(x) 1900 | x
#define CALLBACK_START_INDEX 1000
#define CALLBACK_INDEX(x) (x+CALLBACK_START_INDEX)
#define CALLBACK_CHKARG(x,y) if(x!=y) return P_INVALID_ARG
// Clase base del plugin

class CLoader;
class CPlugin;

struct _tagCBInfo
{
	CPlugin * pPlugin;
	int Proc;
};

struct PlgVer
{
	char szPluginVersion[10];
	union
	{
		unsigned char ucVersion[4];
		unsigned long ulVersion;
	};

	PlgVer()
	{

	}
	PlgVer(unsigned long ulConstruct)
	{
		this->operator=(ulConstruct);
	}
	PlgVer(const char * cszConstruct)
	{
		this->operator=(cszConstruct);
	}
	PlgVer(unsigned char v1, unsigned char v2, unsigned char v3, unsigned char v4)
	{

	}

	PlgVer operator =(const char * cszRight)
	{
		char * pString = (char*)cszRight;
		int id = 0;
		for (char * Current = (char*)cszRight; *Current; Current++)
		{
			if (*Current == '.')
			{
				*Current = 0;
				ucVersion[id] = atoi(pString);
				pString = Current + 1;
				continue;
			}
		}
		return *this;
	}
	PlgVer operator =(unsigned long ulRight)
	{
		this->ulVersion = ulRight;
		sprintf_s(this->szPluginVersion, "%d.%d.%d.%d", ucVersion[3], ucVersion[2], ucVersion[1], ucVersion[0]);
		return *this;
	}

	bool operator == (unsigned long ulRight)
	{
		return this->ulVersion == ulRight;
	}
	bool operator == (PlgVer& ulRight)
	{
		return this->ulVersion == ulRight.ulVersion;
	}

	operator const char*()
	{
		return this->szPluginVersion;
	}
	operator unsigned long()
	{
		return this->ulVersion;
	}
};

#include "FlexVar.h"

struct _Property
{
	char szName[20];
	FlexVar Value;
};

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
	PRESULT DispCallBack(unsigned int iEvent, int ArgsCount, ...);
	PRESULT UnregCallBacks(CPlugin * plg);
	FlexVar& Property(const char * szProperty);
	const char * GetPluginName() { return this->m_szName; }
	unsigned long GetPluginVersion() { return this->m_dwVersion; }
	const char * GetPluginVersionString() { return this->m_dwVersion; }
	void Parent(CLoader* pLoader) { this->Loader = pLoader; };
	CLoader * Parent() const { return this->Loader; }
	int AddEvent(char * szEvent) { szEventListNames.push_back(szEvent); return szEventListNames.size() - 1; }

protected:
	std::vector<char *> szEventListNames;
	std::vector<char *> szCallBackListNames;
	std::vector<char *> szProcListNames;
	CLoader * Loader;
	char m_szName[8];
	PlgVer m_dwVersion;

	std::vector<std::vector<_tagCBInfo>> CallBacks;

	std::vector<_Property> propertyList;

	PRESULT FindDependency(const char * szPlugin, PlgVer Version);
};

// Util

std::vector<char *> ppChar2vpChar(char ** szStringArray);

