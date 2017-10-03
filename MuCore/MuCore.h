#pragma once
#include "../Common/Packet.h"

class CMuCore :
	public CPlugin
{
public:
	CMuCore();
	~CMuCore();

	// PluginBase
	PRESULT Start();
	PRESULT invoke(int proc, CVar * ArgList, int ArgCount);
	PRESULT Stop();

	// Plugin
	void SCPJoinResult(int iID, int result);

	// CallBacks
	PRESULT OnConnect(DWORD dwIndex, char * szIP);
	void OnData(DWORD dwIndex, char * Buffer, int Size);
	PRESULT OnDisconnect(DWORD dwIndex, DWORD dwLifeTime);
};