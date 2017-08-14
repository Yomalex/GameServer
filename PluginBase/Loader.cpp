#include "stdafx.h"
#include "Loader.h"
#include <Windows.h>
#include <iostream>
#include <regex>

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

	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << "Named: " << pInfo.Name << std::endl;

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
	return pInfo.pLink->Start();
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

std::cmatch cm;
std::regex e("(.+)(?:->)(.+)(?:\\()(.+)(?:\\))(?:\\s*)(?:;*)(?:\\s*)(.*)(?:\\n?)$");
std::regex comment("(?:\\s?)(?:;)(?:\\s?)(.*)$(?:\\n?)");
PRESULT CLoader::Command(const char * szLine)
{
	// Extraccion de Comentarios
	if (std::regex_match(szLine, cm, comment))
	{
		// Mostrar el comentario en Verde con fondo negro
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		std::cout << cm[1].str() << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return P_OK;
	}
	// Extraccion de argumentos para hacer llamadas en los plugin
	if (std::regex_match(szLine, cm, e))//"Plugin->Function(ArgumentList)"
	{
		if (cm[1].compare("Container") == 0)
		{
			if (cm[2].compare("Load") == 0)
			{
				std::string arguments(cm[3].str());
				std::string filename;
				filename.assign(arguments.begin() + 1, arguments.end() - 1);

				std::cout << "Loading plugin '" << filename.c_str() << "'";
				if (strlen(cm[4].str().c_str()) > 1)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
					std::cout << " " << cm[4].str();
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				}

				std::cout << std::endl;

				if (this->Load(filename.c_str()) != P_OK)
				{
					std::cout << "Error Starting plugin" << std::endl;
				}
			}
		}
		else
		{
			CVar Args[10];
			int argCount = 10;
			size_t lastFind = 0, prevFind = 0;

			//parse args
			std::string arguments(cm[3].str());
			std::string argList[10];
			std::cmatch cmArg;
			std::regex eArgs("(?:\\s*)'(.+?)'");
			std::regex eArgs2("(?:\\s*)''");

			for (register int i = 0; i < 10; i++)
			{
				if ((lastFind = arguments.find(',', lastFind + 1)) != std::string::npos)
				{
					argList[i].assign(arguments.begin() + prevFind, arguments.begin() + lastFind);
					prevFind = lastFind + 1;
				}
				else
				{
					argList[i].assign(arguments.begin() + prevFind, arguments.end());
					argCount = i + 1;
					break;
				}
			}

			std::vector<void*> GlobalPointer;

			for (register int i = 0; i < argCount; i++)
			{
				if (std::regex_match(argList[i].c_str(), cmArg, eArgs))
				{
					char * pLine = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1024);
					strcpy_s(pLine, 1024, cmArg[1].str().c_str());
					Args[i] = pLine;
					GlobalPointer.push_back(pLine);
				}
				else if (std::regex_match(argList[i].c_str(), cmArg, eArgs2))
				{
					Args[i] = "";
				}
				else
				{
					Args[i] = (float)atof(argList[i].c_str());
				}
			}

			if (this->invoke(cm[1].str().c_str(), cm[2].str().c_str(), Args, argCount) != P_OK)
			{
				std::cout << "Error calling:" << cm[1].str().c_str() << "::" << cm[2].str().c_str() << std::endl;
			}
			for (register unsigned int i = 0; i < GlobalPointer.size(); i++)
			{
				HeapFree(GetProcessHeap(), 0, GlobalPointer[i]);
			}
		}
	}
	else
	{
		return P_ERROR;
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
