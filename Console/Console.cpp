// Console.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "../PluginBase/Loader.h"
#include "Console.h"
#include <iostream>
#include <time.h>

char * szCallBacks[] = {
	"OnError",
	"OnMessage",
	nullptr
};

char * szProcList[] = {
	"Message",
	nullptr
};

DWORD WINAPI th_ConsoleInput(CConsole * con)
{
	char szLine[1024];
	while (true)
	{
		std::cin.getline(szLine, sizeof(szLine));
		con->Loader->Command(szLine);
	}
}

CConsole::CConsole()
{
	this->szCallBackListNames = ppChar2vpChar(szCallBacks);
	this->szProcListNames = ppChar2vpChar(szProcList);
	strcpy_s(this->m_szName, "CONSOLE");
	this->m_dwVersion = PLUGIN_MAKEVERSION(1, 0, 0, 0);

	InitializeCriticalSection(&csConsole);
}


CConsole::~CConsole()
{
	DeleteCriticalSection(&csConsole);
}

PRESULT CConsole::Start()
{
	if (AllocConsole())
	{
		freopen("CONOUT$", "wt", stdout);
		freopen("CONIN$", "rt", stdin);
	}

	SetConsoleTitleA("Console");

	hThread = CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)th_ConsoleInput, this, 0, nullptr);

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
	TerminateThread(hThread, 0);
	if (!FreeConsole()) return P_ERROR;
	return P_OK;
}

void HelperAddTime()
{
	time_t t;
	tm tim;
	time(&t);
	localtime_s(&tim, &t);

	std::cout.width(2);
	std::cout << tim.tm_mon + 1 << "/" << tim.tm_mday << "/" << tim.tm_year + 1900 << " - ";
	std::cout << tim.tm_hour << ":" << tim.tm_min << ":" << tim.tm_sec << " ";
}

void CConsole::OnError(const char * szPlugin, const char * szFile, unsigned int Line, DWORD ErrorNo, const char * szErrString)
{

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
	EnterCriticalSection(&csConsole);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);// White
	HelperAddTime();
	std::cout << "[";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
	std::cout << "Error ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	std::cout << szPlugin;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);// White
	std::cout << "] Number: " << ErrorNo;
	//printf("%s_%s (%d) Error(%d)", szPlugin, file.c_str(), Line, ErrorNo);
	if (szErrString) std::cout << " Message: " << szErrString << std::endl;
	else std::cout << std::endl;

	HelperAddTime();
	std::cout << "[Debug] File:" << file << " Line:" << Line << std::endl;
	LeaveCriticalSection(&csConsole);
}

void CConsole::Message(DWORD dwAttr, const char * szPlugin, const char * szMessage)
{
	EnterCriticalSection(&csConsole);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), dwAttr);
	HelperAddTime();
	std::cout << "[Message "<< szPlugin << "] " << szMessage << std::endl;
	LeaveCriticalSection(&csConsole);
}
