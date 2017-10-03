#pragma once
class CVar
{
	enum VarType
	{
		None,
		String,
		Pointer,
		SInterger,
		Interger,
		LInterger,
		USInterger,
		UInterger,
		ULInterger,
		Real
	};
public:
	CVar();
	~CVar();

	CVar operator =(void*lPointer);
	CVar operator =(const char*lPointer);
	CVar operator =(char*lPointer) { return operator =((const char *)lPointer); }
	CVar operator=(short int siInterger);
	CVar operator=(int iInterger);
	CVar operator=(unsigned short int usiInterger);
	CVar operator=(unsigned int uiInterger);
	CVar operator=(unsigned long int uliInterger);
	CVar operator=(float Real);

	operator void*();
	operator int*() { return (int*)this->operator void *(); }
	operator short*() { return (short*)this->operator void *(); }
	operator long*() { return (long*)this->operator void *(); }

	operator char*();
	operator unsigned char*() { return (unsigned char*)operator char*(); }

	operator short();
	operator unsigned short() { return (unsigned short)operator short(); }

	operator int();
	operator unsigned int();

	operator long();
	operator unsigned long() { return m_MixedVar.Interger; }

	operator float();

	void copy(void* _Dest)
	{
		switch (m_Type)
		{
		case SInterger:
		case USInterger:
			//memcpy(_Dest, &m_MixedVar.Interger, 2);
			*(unsigned short*)_Dest = m_MixedVar.Interger;
			break;
		case Interger:
		case UInterger:
			//memcpy(_Dest, &m_MixedVar.Interger, 4);
			*(unsigned int*)_Dest = m_MixedVar.Interger;
			break;
		case LInterger:
		case ULInterger:
			*(unsigned long*)_Dest = m_MixedVar.Interger;
			break;
		case Real:
			//memcpy(_Dest, &m_MixedVar.Interger, 8);
			*(float*)_Dest = m_MixedVar.Real;
			break;
		case Pointer:
		case String:
			//memcpy(_Dest, (void*)&m_MixedVar.Pointer, sizeof(void*));
			*(void**)_Dest = m_MixedVar.Pointer;
			break;
		}
	}
	void copy(void ** _Dest)
	{
		*_Dest = m_MixedVar.Pointer;
	}
	void copy(char ** _Dest)
	{
		*_Dest = m_MixedVar.pString;
	}

private:
	VarType m_Type;
	union MyUnion
	{
		char * pString;
		void * Pointer;
		long Interger;
		double Real;
	} m_MixedVar;
};

