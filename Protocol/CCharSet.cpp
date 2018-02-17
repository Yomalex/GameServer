#include "stdafx.h"
#include "CCharSet.h"


CCharSet::CCharSet()
{
}


CCharSet::~CCharSet()
{
}

void CCharSet::SetServerClass(BYTE Class)
{
	this->CharSet18.ClientClass.Class = Class >> 4;
	this->CharSet18.ClientClass.ChangeUP = (Class & 1);
	this->CharSet18.ClientClass.ChangeUP2 = (Class & 2) != 0;
}

void CCharSet::ProcessInventory16(BYTE * dbInventory)
{
	_tagDBItem16 * pInventory = (_tagDBItem16*)dbInventory;
}

void CCharSet::ProcessInventory4(BYTE * dbInventory)
{
	_tagDBItem4 * pInventory = (_tagDBItem4*)dbInventory;

	ZeroMemory(&this->CharSet18, sizeof(this->CharSet18));

	this->CharSet18.RHIndex = pInventory[INV_Rigth_Hand].Index;
	this->CharSet18.RHType = pInventory[INV_Rigth_Hand].GetItemType();
	this->CharSet18.RHLevel = LevelSmall(pInventory[INV_Rigth_Hand].Level);
	this->CharSet18.RHExcellent = pInventory[INV_Rigth_Hand].Excellent != 0;

	this->CharSet18.LHIndex = pInventory[INV_Left_Hand].Index;
	this->CharSet18.LHType = pInventory[INV_Left_Hand].GetItemType();
	this->CharSet18.LHLevel = LevelSmall(pInventory[INV_Left_Hand].Level);
	this->CharSet18.LHExcellent = pInventory[INV_Left_Hand].Excellent != 0;

	Set18Index(this->CharSet18, HIndex, pInventory[INV_Helmet].Index);
	this->CharSet18.HExcellent = pInventory[INV_Helmet].Excellent;

	Set18Index(this->CharSet18, AIndex, pInventory[INV_Armor].Index);
	this->CharSet18.AExcellent = pInventory[INV_Armor].Excellent;

	Set18Index(this->CharSet18, PIndex, pInventory[INV_Pants].Index);
	this->CharSet18.PExcellent = pInventory[INV_Pants].Excellent;

	Set18Index(this->CharSet18, GIndex, pInventory[INV_Gloves].Index);
	this->CharSet18.GExcellent = pInventory[INV_Gloves].Excellent;

	Set18Index(this->CharSet18, BIndex, pInventory[INV_Boots].Index);
	this->CharSet18.BExcellent = pInventory[INV_Boots].Excellent;
}
