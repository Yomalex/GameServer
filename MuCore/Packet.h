#pragma once
#include "spe.h"
#include "SimpleModulus.h"

#define DEFAULT_PCK_SIZE 1024*3

struct NWORD // Network Word
{
	union
	{
		struct // Number Bytes
		{
			BYTE High;
			BYTE Low;
		};
		WORD Word; // Network Word
	};
	NWORD();
	NWORD(WORD wNNumber) { this->operator=(wNNumber); }
	NWORD(BYTE Low, BYTE High);

	WORD Number(); // Normal Number
	void Number(WORD wNumber);

	NWORD operator =(WORD wNumber); // Assing Network Number

	operator WORD();
	operator BYTE();
};

struct WzWHead
{
	NWORD m_Size;
	BYTE m_Body[1];
};
struct WzBHead
{
	BYTE m_Size;
	BYTE m_Body[1];
};

template<typename _T>
class Packet
{
public:
	Packet() { ZeroMemory(m_Binary, DEFAULT_PCK_SIZE); }
	Packet(BYTE * lpPacket) { this->operator=(lpPacket); }
	Packet(BYTE Head)
	{
		switch (this->m_hCode = Head)
		{
		case 0xC1:
		case 0xC3:
			this->bHead.m_Size = sizeof(_T);
			break;
		case 0xC2:
		case 0xC4:
			this->wHead.m_Size.Number(sizeof(_T));
			break;
		}
	}
	~Packet() {}

	void raw(void * lpPacket, int Len)
	{
		memcpy(m_Binary, lpPacket, Len);
	}

	UINT size() {
		return (this->m_hCode == 0xC1 || this->m_hCode == 0xC3) ?
			this->bHead.m_Size : this->wHead.m_Size.Number();
	}

	_T* operator ->() { 
		return (_T*)((this->m_hCode == 0xC1 || this->m_hCode == 0xC3)?
			this->bHead.m_Body : this->wHead.m_Body);
	}

	Packet operator =(BYTE * lpPacket);
	BYTE operator [](unsigned int index) { return this->m_Binary[index]; }

	operator char*() { return (char*)this->m_Binary; }

private:
	union
	{
		BYTE m_Binary[DEFAULT_PCK_SIZE];
		BYTE m_hCode;
		WzWHead wHead;
		WzBHead bHead;
	};
};

template<typename _T>
Packet<_T> Packet<_T>::operator=(BYTE * lpPacket)
{
	static CStreamPacketEngine_Server PacketStream;
	// Copio los 3 primeros Bytes a la clase
	// Ahi se almacena la cabecera del packet
	memcpy(this->m_Binary, lpPacket, 3);
	int PSize = 0;

	switch (this->m_hCode)
	{
	case 0xC1:
	case 0xC3:
		// Copio el packet completo
		memcpy(this->m_Binary, lpPacket, PSize = this->bHead.m_Size);
		break;
	case 0xC2:
	case 0xC4:
		// Copio el packet completo
		memcpy(this->m_Binary, lpPacket, PSize = this->wHead.m_Size.Number());
		break;
	default:
		return Packet<_T>();
	}
	PacketStream.Clear();

	// Es un Packet Protegido?
	if (this->m_hCode == 0xC3 || this->m_hCode == 0xC4)
	{
		Packet Dec; // Packet Intermedio
		int result;

		switch (this->m_hCode)
		{
		case 0xC3:
			result = g_SimpleModulusCS.Decrypt(Dec.bHead.m_Body, this->bHead.m_Body, this->bHead.m_Size - 2);
			if (result > 0)
			{
				Dec.m_Binary[1] = 0xC1;
				Dec.m_Binary[2] = result + 2;
				PacketStream.AddData(Dec.m_Binary + 1, result + 2);
			}
			break;
		case 0xC4:
			result = g_SimpleModulusCS.Decrypt(Dec.wHead.m_Body, this->wHead.m_Body, this->wHead.m_Size.Number() - 3);
			if (result > 0)
			{
				Dec.m_Binary[1] = 0xC2;
				*(NWORD*)(Dec.m_Binary + 2) = result + 2;
				PacketStream.AddData(Dec.m_Binary + 1, result + 3);
			}
			break;
		}

	}
	else
	{
		// Agregar directamente el Packet al Stream
		PacketStream.AddData(this->m_Binary, PSize);
	}

	PacketStream.ExtractPacket(this->m_Binary);
	return *this;
}