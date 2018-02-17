#pragma once
#include "..\PluginBase\Plugin.h"
#include "../shared/ConShared.h"

class CConsole :
	public CPlugin
{
public:
	CConsole();
	~CConsole();

	// Plugin Base
	PRESULT Start();
	PRESULT invoke(int proc, CVar * ArgList, int ArgCount);
	PRESULT Stop();

	// Console
	void OnError(const char * szPlugin, const char * szFile, unsigned int Line, DWORD ErrorNo, const char * szErrString);
	void Message(DWORD dwAttr, const char * szPlugin, const char * szMessage);

	friend DWORD WINAPI th_ConsoleInput(CConsole * con);
private:
	HANDLE hThread;
	CRITICAL_SECTION csConsole;
	HANDLE STD_OUT, STD_IN;
};

