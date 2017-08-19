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
	operator char*();
	operator short();
	operator unsigned int();
	operator int();
	operator long();
	operator unsigned long() { return m_MixedVar.Interger; }
	operator float();
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

