#include "stdafx.h"
#include "Var.h"
#include <Windows.h>
#include <string>


CVar::CVar()
{
	this->m_Type = None;
}


CVar::~CVar()
{
	//if (this->m_Type == String)  HeapFree(GetProcessHeap(), 0, this->m_MixedVar.Pointer);
	this->m_MixedVar.Pointer = nullptr;
	this->m_Type = None;
}

CVar CVar::operator=(void * lPointer)
{
	CVar& a = *this;
	a.m_Type = Pointer;
	a.m_MixedVar.Pointer = lPointer;
	return a;
}

CVar CVar::operator=(const char * lPointer)
{
	this->m_MixedVar.pString = (char*)lPointer;// (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(lPointer) + 10);
	this->m_Type = String;
	//strcpy_s(this->m_MixedVar.pString, strlen(lPointer) + 1, lPointer);

	return *this;
}

CVar CVar::operator=(short int siInterger)
{
	CVar& a = *this;
	a.m_Type = SInterger;
	a.m_MixedVar.Interger = siInterger;
	//if (this->m_Type == String)  HeapFree(GetProcessHeap(), 0, this->m_MixedVar.Pointer);
	return a;
}

CVar CVar::operator=(int iInterger)
{
	CVar& a = *this;
	a.m_Type = Interger;
	a.m_MixedVar.Interger = iInterger;
	//if (this->m_Type == String)  HeapFree(GetProcessHeap(), 0, this->m_MixedVar.Pointer);
	return a;
}

CVar CVar::operator=(unsigned short int usiInterger)
{
	CVar& a = *this;
	a.m_Type = USInterger;
	a.m_MixedVar.Interger = usiInterger;
	//if (this->m_Type == String)  HeapFree(GetProcessHeap(), 0, this->m_MixedVar.Pointer);
	return a;
}

CVar CVar::operator=(unsigned int uiInterger)
{
	CVar& a = *this;
	a.m_Type = UInterger;
	a.m_MixedVar.Interger = uiInterger;
	//if (this->m_Type == String)  HeapFree(GetProcessHeap(), 0, this->m_MixedVar.Pointer);
	return a;
}

CVar CVar::operator=(unsigned long int uliInterger)
{
	CVar& a = *this;
	a.m_Type = ULInterger;
	a.m_MixedVar.Interger = uliInterger;
	//if (this->m_Type == String)  HeapFree(GetProcessHeap(), 0, this->m_MixedVar.Pointer);
	return a;
}

CVar CVar::operator=(float fReal)
{
	CVar& a = *this;
	a.m_Type = Real;
	a.m_MixedVar.Real = fReal;
	//if (this->m_Type == String)  HeapFree(GetProcessHeap(), 0, this->m_MixedVar.Pointer);
	return a;
}

CVar::operator void*()
{
	return this->m_MixedVar.Pointer;
}

CVar::operator char*()
{
	return (char*)this->m_MixedVar.Pointer;
}

CVar::operator short()
{
	return (this->m_Type==Real)?this->m_MixedVar.Real:this->m_MixedVar.Interger;
}

CVar::operator unsigned int()
{
	return (this->m_Type==Real)?this->m_MixedVar.Real:this->m_MixedVar.Interger;
}

CVar::operator int()
{
	return (this->m_Type==Real)?this->m_MixedVar.Real:this->m_MixedVar.Interger;
}

CVar::operator long()
{
	return (this->m_Type==Real)?this->m_MixedVar.Real:this->m_MixedVar.Interger;
}

CVar::operator float()
{
	return this->m_MixedVar.Real;
}
