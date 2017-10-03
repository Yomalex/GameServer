#pragma once
#include "..\PluginBase\Plugin.h"
#include <vector>

#define MAX_PROPERTIES 100

class CObjCore :
	public CPlugin
{
	char szProperties[MAX_PROPERTIES][100];
	CVar cvObjects[65535][MAX_PROPERTIES];
public:
	CObjCore();
	~CObjCore();

	PRESULT Start();
	PRESULT invoke(int proc, CVar * ArgList, int ArgCount);
	PRESULT Stop();

	PRESULT Set(DWORD ObjIndex, const char * szProperty, CVar Value);
	PRESULT Get(DWORD ObjIndex, const char * szProperty, void * Value);
};

