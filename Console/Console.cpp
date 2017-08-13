// Console.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "Console.h"
#include <iostream>

char * szCallBacks[] = {
	"OnError",
	"OnMessage",
	nullptr
};

char * szProcList[] = {
	"Message",
	nullptr
};

CConsole::CConsole()
{
	this->szCallBackListNames = ppChar2vpChar(szCallBacks);
	this->szProcListNames = ppChar2vpChar(szProcList);
	strcpy_s(this->m_szName, "CONS");
}


CConsole::~CConsole()
{
}

PRESULT CConsole::Start()
{
	if (!AllocConsole()) return P_ERROR;

	freopen("CONOUT$", "wt", stdout);
	freopen("CONIN$", "rt", stdin);
	SetConsoleTitleA("Console");

	return P_OK;
}

PRESULT CConsole::invoke(int proc, CVar * ArgList, int ArgCount)
{
	switch (proc)
	{
	case 0:
		if (ArgCount != 3) return P_INVALID_ARG;
		this->Message((long)ArgList[0], ArgList[1], ArgList[2]);
		break;
	case CALLBACK_INDEX(0):
		if (ArgCount != 5) return P_INVALID_ARG;
		this->OnError(ArgList[0], ArgList[1], ArgList[2], (long)ArgList[3], ArgList[4]);
		break;
	}
	return P_OK;
}

PRESULT CConsole::Stop()
{
	if (!FreeConsole()) return P_ERROR;
	return P_OK;
}

void CConsole::OnError(const char * szPlugin, const char * szFile, unsigned int Line, DWORD ErrorNo, const char * szErrString)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

	std::string file(szFile);
	size_t backslash = file.rfind('/');
	if (backslash == std::string::npos)
	{
		backslash = file.rfind('\\');
	}
	if (backslash != std::string::npos)
	{
		file.erase(file.begin(), file.begin() + backslash + 1);
	}
	char szOutput[1024];
	sprintf(szOutput, "%s_%s (%d) Error(%d)", szPlugin, file.c_str(), Line, ErrorNo);
	std::cout << szOutput;
	if (szErrString) std::cout << szErrString << std::endl;
	else std::cout << std::endl;
}

void CConsole::Message(DWORD dwAttr, const char * szPlugin, const char * szMessage)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << szPlugin << ": " << szMessage << std::endl;
}
