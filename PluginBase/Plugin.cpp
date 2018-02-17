#include "stdafx.h"
#include "Plugin.h"
#include "Loader.h"

char * szEmptyList[1] = { nullptr };


CPlugin::CPlugin()
{
	this->szCallBackListNames = ppChar2vpChar(szEmptyList);
	this->szEventListNames = ppChar2vpChar(szEmptyList);
	this->szProcListNames = ppChar2vpChar(szEmptyList);
}

CPlugin::~CPlugin()
{
}

PRESULT CPlugin::EventList(std::vector<char*>& names)
{
	names = this->szEventListNames;
	if (!this->CallBacks.size())
	{
		this->CallBacks.resize(this->szEventListNames.size());
	}
	return P_OK;
}

PRESULT CPlugin::ProcList(std::vector<char *> &names)
{
	names = this->szProcListNames;
	return P_OK;
}

PRESULT CPlugin::CallBackList(std::vector<char*>& names)
{
	names = this->szCallBackListNames;
	return P_OK;
}

PRESULT CPlugin::RegCallBack(unsigned int iEvent, CPlugin * plg, int iCBIndex)
{
	if (this->szEventListNames.size() <= iEvent) return P_INVALID_ARG;

	_tagCBInfo CBInfo;
	CBInfo.pPlugin = plg;
	CBInfo.Proc = CALLBACK_INDEX(iCBIndex);
	this->CallBacks[iEvent].push_back(CBInfo);
	
	return P_OK;
}

PRESULT CPlugin::DispCallBack(unsigned int iCallBack, CVar * Args, int ArgsCount)
{
	if (this->szEventListNames.size() <= iCallBack) return P_INVALID_ARG;
	PRESULT pResult;

	for (unsigned int i = 0; i < this->CallBacks[iCallBack].size(); i++)
	{
		_tagCBInfo &CBInfo = this->CallBacks[iCallBack][i];
		pResult = CBInfo.pPlugin->invoke(CBInfo.Proc, Args, ArgsCount);
		if(pResult != P_NO_IMPLEMENT) return pResult;
	}

	return P_NO_IMPLEMENT;
}

PRESULT CPlugin::DispCallBack(unsigned int iEvent, int ArgsCount, ...)
{
	CVar AList[10];
	va_list ap;
	va_start(ap, ArgsCount);
	for (register int i = 0; i < min(ArgsCount, 10); i++) AList[i] = va_arg(ap, void*);
	va_end(ap);

	return this->DispCallBack(iEvent, AList, ArgsCount);
}

PRESULT CPlugin::UnregCallBacks(CPlugin * plg)
{
	for (unsigned int i = 0; i < this->CallBacks.size(); i++)
	{
		for (unsigned int j = 0; j < this->CallBacks[i].size(); j++)
		{
			if (this->CallBacks[i][j].pPlugin != plg) continue;
			std::vector<_tagCBInfo>::iterator it = this->CallBacks[i].begin()+j;
			this->CallBacks[i].erase(it);
		}
	}

	return P_OK;
}

FlexVar & CPlugin::Property(const char * szProperty)
{
	static char szName[20];
	ZeroMemory(szName, 20);
	strcpy_s(szName, szProperty);

	for (register unsigned int i = 0; i < this->propertyList.size(); i++)
	{
		if (*(DWORD*)szName == *(DWORD*)this->propertyList[i].szName
			&& !strcmp(szName, this->propertyList[i].szName))
			return this->propertyList[i].Value;
	}
	
	_Property prop;
	strcpy_s(prop.szName, szProperty);
	this->propertyList.push_back(prop);

	return this->propertyList.back().Value;
}

PRESULT CPlugin::FindDependency(const char * szPlugin, PlgVer Version)
{
	if (this->Loader->PluginVersion(szPlugin) < Version.ulVersion)
	{
		char szString[100];
		sprintf_s(szString, "Se requiere el plugin %s version %s+", szPlugin, Version.szPluginVersion);
		MessageBoxA(NULL, szString, this->m_szName, MB_ICONERROR);
		return P_MISSING_DEPENDENCY;
	}
	return P_OK;
}

std::vector<char*> ppChar2vpChar(char ** szStringArray)
{
	std::vector<char*> ret;
	for (register unsigned int i = 0; szStringArray[i] != nullptr; i++)
	{
		ret.push_back(szStringArray[i]);
	}
	return ret;
}
