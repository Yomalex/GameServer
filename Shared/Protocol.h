#pragma once

#define CHAR_SET_SIZE		18

#pragma pack(push,1)

struct PBMSG_BASE
{
	BYTE Head;
	BYTE Size;
	BYTE Operation;
};

struct PWMSG_BASE
{
	BYTE Head;
	BYTE bSize[2];
	BYTE Operation;

	WORD Size() { return MAKEWORD(bSize[0], bSize[1]); }
	void Size(WORD wSize) { this->bSize[0] = HIBYTE(wSize); this->bSize[1] = LOBYTE(wSize); }
};

struct PBMSG_BASEB
{
	BYTE Head;
	BYTE Size;
	BYTE Operation;
	BYTE Body[1];
};

struct PWMSG_BASEB
{
	BYTE Head;
	BYTE bSize[2];
	BYTE Operation;
	BYTE Body[1];

	WORD Size() { return MAKEWORD(bSize[0], bSize[1]); }
	void Size(WORD wSize) { this->bSize[0] = HIBYTE(wSize); this->bSize[1] = LOBYTE(wSize); }
};

struct PMMSG_BASE
{
	union
	{
		PBMSG_BASEB bHead;
		PWMSG_BASEB wHead;
	};
	BYTE Operation() const { return (bHead.Head == 0xC1 || bHead.Head == 0xC3) ? bHead.Operation : wHead.Operation; }
	BYTE * Body() { return (bHead.Head == 0xC1 || bHead.Head == 0xC3) ? bHead.Body : wHead.Body; }
};

struct PMSG_JOINRESULT : PBMSG_BASE
{
	BYTE SH;
	BYTE Result;
	DWORD ItemCount;
};
#pragma pack(pop)