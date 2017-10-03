#include "Stdafx.h"
#include "Packet.h"


NWORD::NWORD()
{
	Word = 0;
}

NWORD::NWORD(BYTE bLow, BYTE bHigh)
{
	this->High = bHigh;
	this->Low = bLow;
}

WORD NWORD::Number()
{
	return MAKEWORD(this->Low, this->High);
}

void NWORD::Number(WORD wNumber)
{
	this->High = HIBYTE(wNumber);
	this->Low = LOBYTE(wNumber);
}

NWORD NWORD::operator=(WORD wNumber)
{
	this->Word = wNumber;
	return *this;
}

NWORD::operator WORD()
{
	return this->Word;
}

NWORD::operator BYTE()
{
	return this->Low;
}