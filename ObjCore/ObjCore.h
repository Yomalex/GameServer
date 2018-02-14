#pragma once
#include "..\PluginBase\Plugin.h"
#include <vector>
#include <map>
#include <string>

//#define MAX_PROPERTIES 100
#define MAX_OBJECTS 0xffff

class CObjCore :
	public CPlugin
{
	//char szProperties[MAX_PROPERTIES][100];
	//CVar cvObjects[65535][MAX_PROPERTIES];

	std::map<CVar, std::string> cvObjects[MAX_OBJECTS];
public:
	CObjCore();
	~CObjCore();

	PRESULT Start();
	PRESULT invoke(int proc, CVar * ArgList, int ArgCount);
	PRESULT Stop();

	PRESULT Set(DWORD ObjIndex, const char * szProperty, CVar Value);
	PRESULT Get(DWORD ObjIndex, const char * szProperty, void * Value);
};

