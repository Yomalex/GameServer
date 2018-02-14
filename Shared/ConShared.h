#pragma once

#define Error(EventID, ErrorNo, ErrorStr) this->DispCallBack(EventID, 5, this->m_szName, __FILE__, __LINE__, ErrorNo, ErrorStr)