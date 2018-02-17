#pragma once

#define Error(EventID, ErrorNo, ErrorStr) \
this->DispCallBack(EventID, 5, this->m_szName, __FILE__, __LINE__, ErrorNo, ErrorStr)

#define Message(color, text)\
this->Loader->invoke("CONSOLE", "Message", 3, color, this->m_szName, text)