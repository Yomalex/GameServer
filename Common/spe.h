#ifndef SPE_H
#define SPE_H

// GS-N 0.99.60T - 0x00474A10

class CStreamPacketEngine_Server
{

public:

	CStreamPacketEngine_Server()	// Good
	{
		this->Clear();
	};

	virtual ~CStreamPacketEngine_Server()	// Good
	{
		return;
	};

	void Clear()	// Good 
	{
		this->m_PacketSize = 0;
	};

	BOOL AddData(unsigned char* pSrc, WORD wSize)	// Good 
	{
		if (((this->m_PacketSize + wSize) >= 2048) || (wSize == 0))
		{
			int iSize = 2048;
			return FALSE;
		}

		memcpy((void*)&this->m_PacketStream[this->m_PacketSize], pSrc, wSize);
		this->m_PacketSize += wSize;
		return TRUE;
	};

	BOOL ExtractPacket(unsigned char* pTar)	// Good
	{
		WORD wSize;
		BYTE byXorFilter[32];

		BYTE byTemp[2048];



		switch (this->m_PacketStream[0])
		{
		case 0xC1:
			wSize = this->m_PacketStream[1];
			break;

		case 0xC2:
			wSize = this->m_PacketStream[1] * 256 + this->m_PacketStream[2];
			break;

		default:
			return true;
			break;
		}

		if (this->m_PacketSize < wSize)
		{
			return 2;
		}

		byXorFilter[0] = 0xE7;
		byXorFilter[1] = 0x6D;
		byXorFilter[2] = 0x3A;
		byXorFilter[3] = 0x89;
		byXorFilter[4] = 0xBC;
		byXorFilter[5] = 0xB2;
		byXorFilter[6] = 0x9F;
		byXorFilter[7] = 0x73;
		byXorFilter[8] = 0x23;
		byXorFilter[9] = 0xA8;
		byXorFilter[10] = 0xFE;
		byXorFilter[11] = 0xB6;
		byXorFilter[12] = 0x49;
		byXorFilter[13] = 0x5D;
		byXorFilter[14] = 0x39;
		byXorFilter[15] = 0x5D;
		byXorFilter[16] = 0x8A;
		byXorFilter[17] = 0xCB;
		byXorFilter[18] = 0x63;
		byXorFilter[19] = 0x8D;
		byXorFilter[20] = 0xEA;
		byXorFilter[21] = 0x7D;
		byXorFilter[22] = 0x2B;
		byXorFilter[23] = 0x5F;
		byXorFilter[24] = 0xC3;
		byXorFilter[25] = 0xB1;
		byXorFilter[26] = 0xE9;
		byXorFilter[27] = 0x83;
		byXorFilter[28] = 0x29;
		byXorFilter[29] = 0x51;
		byXorFilter[30] = 0xE8;
		byXorFilter[31] = 0x56;

		this->XorData(wSize - 1, (this->m_PacketStream[0] == 0xC1 ? 2 : 3), -1);
		memcpy(pTar, this->m_PacketStream, wSize);
		this->m_PacketSize -= wSize;
		memcpy(byTemp, &this->m_PacketStream[wSize], this->m_PacketSize);
		memcpy(this->m_PacketStream, byTemp, this->m_PacketSize);
		return false;
	};

	void XorData(int iStart, int iEnd, int iDir)	// Good -JNS
	{
		if (iStart < iEnd)
		{
			return;
		}

		unsigned char byXorFilter[32];

		byXorFilter[0] = 0xE7;
		byXorFilter[1] = 0x6D;
		byXorFilter[2] = 0x3A;
		byXorFilter[3] = 0x89;
		byXorFilter[4] = 0xBC;
		byXorFilter[5] = 0xB2;
		byXorFilter[6] = 0x9F;
		byXorFilter[7] = 0x73;
		byXorFilter[8] = 0x23;
		byXorFilter[9] = 0xA8;
		byXorFilter[10] = 0xFE;
		byXorFilter[11] = 0xB6;
		byXorFilter[12] = 0x49;
		byXorFilter[13] = 0x5D;
		byXorFilter[14] = 0x39;
		byXorFilter[15] = 0x5D;
		byXorFilter[16] = 0x8A;
		byXorFilter[17] = 0xCB;
		byXorFilter[18] = 0x63;
		byXorFilter[19] = 0x8D;
		byXorFilter[20] = 0xEA;
		byXorFilter[21] = 0x7D;
		byXorFilter[22] = 0x2B;
		byXorFilter[23] = 0x5F;
		byXorFilter[24] = 0xC3;
		byXorFilter[25] = 0xB1;
		byXorFilter[26] = 0xE9;
		byXorFilter[27] = 0x83;
		byXorFilter[28] = 0x29;
		byXorFilter[29] = 0x51;
		byXorFilter[30] = 0xE8;
		byXorFilter[31] = 0x56;

		for (int i = iStart; i != iEnd; i += iDir)
		{
			this->m_PacketStream[i] ^= this->m_PacketStream[i - 1] ^ byXorFilter[(i&sizeof(byXorFilter) - 1)];
		}
	};





private:

	WORD m_PacketSize;
	unsigned char m_PacketStream[2048];

};


#endif