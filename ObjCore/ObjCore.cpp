// ObjCore.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "ObjCore.h"

enum Proc
{
	pSet,
	pGet
};

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
	case pSet: return this->Set(ArgList[0], ArgList[1], ArgList[2]);
	case pGet: return this->Get(ArgList[0], ArgList[1], ArgList[2]);
	}
	return P_OK;
}

PRESULT CObjCore::Stop()
{
	return P_OK;
}

PRESULT CObjCore::Set(DWORD ObjIndex, const char * szProperty, CVar Value)
{
	if (ObjIndex >= MAX_OBJECTS)
		return P_ERROR;

	this->cvObjects[ObjIndex][szProperty] = Value;

	return P_OK;
	/*UINT i;

	// Se busca la propiedad en la lista actual
	for (i = 0; i < MAX_PROPERTIES; i++)
	{
		// En caso tal de que se llegue al final de la lista y
		// no se encontraran coincidencias
		// se agrega un elemento nuevo
		if (!this->szProperties[i][0])
		{
			strcpy_s(this->szProperties[i], szProperty);
			break;
		}
		// En caso de que exista coincidencias
		// se detiene la busqueda
		if (*(WORD*)this->szProperties[i] == *(WORD*)szProperty && 
			!strcmp(this->szProperties[i], szProperty))
		{
			break;
		}
	}

	if (i == MAX_PROPERTIES) return P_ERROR;// todas las propiedades usadas

	this->cvObjects[ObjIndex][i] = Value;

	return P_OK;*/
}

PRESULT CObjCore::Get(DWORD ObjIndex, const char * szProperty, void * Value)
{
	if (ObjIndex >= MAX_OBJECTS)
		return P_ERROR;

	*(CVar*)Value = this->cvObjects[ObjIndex][szProperty];

	return P_OK;
	//UINT i;
	//for (i = 0; i < MAX_PROPERTIES; i++)
	//{
	//	// si se llego al final de la lista y
	//	// no se consiguio ninguna propiedad
	//	// retornar con mensaje de error
	//	if (!this->szProperties[i][0])
	//	{
	//		return P_ERROR; // No existe la propiedad
	//	}
	//	// al existir coincidencia se hace una
	//	// copia del valor en la variable de
	//	// destino
	//	if (*(WORD*)this->szProperties[i] == *(WORD*)szProperty &&
	//		!strcmp(this->szProperties[i], szProperty))
	//	{
	//		this->cvObjects[ObjIndex][i].copy(Value);
	//		return P_OK;
	//	}
	//}

	//return P_ERROR;// No existe la propiedad
}
