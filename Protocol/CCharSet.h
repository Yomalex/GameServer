#pragma once
struct _tagCSClass
{
	BYTE Action : 3;
	BYTE ChangeUP2 : 1; //08
	BYTE ChangeUP : 1; //10
	BYTE Class : 3; //E0
};
enum Inventory
{
	INV_Rigth_Hand = 0,
	INV_Left_Hand,
	INV_Helmet,
	INV_Armor,
	INV_Pants,
	INV_Gloves,
	INV_Boots,
	INV_Wings,
	INV_Helper,
	INV_Necklease,
	INV_Rigth_Ring,
	INV_Left_Ring,
	INV_BlockInventory,
};
#pragma pack(push,1)
struct _tagDBItem16
{
	BYTE Index;//0
	struct//1
	{
		BYTE Option3L : 2;
		BYTE Luck : 1;
		BYTE Level : 4;
		BYTE Skill : 1;
	};
	BYTE Durability;//2
	union uSerial//3-4-5-6
	{
		DWORD dwSerial;
		struct
		{
			BYTE SerialHH;
			BYTE SerialHL;
			BYTE SerialLH;
			BYTE SerialLL;
		};
	} Serial;
	struct//7
	{
		BYTE Excellent : 6;
		BYTE Option3H : 1;
		BYTE TypeL : 1;
	};
	BYTE SetOption;//8
	struct//9
	{
		BYTE Unk : 3;
		BYTE Option380 : 1;
		BYTE TypeH : 4;
	};
	BYTE Harmony;//10
	BYTE Sockets[5];//11-12-13-14-15

	BYTE GetItemType()
	{
		return (this->TypeH << 1) | this->TypeL;
	}
	void SetItemType(BYTE Type)
	{
		this->TypeL = Type & 1;
		this->TypeH = Type >> 1;
	}
	WORD GetItemNumber()
	{
		return MAKEWORD(this->Index, GetItemType());
	}
	void SetItemNumber(WORD Number)
	{
		this->Index = Number & 0xff;
		this->SetItemType(Number >> 8);
	}
	BYTE GetOption3()
	{
		return (Option3H << 2) | Option3L;
	}
	void SetOption3(BYTE Option)
	{
		Option3L = Option & 3;
		Option3H = Option >> 2;
	}
};
struct _tagDBItem4
{
	BYTE Index;//0
	struct // 1
	{
		BYTE Option3L : 2;
		BYTE Option2 : 1;
		BYTE Level : 4;
		BYTE Option1 : 1;
	};
	struct//2
	{
		BYTE Excellent : 6;
		BYTE Option3H : 1;
		BYTE TypeL : 1;
	};
	struct//3
	{
		BYTE Unk : 3;
		BYTE Option380 : 1;
		BYTE TypeH : 4;
	};
	BYTE GetItemType()
	{
		return (TypeH >> 3) | TypeL;
	}
	WORD GetItemNumber()
	{
		return (GetItemType() << 8) | Index;
	}
};
struct _tagCharset18
{
	_tagCSClass ClientClass;//0
	BYTE RHIndex;//1
	BYTE LHIndex;//2
	struct {//3
		BYTE AIndexL : 4;
		BYTE HIndexL : 4;
	};
	struct {//4
		BYTE GIndexL : 4;
		BYTE PIndexL : 4;
	};
	struct//5
	{
		BYTE Helper : 2;
		BYTE Wings : 2;
		BYTE BIndexL : 4;
	};
	struct//6-7-8-9
	{
		DWORD RHLevel : 3;//6
		DWORD LHLevel : 3;
		DWORD HLevel : 3;//7
		DWORD ALevel : 3;
		DWORD PLevel : 3;
		DWORD GLevel : 3;//8
		DWORD BLevel : 3;
		DWORD WLevel : 3;
		//9
		DWORD Wings2 : 3;
		DWORD BIndex1 : 1;
		DWORD GIndex1 : 1;
		DWORD PIndex1 : 1;
		DWORD AIndex1 : 1;
		DWORD HIndex1 : 1;
	};
	struct // 10
	{
		BYTE Dinorant : 1;
		BYTE RHExcellent : 1;
		BYTE LHExcellent : 1;
		BYTE HExcellent : 1;
		BYTE AExcellent : 1;
		BYTE PExcellent : 1;
		BYTE GExcellent : 1;
		BYTE BExcellent : 1;
	};
	struct // 11
	{
		BYTE FullSet : 1;
		BYTE RHSet : 1;
		BYTE LHSet : 1;
		BYTE HSet : 1;
		BYTE ASet : 1;
		BYTE PSet : 1;
		BYTE GSet : 1;
		BYTE BSet : 1;
	};
	struct// 12
	{
		BYTE DarkHorse : 1;
		BYTE Unk : 1;
		BYTE Fenrir : 1;
		BYTE Unk2 : 1;
		BYTE RHType : 4;
	};
	struct // 13
	{
		BYTE HIndexH : 4;
		BYTE LHType : 4;
	};
	struct // 14
	{
		BYTE PIndexH : 4;
		BYTE AIndexH : 4;
	};
	struct // 15
	{
		BYTE BIndexH : 4;
		BYTE GIndexH : 4;
	};
	struct // 16
	{
		BYTE FenrirBlack : 1;
		BYTE FenrirBlue : 1;
		BYTE Unk3 : 3;
		BYTE Unk4 : 3;
	};
	struct //17
	{
		BYTE Unk5 : 1;
		BYTE Unk6 : 7;
	};


};
#pragma pack(pop)

#define Set18Index(var, name, value) \
var ##.## name ##L = value&0x0f; \
var ##.## name ##1 = (value>>4)&1; \
var ##.## name ##H = (value>>5)&0x0f
#define LevelSmall(x) (BYTE)((x-1)/2)

class CCharSet
{
public:
	CCharSet();
	~CCharSet();

	void SetServerClass(BYTE Class);
	void ProcessInventory16(BYTE * dbInventory);
	void ProcessInventory4(BYTE * dbInventory);

	union {
		_tagCharset18 CharSet18;
		BYTE SmallInventory[18];
	};
};

