#include "stdafx.h"
#include "Loader.h"
#include <Windows.h>


CLoader::CLoader()
{
}


CLoader::~CLoader()
{
}

PRESULT CLoader::Load(const char * szFileName)
{
	PLUGIN_INFO pInfo;
	std::vector<char*> EventList;

	pInfo.hModule = LoadLibraryA(szFileName);

	if (!pInfo.hModule) return P_ERROR;
	if (this->GetPluginInfo(pInfo.hModule) != nullptr) return P_OK;

	pInfo.pLink = (CPlugin *)(GetProcAddress(pInfo.hModule, "Plugin")());

	if (pInfo.pLink == nullptr)
	{
		FreeLibrary(pInfo.hModule);
		return P_ERROR;
	}
	strcpy_s(pInfo.Name, pInfo.pLink->GetPluginName());
	pInfo.pLink->Parent(this);

	// Creando lista de funciones
	pInfo.pLink->ProcList(pInfo.ProcList);

	// Obteniendo lista de Eventos a escuchar
	pInfo.pLink->CallBackList(pInfo.CBacList);

	// Registrando en la lista
	this->Plugins.push_back(pInfo);

	for (register UINT i = 0; i<pInfo.CBacList.size(); i++)
	{
		this->Link(pInfo.CBacList[i], pInfo.pLink, i);
	}

	// Obteniendo lista de Eventos a llamar
	pInfo.pLink->EventList(EventList);
	
	EVENT_INFO ei;
	strcpy_s(ei.szPlugin, pInfo.Name);
	
	// Globalizando Eventos
	for (register UINT i = 0; i<EventList.size(); i++)
	{
		ei.szEvent = EventList[i];
		ei.iEvent = i;
		this->Events.push_back(ei);
		this->FindAndLink(ei.szEvent);	// Buscar Eventos de escucha
										// en los plugins ya cargados
	}

	// Inicializando Plugin
	pInfo.pLink->Start();

	return P_OK;
}

PRESULT CLoader::Link(const char * szEvent, CPlugin * plg, int iCBIndex)
{
	for (register UINT i = 0; i < this->Events.size(); i++)
	{
		if (!strcmp(szEvent, this->Events[i].szEvent))
		{
			this->GetPluginInfo(this->Events[i].szPlugin)->pLink->RegCallBack(this->Events[i].iEvent, plg, iCBIndex);
			return P_OK;
		}
	}
	return P_ERROR;
}

PRESULT CLoader::Unlink(CPlugin * plg)
{
	for (register UINT i = 0; i < this->Plugins.size(); i++)
	{
		this->Plugins[i].pLink->UnregCallBacks(plg);
	}
	return P_OK;
}

PRESULT CLoader::FindAndLink(const char * szCallBack)
{
	for (register UINT i = 0; i < this->Plugins.size(); i++)
	{
		for (register UINT j = 0; j < this->Plugins[i].CBacList.size(); j++)
		{
			if (strcmp(this->Plugins[i].CBacList[j], szCallBack) == 0)
				this->Link(szCallBack, this->Plugins[i].pLink, j);
		}
	}
	return P_OK;
}

PRESULT CLoader::invoke(const char * szPlugin, const char * szFunction, CVar * Args, int ArgCount)
{
	PLUGIN_INFO* pInfo = this->GetPluginInfo(szPlugin);

	if (!pInfo) return P_ERROR;

	static short FID1, FID2;
	for (register int j = 0; j < pInfo->ProcList.size(); j++)
	{
		FID1 = *(short*)pInfo->ProcList[j];
		FID2 = *(short*)szFunction;

		if (FID1 == FID2 && !strcmp(pInfo->ProcList[j], szFunction))
		{
			return pInfo->pLink->invoke(j, Args, ArgCount);
		}
	}

	return P_INVALID_ARG;
}

PRESULT CLoader::Free(const char * szPluginName)
{
	__int64 UID = *(__int64*)szPluginName;
	for (register int i = 0; i < this->Plugins.size(); i++)
	{
		if (this->Plugins[i].UID != UID) continue;


		for (register UINT j = 0; j<this->Plugins[i].CBacList.size(); j++)
		{
			this->Unlink(this->Plugins[i].pLink);
		}

		this->Plugins[i].pLink->Stop();
		FreeLibrary(this->Plugins[i].hModule);

		break;
	}

	return P_OK;
}

PRESULT CLoader::Free()
{
	for (register UINT i = 0; i < this->Plugins.size(); i++)
	{
		for (register UINT j = 0; j < this->Plugins[i].CBacList.size(); j++)
		{
			this->Unlink(this->Plugins[i].pLink);
		}

		this->Plugins[i].pLink->Stop();
		FreeLibrary(this->Plugins[i].hModule);
	}

	return P_OK;
}

PLUGIN_INFO* CLoader::GetPluginInfo(const char * szPlugin)
{
	static char szPluginName[8];
	static __int64 UID;

	strcpy_s(szPluginName, szPlugin);

	UID = *(__int64*)szPluginName;

	for (register int i = 0; i < this->Plugins.size(); i++)
	{
		if (this->Plugins[i].UID != UID) continue;

		return &this->Plugins[i];
	}

	return nullptr;
}

PLUGIN_INFO * CLoader::GetPluginInfo(HMODULE hModule)
{
	for (register UINT i = 0; i < this->Plugins.size(); i++)
	{
		if (this->Plugins[i].hModule == hModule) return &this->Plugins[i];
	}
	return nullptr;
}
