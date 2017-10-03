// ObjCore.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "ObjCore.h"

char * szProc[] =
{
	"Set",
	"Get",
	nullptr
};


CObjCore::CObjCore()
{
	strcpy_s(this->m_szName, "ObjCore");
	this->m_dwVersion = PLUGIN_MAKEVERSION(1, 0, 0, 0);

	this->szProcListNames = ppChar2vpChar(szProc);
}


CObjCore::~CObjCore()
{
}

PRESULT CObjCore::Start()
{
	return P_OK;
}

PRESULT CObjCore::invoke(int proc, CVar * ArgList, int ArgCount)
{
	if (ArgCount != 3) return P_INVALID_ARG;
	switch (proc)
	{
	case 0: return this->Set(ArgList[0], ArgList[1], ArgList[2]);
	case 1: return this->Get(ArgList[0], ArgList[1], ArgList[2]);
	}
	return P_OK;
}

PRESULT CObjCore::Stop()
{
	return P_OK;
}

PRESULT CObjCore::Set(DWORD ObjIndex, const char * szProperty, CVar Value)
{
	UINT i;
	for (i = 0; i < MAX_PROPERTIES; i++)
	{
		if (!this->szProperties[i][0])
		{
			strcpy_s(this->szProperties[i], szProperty);
			break;
		}
		if (*(WORD*)this->szProperties[i] == *(WORD*)szProperty && 
			!strcmp(this->szProperties[i], szProperty))
		{
			break;
		}
	}

	if (i == MAX_PROPERTIES) return P_ERROR;// todas las propiedades usadas

	this->cvObjects[ObjIndex][i] = Value;

	return P_OK;
}

PRESULT CObjCore::Get(DWORD ObjIndex, const char * szProperty, void * Value)
{
	UINT i;
	for (i = 0; i < MAX_PROPERTIES; i++)
	{
		if (!this->szProperties[i][0])
		{
			return P_ERROR; // No existe la propiedad
		}
		if (*(WORD*)this->szProperties[i] == *(WORD*)szProperty &&
			!strcmp(this->szProperties[i], szProperty))
		{
			this->cvObjects[ObjIndex][i].copy(Value);
			return P_OK;
		}
	}

	return P_ERROR;// No existe la propiedad
}
