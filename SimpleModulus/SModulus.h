#pragma once
#include "..\PluginBase\Plugin.h"
#include "..\Shared\ConShared.h"

class CSModulus :
	public CPlugin
{
public:
	CSModulus();
	~CSModulus();

	PRESULT Start();
	PRESULT invoke(int proc, CVar * ArgList, int ArgCount);
	PRESULT Stop();

	void LoadEncKey(LPSTR lpszFileName);
	void LoadDecKey(LPSTR lpszFileName);

	PRESULT Encrypt(void * lpDest, void * lpSource, int * iSize);
	PRESULT Decrypt(void * lpDest, void * lpSource, int * iSize);
};

