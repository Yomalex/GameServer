#pragma once

#define Error(EventID, ErrorNo, ErrorStr) \
this->DispCallBack(EventID, 5, this->m_szName, __FILE__, __LINE__, ErrorNo, ErrorStr)

#define Msg(color, text)\
this->Loader->invoke("CONSOLE", "Message", 3, color, this->m_szName, text)

enum ConColors
{
	Con_Black,
	Con_Blue,
	Con_Green,
	Con_Aqua,
	Con_Red,
	Con_Purple,
	Con_Yellow,
	Con_White,
	Con_Gray,
	Con_LBlue,
	Con_LGreen,
	Con_LAqua,
	Con_LRed,
	Con_LPurple,
	Con_LYellow,
	Con_LWhite,
};