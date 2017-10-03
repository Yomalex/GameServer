#pragma once
#pragma pack(push,1)

struct PBMSG_BASE
{
	BYTE Head;
	BYTE Size;
	BYTE Operation;
	BYTE Body[1];
};

struct PWMSG_BASE
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
		PBMSG_BASE bHead;
		PWMSG_BASE wHead;
	};
	BYTE Operation() const { return (bHead.Head == 0xC1 || bHead.Head == 0xC3) ? bHead.Operation : wHead.Operation; }
	BYTE * Body() { return (bHead.Head == 0xC1 || bHead.Head == 0xC3) ? bHead.Body : wHead.Body; }
};

#pragma pack(pop)