#include "stdafx.h"
#include "FlexVar.h"

FlexVar FlexVar::operator=(const char * szValue)
{
	this->szContent = szValue;
	
	bool dot = false;
	while((*szValue >= '0' && *szValue <= '9') || !dot && (*szValue == '.'))
	{
		dot |= *szValue == '.';
		szValue++;
	}

	if (
		*szValue == 0 || 
		*szValue == '\n' || 
		*szValue == '\r' || 
		*szValue == '\t' ||
		*szValue == ' ')
	{
		this->dfContent = atof(this->szContent.c_str());
	}
	else
	{
		this->dfContent = 0;
	}

	return FlexVar();
}

FlexVar FlexVar::operator=(double dfValue)
{
	char szPrintf[100];
	sprintf_s(szPrintf, "%f", dfValue);
	this->szContent = szPrintf;
	this->dfContent = dfValue;
	return *this;
}

FlexVar FlexVar::operator=(float fValue)
{
	return this->operator=((double)fValue);
}

FlexVar FlexVar::operator=(long lValue)
{
	return this->operator=((double)lValue);
}

FlexVar FlexVar::operator=(int iValue)
{
	return this->operator=((double)iValue);
}

FlexVar FlexVar::operator=(short sValue)
{
	return this->operator=((double)sValue);
}

FlexVar::operator const char*()
{
	return szContent.c_str();
}

FlexVar::operator double()
{
	return dfContent;
}

FlexVar::operator float()
{
	return dfContent;
}

FlexVar::operator long()
{
	return (long)dfContent;
}

FlexVar::operator int()
{
	return (int)dfContent;
}

FlexVar::operator short()
{
	return (short)this->dfContent;
}

