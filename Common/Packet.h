#pragma once
#include "spe.h"
//#include "SimpleModulus.h"
#include "../PluginBase/Plugin.h"
#include "../PluginBase/Loader.h"

#define DEFAULT_PCK_SIZE 1024*3

#pragma pack(push,1)

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
	CPlugin * m_Owner;
public:
	Packet(CPlugin* owner) : m_Owner(owner) { ZeroMemory(m_Binary, DEFAULT_PCK_SIZE); }
	Packet(CPlugin* owner, BYTE * lpPacket) : m_Owner(owner) { this->operator=(lpPacket); }
	Packet(CPlugin* owner, BYTE Head) : m_Owner (owner)
	{
		switch (this->m_hCode = Head)
		{
		case 0xC1:
		case 0xC3:
			this->m_bHead.m_Size = sizeof(_T)+3;
			break;
		case 0xC2:
		case 0xC4:
			this->m_wHead.m_Size.Number(sizeof(_T)+4);
			break;
		}
	}
	~Packet() {}

	void Owner(CPlugin* owner) {
		m_Owner = owner;
	};

	void raw(void * lpPacket, int Len) const
	{
		memcpy(m_Binary, lpPacket, Len);
	}
	BYTE * enc();

	BYTE code() const{
		return (this->m_hCode == 0xC1) ? this->m_bHead.m_Body[0]: this->m_wHead.m_Body[0];
	}

	char * body() {
		return (char*)((this->m_hCode == 0xC1) ? this->m_bHead.m_Body : this->m_wHead.m_Body);
	}

	bool encrypt() const { return this->m_Encrypt; };

	WORD size() {
		return (this->m_hCode == 0xC1 || this->m_hCode == 0xC3) ?
			this->m_bHead.m_Size : this->m_wHead.m_Size.Number();
	}

	_T* operator ->() { 
		return (_T*)((this->m_hCode == 0xC1 || this->m_hCode == 0xC3)?
			this->m_bHead.m_Body : this->m_wHead.m_Body);
	}

	Packet operator =(BYTE * lpPacket);
	BYTE& operator [](unsigned int index) { return this->m_Binary[index]; }

	operator char*() { return (char*)this->m_Binary; }

private:
	union
	{
		BYTE m_Binary[DEFAULT_PCK_SIZE];
		struct {
			BYTE m_hCode;
			union {
				WzWHead m_wHead;
				WzBHead m_bHead;
			};
		};
	};

	bool m_Encrypt;
	CStreamPacketEngine_Server PacketStream;
};

#pragma pack(pop)

template<typename _T>
Packet<_T> Packet<_T>::operator=(BYTE * lpPacket)
{
	// Copio los 3 primeros Bytes a la clase
	// Ahi se almacena la cabecera del packet
	memcpy(this->m_Binary, lpPacket, 3);
	int PSize = 0;

	switch (this->m_hCode)
	{
	case 0xC1:
	case 0xC3:
		// Copio el packet completo
		memcpy(this->m_Binary, lpPacket, PSize = this->m_bHead.m_Size);
		break;
	case 0xC2:
	case 0xC4:
		// Copio el packet completo
		memcpy(this->m_Binary, lpPacket, PSize = this->m_wHead.m_Size.Number());
		break;
	default:
		return Packet<_T>(this->m_Owner);
	}
	PacketStream.Clear();

	// Es un Packet Protegido?
	if (this->m_hCode == 0xC3 || this->m_hCode == 0xC4)
	{
		Packet<_T> Dec(this->m_Owner); // Packet Intermedio
		int result;

		switch (this->m_hCode)
		{
		case 0xC3:
			result = this->m_bHead.m_Size - 2;
			this->m_Owner->Parent()->invoke("Modulus", "Decrypt", 3, Dec.m_bHead.m_Body, this->m_bHead.m_Body, &result);
			//result = g_SimpleModulusCS.Decrypt(Dec.m_bHead.m_Body, this->m_bHead.m_Body, this->m_bHead.m_Size - 2);
			if (result > 0)
			{
				Dec.m_Binary[0] = 0xC1;
				Dec.m_Binary[1] = result + 2;
				PacketStream.AddData(Dec.m_Binary + 1, result + 2);
			}
			break;
		case 0xC4:
			result = this->m_bHead.m_Size - 3;
			this->m_Owner->Parent()->invoke("Modulus", "Decrypt", 3, Dec.m_wHead.m_Body, this->m_wHead.m_Body, &result);
			//result = g_SimpleModulusCS.Decrypt(Dec.m_wHead.m_Body, this->m_wHead.m_Body, this->m_wHead.m_Size.Number() - 3);
			if (result > 0)
			{
				Dec.m_Binary[0] = 0xC2;
				*(NWORD*)(Dec.m_Binary + 1) = result + 3;
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

template<typename _T>
BYTE * Packet<_T>::enc()
{
	// Es un Packet Protegido?
	if (this->m_hCode == 0xC3 || this->m_hCode == 0xC4)
	{
		Packet<_T> enc(this->m_hCode);
		int result;
		switch (this->m_hCode)
		{
		case 0xC3:
			result = g_SimpleModulusSC.Encrypt(enc.m_bHead.m_Body, this->m_bHead.m_Body, this->m_bHead.m_Size - 2);
			if (result > 0)
			{
				enc.m_Binary[0] = 0xC3;
				enc.m_Binary[1] = result + 2;
			}
			break;
		case 0xC4:
			result = g_SimpleModulusSC.Encrypt(enc.wHead.m_Body, this->m_wHead.m_Body, this->m_wHead.Number() - 3);
			if (result > 0)
			{
				enc.m_Binary[0] = 0xC4;
				*(NWORD*)(enc.m_Binary + 1) = result + 2;
			}
			break;
		}

		memcpy(this->m_Binary, enc.m_Binary, enc.size());
	}
}