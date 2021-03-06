#include "stdafx.h"
#include "SModulus.h"
#include "SimpleModulus.h"

char * szProc[] = {
	"LoadEncKey",
	"LoadDecKey",
	"Encrypt",
	"Decrypt",
	nullptr,
};

char * szEvent[] = {
	"OnError",
	nullptr,
};

CSModulus::CSModulus()
{
	this->szProcListNames = ppChar2vpChar(szProc);
	this->szEventListNames = ppChar2vpChar(szEvent);
	strcpy_s(this->m_szName, "Modulus");
	this->m_dwVersion = PLUGIN_MAKEVERSION(1, 0, 0, 0);
}

CSModulus::~CSModulus()
{
}

PRESULT CSModulus::Start()
{
	return P_OK;
}

PRESULT CSModulus::invoke(int proc, CVar * ArgList, int ArgCount)
{
	switch (proc)
	{
	case 0:
		CALLBACK_CHKARG(ArgCount, 1);
		this->LoadEncKey(*ArgList);
		return P_OK;
	case 1:
		CALLBACK_CHKARG(ArgCount, 1);
		this->LoadDecKey(*ArgList);
		return P_OK;
	case 2:
		CALLBACK_CHKARG(ArgCount, 3);
		return this->Encrypt(ArgList[0], ArgList[1], ArgList[2]);
	case 3:
		CALLBACK_CHKARG(ArgCount, 3);
		return this->Decrypt(ArgList[0], ArgList[1], ArgList[2]);
	}
	return P_ERROR;
}

PRESULT CSModulus::Stop()
{
	return P_OK;
}

void CSModulus::LoadEncKey(LPSTR lpszFileName)
{
	if (!g_SimpleModulusSC.LoadEncryptionKey(lpszFileName))
	{
		Error(0, PLUGIN_ERROR(P_INVALID_ARG), "Un argumento es invalido");
	}
}

void CSModulus::LoadDecKey(LPSTR lpszFileName)
{
	if (!g_SimpleModulusCS.LoadDecryptionKey(lpszFileName))
	{
		Error(0, PLUGIN_ERROR(P_INVALID_ARG), "Un argumento es invalido");
	}
}

PRESULT CSModulus::Encrypt(void * lpDest, void * lpSource, int * iSize)
{
	if (iSize == nullptr || lpDest == nullptr || lpSource == nullptr)
	{
		Error(0, PLUGIN_ERROR(P_INVALID_ARG), "Un argumento es invalido");
		return P_INVALID_ARG;
	}
		

	*iSize = g_SimpleModulusSC.Encrypt(lpDest, lpSource, *iSize);

	return P_OK;
}

PRESULT CSModulus::Decrypt(void * lpDest, void * lpSource, int * iSize)
{
	if (iSize == nullptr || lpDest == nullptr || lpSource == nullptr)
	{
		Error(0, PLUGIN_ERROR(P_INVALID_ARG), "Un argumento es invalido");
		return P_INVALID_ARG;
	}

	*iSize = g_SimpleModulusCS.Decrypt(lpDest, lpSource, *iSize);

	return P_OK;
}
