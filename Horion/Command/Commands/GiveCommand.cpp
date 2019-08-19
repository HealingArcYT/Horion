#include "GiveCommand.h"



GiveCommand::GiveCommand() : IMCCommand("give", "spawn items", "<itemName> <count> <itemData>") 
{
}


GiveCommand::~GiveCommand()
{
}

bool GiveCommand::execute(std::vector<std::string>* args)
{
	assertTrue(args->size() > 2);
	int  count = assertInt(args->at(2));
	char itemData = 0;
	if (args->size() > 3)
		itemData = assertInt(args->at(3));

	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	C_Inventory* inv = supplies->inventory;
	C_BlockLegacy* legacy = nullptr;
	C_Item* yeet = nullptr;
	
	static uintptr_t** VanillaBlocks__mStonePtr = 0x0;
	static uintptr_t** VanillaItems__mShovel_ironPtr = 0x0;
	
	if (VanillaBlocks__mStonePtr == 0x0) {
		uintptr_t sigOffset = Utils::FindSignature("48 8B 05 ?? ?? ?? ?? 4C 8B 00 4D 39 01 75 13");
		int offset = *reinterpret_cast<int*>(sigOffset + 3);
		VanillaBlocks__mStonePtr = reinterpret_cast<uintptr_t * *>(sigOffset + offset + /*length of instruction*/ 7);
		if (VanillaBlocks__mStonePtr == 0x0 || sigOffset == 0x0)
			logF("VanillaBlocks sig not working!!!");
	}

	if (VanillaItems__mShovel_ironPtr == 0x0) {
		uintptr_t sigOffset = Utils::FindSignature("48 8B 05 ?? ?? ?? ?? 48 85 C0 74 05 4C 8B 00 EB 03 4C 8B C3 49 3B F0 74 32 48 8B 05 ?? ?? ?? ?? 48 85 C0 74 05 4C 8B 00 EB 03 4C 8B C3 49 3B F0 74 19 48 8B 05 ?? ?? ?? ?? 48 85 C0 74 05 4C 8B 00 EB 03 4C 8B C3 49 3B F0 75 62 4D 8B 45 ?? 48 8B 01 49 39 00 75 56 49 8B 49 ?? 48 8B 02 48 39 01 75 4A 48 89 5F ??");
		int offset = *reinterpret_cast<int*>(sigOffset + 3);
		VanillaItems__mShovel_ironPtr = reinterpret_cast<uintptr_t * *>(sigOffset + offset + /*length of instruction*/ 7);
		if (VanillaItems__mShovel_ironPtr == 0x0 || sigOffset == 0x0)
			logF("VanillaItems sig not working!!!");
	}

	if (VanillaBlocks__mStonePtr != nullptr)
	{
		for (int i = 0; i < 465; i++)
		{
			if (VanillaBlocks__mStonePtr[i] != nullptr &&
				strcmp(reinterpret_cast<C_BlockLegacy*>(VanillaBlocks__mStonePtr[i][0])->name.getText(), args->at(1).c_str()) == 0)
			{
				legacy = reinterpret_cast<C_BlockLegacy*>(VanillaBlocks__mStonePtr[i][0]);
				break;
			}
		}
	}

	if (VanillaItems__mShovel_ironPtr != nullptr && legacy == nullptr)
	{
		for (int i = 0; i < 233; i++)
		{
			if (VanillaItems__mShovel_ironPtr[i] != nullptr &&
				strcmp(reinterpret_cast<C_Item*>(VanillaItems__mShovel_ironPtr[i][0])->name.getText(), args->at(1).c_str()) == 0)
			{
				yeet = reinterpret_cast<C_Item*>(VanillaItems__mShovel_ironPtr[i][0]);
				break;
			}
		}
	}

	C_ItemStack* yot = reinterpret_cast<C_ItemStack*>(malloc(0x88));

	if (legacy == nullptr && yeet == nullptr)
	{
		clientMessageF("%sInvalid Item!", RED);
		return true;
	}
	else if (legacy != nullptr)
		yot->ItemStackConstructor__1(legacy, count);
	else
		yot->ItemStackConstructor(yeet, count, itemData);
	
	if(yot != nullptr)
		yot->count = count;

	inv->addItemToFirstEmptySlot(yot);
	
	clientMessageF("%sSuccessfully given item!", GREEN);
	return true;
}
