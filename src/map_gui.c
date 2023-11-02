#include <atari.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <peekpoke.h>
#include "fixed.h"
#include "atari_fn.h"

#include "c_asm_memory.h"
#include "constants.h"
#include "globals.h"
#include "draw_map.h"

// Map GUI has 4 parts
// A = Menu text of 7 lines, showing 5 entries [8x7]
// B = 5x5 action icon, showing the unlock requirements on the icon [7x7]
// C = Pouch, Chest or Ghost inventory or actions [18x7]
// D = Player's Inventory [7x7]

extern void wait(void);

byte PouchInteraction(void);
byte TradeInteraction(void);
void BuildTradingOptions();

typedef struct MapMenuItem
{
	char Type;			// What action 0-Back, 1=Ghost
	byte Icon;			// Which icon to draw
	byte* NormalName;
	byte* InverseName;
	void (*DrawUnlockCostFunc)();
	/* data */
} MapMenuItem;

MapMenuItem MMI_Back = {
	0,		// Back
	ICON_BACK,
	(byte*)FIXED_BACKTEXT,
	(byte*)FIXED_BACKTEXTINV,
	NULL
};

MapMenuItem MMI_GhostAttack = {
	MENU_ACTION_ATTACK_GHOST,		// OBJECT_GHOST_BOB(1) - OBJECT_GHOST_PANDA(5)
	ICON_GHOST,
	(byte*)FIXED_ATTACKTEXT,
	(byte*)FIXED_ATTACKTEXTINV,
	(void*)FIXED_DRAWATTACKCOSTFORGHOST
};

MapMenuItem MMI_GhostTake = {
	MENU_ACTION_TAKE_GHOST,
	ICON_GHOST,
	(byte*)FIXED_TAKETEXT,
	(byte*)FIXED_TAKETEXTINV,
	NULL
};

MapMenuItem MMI_GhostTrade = {
	MENU_ACTION_TRADE_GHOST,		// OBJECT_GHOST_BOB(1) - OBJECT_GHOST_PANDA(5)
	ICON_TRADE,
	(byte*)FIXED_TRADETEXT,
	(byte*)FIXED_TRADETEXTINV,
	NULL
};

// ==============================================
// Chest menu options
MapMenuItem MMI_ChestLocked = {
	MENU_ACTION_UNLOCK_CHEST,
	ICON_CHEST,
	(byte*)FIXED_UNLOCKTEXT,
	(byte*)FIXED_UNLOCKTEXTINV,
	(void*)FIXED_DRAWLOCKEDCHESTCOST
};

MapMenuItem MMI_ChestTake = {
	MENU_ACTION_TAKE_CHEST,
	ICON_CHEST_OPEN,
	(byte*)FIXED_TAKETEXT,
	(byte*)FIXED_TAKETEXTINV,
	NULL
};


// ==============================================
MapMenuItem MMI_Vortex = {
	MENU_ACTION_VORTEX,
	ICON_VORTEX,
	(byte*)FIXED_WARPTEXT,
	(byte*)FIXED_WARPTEXTINV,
	NULL
};

MapMenuItem MMI_Switch = {
	MENU_ACTION_SWITCH,
	ICON_SWITCH,
	(byte*)FIXED_SWITCHTEXT,
	(byte*)FIXED_SWITCHTEXTINV,
	NULL
};

MapMenuItem MMI_ExitIsLocked = {
	MENU_ACTION_LEVEL_EXIT,
	ICON_EXIT,
	(byte*)FIXED_UNLOCKTEXT,
	(byte*)FIXED_UNLOCKTEXTINV,
	(void*)FIXED_DRAWLOCKEDEXITCOST
};

MapMenuItem MMI_Exit = {
	MENU_ACTION_LEVEL_EXIT,
	ICON_EXIT,
	(byte*)FIXED_EXITTEXT,
	(byte*)FIXED_EXITTEXTINV,
	NULL
};

MapMenuItem MMI_Pouch = {
	MENU_ACTION_OPEN_POUCH,
	ICON_POUCH,
	(byte*)FIXED_POUCHTEXT,
	(byte*)FIXED_POUCHTEXTINV,
	NULL
};

#define MENU_HIGHLIGHT_TOP_CHAR 121
#define MENU_HIGHLIGHT_BOTTOM_CHAR 122

MapMenuItem* Menu[10] = {
	&MMI_Back,
};
byte NumMenuItems = 1;
byte WindowTop;
byte WindowStep;				// How far into the window are we 0-4
byte MenuSelectionIndex;

byte CurrentTradingOption;
byte NumTradingOptions;



#define MAX_NUM_TRADE_OPTIONS 10
byte TradingTypeGiveType[MAX_NUM_TRADE_OPTIONS];
byte TradingTypeGiveCount[MAX_NUM_TRADE_OPTIONS];
byte TradingTypeGetType[MAX_NUM_TRADE_OPTIONS];
byte TradingTypeGetCount[MAX_NUM_TRADE_OPTIONS];

void DrawMenu();
void DrawPlayerInventory();

void DrawEverythingAndPlayTakeSound()
{
	DrawMenu();
	DrawPlayerInventory();
	DrawThePowerBar();
	POKE(SFX_CH1, SOUND_USE_AN_ORB);
	WaitFor30Cycles();
}

void DrawOpenChestItems()
{
	DrawSectionC();
	ASM_DATA.SectionCInverse = 0;

	ASM_DATA.SectionCBox0Type = ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeType[0];
	ASM_DATA.SectionCBox1Type = ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeType[1];
	ASM_DATA.SectionCBox2Type = ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeType[2];

	ASM_DATA.SectionCBox0Count = ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeNum[0];
	ASM_DATA.SectionCBox1Count = ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeNum[1];
	ASM_DATA.SectionCBox2Count = ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeNum[2];

	DrawSectionCItems();
}

// ========================================================
// Draw the menu text in normal or highlight
// @ the current (MenuDrawPtr) location
//
void DrawMenuName(MapMenuItem *item, byte inv) {
	if (inv)
		ASM_DATA.FromPtr = item->InverseName;
	else
		ASM_DATA.FromPtr = item->NormalName;

	asm("ldy #7");
	asm("DrawMenuCopyLoop: lda (%w),y", offsetof(_asm_data, FromPtr));
	asm("sta (%w),y", offsetof(_asm_data, MenuDrawPtr));
	asm("dey");
	asm("bpl DrawMenuCopyLoop");
	ASM_DATA.MenuDrawPtr += 40;
}

// Create the menu options
// BACK - Is always the first option
// Add object options
// - GHOST
// - CHEST
void BuildMapGui(void)
{
	BuildTradingOptions();

	// Every menu always start with the BACK option
	NumMenuItems = 1;

	// If on an object, add the options specific to the object
	switch (ASM_DATA.CurrentObject)
	{
		case OBJECT_GHOST_BOB:
		case OBJECT_GHOST_CYCO:
		case OBJECT_GHOST_ALICE:
		case OBJECT_GHOST_SPECULA:
		case OBJECT_GHOST_PANDA:
			{
				if (ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost)
				{
					// Ghost ATTACK option
					Menu[NumMenuItems] = &MMI_GhostAttack;
					++NumMenuItems;
				}
				else {
					Menu[NumMenuItems] = &MMI_GhostTake;
					++NumMenuItems;
				}

				if (NumTradingOptions > 0)
				{
					// Ghost TRADE option
					Menu[NumMenuItems] = &MMI_GhostTrade;
					++NumMenuItems;
				}
			}
			break;

		case OBJECT_CHEST:
			{
				// Figure out if the chest is locked and what type of lock it is
				switch(ASM_DATA.InteractingWithThisObjPtr->Data.Chest.UnlockType)
				{
					case CHEST_LOCKED_TYPE_NONE:
						Menu[NumMenuItems] = &MMI_ChestTake;
						break;

					case CHEST_LOCKED_TYPE_POWER:
					case CHEST_LOCKED_TYPE_ORBS:
					case CHEST_LOCKED_TYPE_KEYS:
						Menu[NumMenuItems] = &MMI_ChestLocked;
						break;
				}
				++NumMenuItems;
			}
			break;

		case OBJECT_VORTEX:
			{
				Menu[NumMenuItems] = &MMI_Vortex;
				++NumMenuItems;
			}
			break;

		case OBJECT_SWITCH_OFF:
		case OBJECT_SWITCH_ON:
			{
				Menu[NumMenuItems] = &MMI_Switch;
				++NumMenuItems;
			}
			break;

		case OBJECT_EXIT:
			{
				// Figure out if the exit needs to be unlocked
				if (ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost0 + ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost1)
				{
					Menu[NumMenuItems] = &MMI_ExitIsLocked;
				}
				else
				{
					Menu[NumMenuItems] = &MMI_Exit;
				}
				++NumMenuItems;
			}
			break;
	}

	// Add the POUCH
	Menu[NumMenuItems] = &MMI_Pouch;
	++NumMenuItems;
}

// ========================================================
// Draw the menu text

void DrawMenu()
{
	MapMenuItem *Interaction;
	MapMenuItem *TheItem;

	ASM_DATA.MenuDrawPtr = (byte *)FIXED_MAPMENUSCREEN;		// Draw the menu text items into this block (8*12)

	// Turn the 'NumMenuItems' Menu[] items into text
	// The menu item text is rendered out as text to the ASM_DATA.MenuDrawPtr location

	for (ASM_DATA.WorkI = WindowTop; ASM_DATA.WorkI < NumMenuItems; ++ASM_DATA.WorkI)
	{
		TheItem = Menu[ASM_DATA.WorkI];
		if (ASM_DATA.WorkI == MenuSelectionIndex) {
			Interaction = TheItem;
			asm("ldy #7");
			asm("lda #%w", MENU_HIGHLIGHT_TOP_CHAR);
			asm("DrawMenuHighlightTopLoop: sta (%w),y", offsetof(_asm_data, MenuDrawPtr));
			asm("dey");
			asm("bpl DrawMenuHighlightTopLoop");
			ASM_DATA.MenuDrawPtr += 40;
		}
		DrawMenuName(TheItem, ASM_DATA.WorkI == MenuSelectionIndex);
		if (ASM_DATA.WorkI == MenuSelectionIndex) {
			asm("ldy #7");
			asm("lda #%w", MENU_HIGHLIGHT_BOTTOM_CHAR);
			asm("DrawMenuHighlightBottomLoop: sta (%w),y", offsetof(_asm_data, MenuDrawPtr));
			asm("dey");
			asm("bpl DrawMenuHighlightBottomLoop");
			ASM_DATA.MenuDrawPtr += 40;
		}
	}

	ASM_DATA.ByteParam0 = Interaction->Icon;
	asm("jsr %w", FIXED_DRAWINTERACTIONICON);
	if (Interaction->DrawUnlockCostFunc)
	{
		(*Interaction->DrawUnlockCostFunc)();
	}
	if (Interaction->Type == MENU_ACTION_TAKE_CHEST || Interaction->Type == MENU_ACTION_TAKE_GHOST) //  && ASM_DATA.InteractingWithThisObjPtr->Data.Chest.UnlockCost == 0)
	{
		DrawOpenChestItems();
	}
	else {
		ClearSectionC();		
	}

}

void StepUp()
{
	if (WindowTop == 0 && WindowStep == 0)
		return;
	if (WindowStep >= 2)
	{
		// Move to the middle then move the window up
		if (WindowTop > 0 && WindowStep > 2)
		{
			// Move up inside the window until we hit pos 2
			--WindowStep;
		}
		else if (WindowTop == 0 && WindowStep >= 2)
		{
			--WindowStep;
		}
		else
		{
			--WindowTop;
		}
	}
	else
	{
		--WindowStep;
	}

	MenuSelectionIndex = WindowTop + WindowStep;
	DrawMenu();
}

void StepDown()
{
	int prevWindowTop = WindowTop;
	int prevWindowStep = WindowStep;

	if (WindowStep < 2)
	{
		++WindowStep;
	}
	else
	{
		// In the second half
		if (WindowTop + 5 < NumMenuItems)
		{
			++WindowTop;
		}
		else
		{
			++WindowStep;
		}
	}
	if (WindowTop + WindowStep == NumMenuItems)
	{
		WindowTop = prevWindowTop;
		WindowStep = prevWindowStep;
	}

	MenuSelectionIndex = WindowTop + WindowStep;

	DrawMenu();
}

// ========================================================
// Draw player's inventory
// Section D
void DrawPlayerInventory()
{
	asm("jsr %w", FIXED_DRAWPLAYERINVENTORY);
	asm("jsr %w", FIXED_DRAWPLAYERINVENTORYNUMBERS);
}

// Unlock with any color orb
// Take orbs starting with the currently selector power bar color
// Input: nr of orbs to take
// Return: nr of orbs left to take
byte UnlockWithAnyOrb(byte costIn, byte *assignBackHere)
{

	// Needs orbs to unlock the chest.
	// Take the orbs from the CurrentColor first
	byte idx = PlayerInventory.CurrentColor;		// 0,1,2
	uint Orbs[3];
	byte cost;
	byte* writeTo = assignBackHere;

	cost = costIn;
	Orbs[0] = PlayerInventory.Red;
	Orbs[1] = PlayerInventory.Green;
	Orbs[2] = PlayerInventory.Blue;

	if (idx > 2) idx = 0;

	// We try and take one orb from each color
	// Starting with the current color
	for (ASM_DATA.WorkI = 0; cost && ASM_DATA.WorkI < (8*4); ++ASM_DATA.WorkI)
	{
		if (cost && Orbs[idx]) {
			// Take one
			--Orbs[idx];
			--cost;
			*writeTo = cost;//--ASM_DATA.InteractingWithThisObjPtr->Data.Chest.UnlockCost;

			// Store the values back into the inventory
			PlayerInventory.Red = Orbs[0];
			PlayerInventory.Green = Orbs[1];
			PlayerInventory.Blue = Orbs[2];

			DrawEverythingAndPlayTakeSound();
		}
		++idx;
		if (idx > 2) idx = 0;
	}

	// Put the info back into the inventory
	PlayerInventory.Red = Orbs[0];
	PlayerInventory.Green = Orbs[1];
	PlayerInventory.Blue = Orbs[2];

	return cost;
}

// Unlock the exit
// Return 255 when the exit is open, 0 otherwise
byte UnlockExit()
{
	byte cost0 = ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost0;
	byte cost1 = ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost1;

	if (cost0 || cost1)
	{
		// Check Type 0 for unlock options
		switch (ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockType0)
		{
			case EXIT_LOCKED_TYPE_POWER:
			{
				// Needs power to unlock the exit
				// PlayerInventory->Keys = how many keys are available
				for (ASM_DATA.WorkI = 0; cost0 && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
				{
					if (PlayerInventory.PowerLevel)
					{
						// Take power 
						--PlayerInventory.PowerLevel;
						--cost0;
						ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost0 = cost0;

						DrawEverythingAndPlayTakeSound();
					}
				}
				break;
			}

			case EXIT_LOCKED_TYPE_ORBS:
			{
				byte costAfter = UnlockWithAnyOrb(cost0, &ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost0);
				//ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost0 = costAfter;

				DrawMenu();
				break;
			}

			case EXIT_LOCKED_TYPE_KEYS:
			{
				// Needs keys to unlock the exit
				// PlayerInventory->Keys = how many keys are available
				for (ASM_DATA.WorkI = 0; cost0 && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
				{
					if (PlayerInventory.Keys)
					{
						// Take one key
						--PlayerInventory.Keys;
						--cost0;
						ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost0 = cost0;

						DrawEverythingAndPlayTakeSound();
					}
				}
				break;
			}

			case EXIT_LOCKED_TYPE_RED:
			{
				// Needs red orbs to unlock the exit
				for (ASM_DATA.WorkI = 0; cost0 && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
				{
					if (PlayerInventory.Red)
					{
						// Take one red orb
						--PlayerInventory.Red;
						--cost0;
						ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost0 = cost0;

						DrawEverythingAndPlayTakeSound();
					}
				}
				break;
			}

			case EXIT_LOCKED_TYPE_GREEN:
			{
				// Needs red orbs to unlock the exit
				for (ASM_DATA.WorkI = 0; cost0 && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
				{
					if (PlayerInventory.Green)
					{
						// Take one green orb
						--PlayerInventory.Green;
						--cost0;
						ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost0 = cost0;

						DrawEverythingAndPlayTakeSound();
					}
				}
				break;
			}

			case EXIT_LOCKED_TYPE_BLUE:
			{
				// Needs red orbs to unlock the exit
				for (ASM_DATA.WorkI = 0; cost0 && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
				{
					if (PlayerInventory.Blue)
					{
						// Take one blue orb
						--PlayerInventory.Blue;
						--cost0;
						ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost0 = cost0;

						DrawEverythingAndPlayTakeSound();
					}
				}
				break;
			}
		}
		// -------------------------------
		// Check Type 1 for unlock options
		switch (ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockType1)
		{
			case EXIT_LOCKED_TYPE_POWER:
			{
				// Needs power to unlock the exit
				// PlayerInventory->Keys = how many keys are available
				for (ASM_DATA.WorkI = 0; cost1 && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
				{
					if (PlayerInventory.PowerLevel)
					{
						// Take power 
						--PlayerInventory.PowerLevel;
						--cost1;
						ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost1 = cost1;

						DrawEverythingAndPlayTakeSound();
					}
				}
				break;
			}

			case EXIT_LOCKED_TYPE_ORBS:
			{
				byte costAfter = UnlockWithAnyOrb(cost1, &ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost1);
				//ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost1 = costAfter;

				DrawMenu();
				break;
			}

			case EXIT_LOCKED_TYPE_KEYS:
			{
				// Needs keys to unlock the exit
				// PlayerInventory->Keys = how many keys are available
				for (ASM_DATA.WorkI = 0; cost1 && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
				{
					if (PlayerInventory.Keys)
					{
						// Take one key
						--PlayerInventory.Keys;
						--cost1;
						ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost1 = cost1;

						DrawEverythingAndPlayTakeSound();
					}
				}
				break;
			}

			case EXIT_LOCKED_TYPE_RED:
			{
				// Needs red orbs to unlock the exit
				for (ASM_DATA.WorkI = 0; cost1 && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
				{
					if (PlayerInventory.Red)
					{
						// Take one red orb
						--PlayerInventory.Red;
						--cost1;
						ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost1 = cost1;

						DrawEverythingAndPlayTakeSound();
					}
				}
				break;
			}

			case EXIT_LOCKED_TYPE_GREEN:
			{
				// Needs red orbs to unlock the exit
				for (ASM_DATA.WorkI = 0; cost1 && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
				{
					if (PlayerInventory.Green)
					{
						// Take one green orb
						--PlayerInventory.Green;
						--cost1;
						ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost1 = cost1;

						DrawEverythingAndPlayTakeSound();
					}
				}
				break;
			}

			case EXIT_LOCKED_TYPE_BLUE:
			{
				// Needs red orbs to unlock the exit
				for (ASM_DATA.WorkI = 0; cost1 && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
				{
					if (PlayerInventory.Blue)
					{
						// Take one blue orb
						--PlayerInventory.Blue;
						--cost1;
						ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost1 = cost1;

						DrawEverythingAndPlayTakeSound();
					}
				}
				break;
			}
		}
		// Check if the EXIT is now open
		if (ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost0 == 0 &&
			ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost1 == 0)
		{
			// Yes, now open.
			Menu[MenuSelectionIndex] = &MMI_Exit;
			DrawMenu();
			POKE(SFX_CH1, SOUND_OPEN_CHEST);
		}
		else
		{
			// Not unlocked
			cost0 = (ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost0) + (ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost1);
			for (ASM_DATA.WorkI = 0; ASM_DATA.WorkI < cost0; ++ASM_DATA.WorkI)
			{
				POKE(SFX_CH1, SOUND_NO_ACTION);
				WaitFor30Cycles();
			}
		}
		return 0;
	}
	// We are open, so fire the exit command
	ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_EXIT;
	return 255;			// Return to 3D view
}

// The menu 'MenuSelectionIndex' item is a ghost that we want to attack.
// The player just clicked attack so reduce the cost until it is dead
void AttackGhost()
{
	byte cost = ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost;
	byte openChest = 0;
	
	switch (ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackType)
	{
		case ATTACK_TYPE_NONE:
			openChest = 1;
			break;
		case ATTACK_TYPE_POWER:
		{
			// Needs power to attack the ghost
			// PlayerInventory->Keys = how many keys are available
			for (ASM_DATA.WorkI = 0; cost && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
			{
				if (PlayerInventory.PowerLevel)
				{
					// Take power 
					--PlayerInventory.PowerLevel;
					--cost;
					ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost = cost;

					DrawEverythingAndPlayTakeSound();
				}
			}
		}

		case ATTACK_TYPE_ORBS:
		{
			UnlockWithAnyOrb(cost, &ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost);
			break;
		}

		case ATTACK_TYPE_RED:
		{
			// Needs red orbs to kill the ghost
			for (ASM_DATA.WorkI = 0; cost && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
			{
				if (PlayerInventory.Red)
				{
					// Take one red orb
					--PlayerInventory.Red;
					--cost;
					ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost = cost;

					DrawEverythingAndPlayTakeSound();
				}
			}
			break;
		}

		case ATTACK_TYPE_GREEN:
		{
			// Needs red orbs to unlock the exit
			for (ASM_DATA.WorkI = 0; cost && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
			{
				if (PlayerInventory.Green)
				{
					// Take one green orb
					--PlayerInventory.Green;
					--cost;
					ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost = cost;

					DrawEverythingAndPlayTakeSound();
				}
			}
			break;
		}

		case ATTACK_TYPE_BLUE:
		{
			// Needs red orbs to unlock the exit
			for (ASM_DATA.WorkI = 0; cost && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
			{
				if (PlayerInventory.Blue)
				{
					// Take one blue orb
					--PlayerInventory.Blue;
					--cost;
					ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost = cost;

					DrawEverythingAndPlayTakeSound();
				}
			}
			break;
		}
	}
	if (ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost == 0)
	{
		ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackType = ATTACK_TYPE_NONE;
		// Change the menu to the open view
		Menu[MenuSelectionIndex] = &MMI_GhostTake;
		openChest = 1;
	} 

	// Redraw the menu top gui and inventory
	DrawTopGui();
	DrawPlayerInventory();
	DrawMenu();
	if (openChest)
	{
		POKE(SFX_CH1, SOUND_OPEN_CHEST);
		
		// Change the attack type
		ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackType = ATTACK_TYPE_NONE;
		DrawSectionC();
		DrawOpenChestItems();
	}
}

// The menu 'MenuSelectionIndex' item is a chest that needs to be unlocked.
// The player just clicked unlock so reduce the cost until it is open
void UnlockChest()
{
	byte cost = ASM_DATA.InteractingWithThisObjPtr->Data.Chest.UnlockCost;
	byte openChest = 0;
	
	switch (ASM_DATA.InteractingWithThisObjPtr->Data.Chest.UnlockType)
	{
		case CHEST_LOCKED_TYPE_POWER:
		{
			// Needs power to unlock the chest
			for (ASM_DATA.WorkI = 0; cost && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
			{
				if (PlayerInventory.PowerLevel)
				{
					// Take power 
					--PlayerInventory.PowerLevel;
					--cost;
					ASM_DATA.InteractingWithThisObjPtr->Data.Chest.UnlockCost = cost;

					DrawEverythingAndPlayTakeSound();
				}
				else {
					// Not enough power
					POKE(SFX_CH1, SOUND_NO_ACTION);
					WaitFor30Cycles();
					break;
				}
			}
			break;
		}

		case CHEST_LOCKED_TYPE_ORBS:
		{
			UnlockWithAnyOrb(cost, &ASM_DATA.InteractingWithThisObjPtr->Data.Chest.UnlockCost);
			break;
		}

		case CHEST_LOCKED_TYPE_KEYS:
		{
			// Needs keys to unlock the chest.
			// PlayerInventory->Keys = how many keys are available
			// InteractingWithThisObjPtr->Data.Chest.UnlockCost = how much is needed to unlock
			byte cost = PlayerInventory.Keys;

			for (ASM_DATA.WorkI = 0; cost && ASM_DATA.WorkI < 8; ++ASM_DATA.WorkI)
			{
				if (PlayerInventory.Keys)
				{
					// Take one key
					--PlayerInventory.Keys;
					--cost;

					DrawEverythingAndPlayTakeSound();
				}
			}
			ASM_DATA.InteractingWithThisObjPtr->Data.Chest.UnlockCost = cost;
			break;
		}
	}

	if (ASM_DATA.InteractingWithThisObjPtr->Data.Chest.UnlockCost == 0)
	{
		// Change the menu to the open view
		Menu[MenuSelectionIndex] = &MMI_ChestTake;
		openChest = 1;
	} 

	// Redraw the menu top gui and inventory
	DrawTopGui();
	DrawPlayerInventory();
	DrawMenu();
	if (openChest)
	{
		POKE(SFX_CH1, SOUND_OPEN_CHEST);
		// Populate some data into the chest
		ASM_DATA.InteractingWithThisObjPtr->Data.Chest.UnlockType = CHEST_LOCKED_TYPE_NONE;
		DrawSectionC();
		DrawRandomSectionC();
		DrawOpenChestItems();
	}
}

// Do the selected menu action
// Return 255 to exit the map menu
byte DoMenuAction()
{
	switch (Menu[MenuSelectionIndex]->Type)
	{
		case MENU_ACTION_ATTACK_GHOST: 
		{
			// Used for the attack type
			AttackGhost();

			break;
		}

		case MENU_ACTION_TAKE_GHOST:
		{
			// This is a ghost and we are taking ALL items from it
			// This will make the ghost disappear.
			for (ASM_DATA.LoopI = 0; ASM_DATA.LoopI < 3; ++ASM_DATA.LoopI)
			{
				ASM_DATA.InteractingWithThisObjPtr->Type = OBJECT_NONE;
				// Add the item to the inventory
				ASM_DATA.ByteParam0 = ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeNum[ASM_DATA.LoopI];
				ASM_DATA.ByteParam1 = ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeType[ASM_DATA.LoopI] & 7;

				if (ASM_DATA.ByteParam0 && ASM_DATA.ByteParam1) 
				{
					asm("jsr %w", FIXED_DISPATCHADDTOINVENTORY);

					ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeType[ASM_DATA.LoopI] = CHEST_ITEM_TYPE_NONE;
					ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeNum[ASM_DATA.LoopI] = 0;
					
					asm("jsr %w", FIXED_DRAWPLAYERINVENTORYNUMBERS);

					POKE(SFX_CH1, SOUND_CHEST_TAKE);

					asm("ldx %w", offsetof(_asm_data, LoopI));
					asm("lda #0");
					asm("tay");
					asm("jsr %w", FIXED_DRAWSECTIONCITEM);

					WaitFor30Cycles();
					//for (ASM_DATA.LoopJ = 30; ASM_DATA.LoopJ; --ASM_DATA.LoopJ) {
					//	WaitForVbi();
					//}


				}
			}
			ASM_DATA.WorkVal = TheMaze[ASM_DATA.PlayerPosition];
			ASM_DATA.WorkVal &= (255-OBJECT);
			TheMaze[ASM_DATA.PlayerPosition] = ASM_DATA.WorkVal | OBJECT_DONE;
			--ASM_DATA.GhostsLeft;
			ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_NONE;
			POKE(SFX_CH1, SOUND_GHOST_KILL);
			return 255;
		}

		case MENU_ACTION_UNLOCK_CHEST:
		{
			UnlockChest();
			return 0;
		}
		case MENU_ACTION_TAKE_CHEST:
		{
			// This is a chest.
			// Take the items in the chest
			ASM_DATA.InteractingWithThisObjPtr->Type = OBJECT_NONE;
			ASM_DATA.WorkVal = TheMaze[ASM_DATA.PlayerPosition];
			ASM_DATA.WorkVal = (ASM_DATA.WorkVal & ~OBJECT) | OBJECT_DONE;
			TheMaze[ASM_DATA.PlayerPosition] = ASM_DATA.WorkVal;

			for (ASM_DATA.LoopI = 0; ASM_DATA.LoopI < 3; ++ASM_DATA.LoopI)
			{
				// Add the item to the inventory
				ASM_DATA.ByteParam0 = ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeNum[ASM_DATA.LoopI];
				ASM_DATA.ByteParam1 = ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeType[ASM_DATA.LoopI] & 7;

				if (ASM_DATA.ByteParam0 && ASM_DATA.ByteParam1) 
				{
					asm("jsr %w", FIXED_DISPATCHADDTOINVENTORY);

					ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeType[ASM_DATA.LoopI] = CHEST_ITEM_TYPE_NONE;
					ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeNum[ASM_DATA.LoopI] = 0;
					
					asm("jsr %w", FIXED_DRAWPLAYERINVENTORYNUMBERS);

					POKE(SFX_CH1, SOUND_CHEST_TAKE);

					asm("ldx %w", offsetof(_asm_data, LoopI));
					asm("lda #0");
					asm("tay");
					asm("jsr %w", FIXED_DRAWSECTIONCITEM);

					for (ASM_DATA.LoopJ = 30; ASM_DATA.LoopJ; --ASM_DATA.LoopJ) {
						WaitForVbi();
					}

					TheMaze[ASM_DATA.PlayerPosition] |= OBJECT_DONE;
				}
			}
			return 0;
		}

		case MENU_ACTION_VORTEX:
		{
			// Want to do a vortex jump.  Do we have enough power?
			byte cost = ASM_DATA.InteractingWithThisObjPtr->Data.Vortex.UnlockCost;
			// Check that we have enough power to fire the vortex
			if (PlayerInventory.PowerLevel < cost)
			{
				// Not enough power
				POKE(SFX_CH1, SOUND_NOT_ENOUGH_POWER);
				POKE(SFX_CH2, SOUND_NOT_ENOUGH_POWER);
				return 0;
			}
			// Fire the vortex
			PlayerInventory.PowerLevel -= cost;	

			ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_VORTEX;
			return 255;			// Return to 3D view
		}

		case MENU_ACTION_SWITCH:
		{
			byte cost = ASM_DATA.InteractingWithThisObjPtr->Data.Vortex.UnlockCost;
			// Check that we have enough power to fire the vortex
			if (PlayerInventory.PowerLevel < cost)
			{
				// Not enough power
				POKE(SFX_CH1, SOUND_NOT_ENOUGH_POWER);
				POKE(SFX_CH2, SOUND_NOT_ENOUGH_POWER);
				return 0;
			}
			PlayerInventory.PowerLevel -= cost;	

			ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_SWITCH;
			return 255;
		}

		case MENU_ACTION_LEVEL_EXIT:
		{
			// Fire the exit
			return UnlockExit();
			ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_EXIT;
			return 255;			// Return to 3D view
		}

		case MENU_ACTION_OPEN_POUCH:
			return PouchInteraction();

		case MENU_ACTION_TRADE_GHOST:
			return TradeInteraction();
	}
	POKE(SFX_CH2, SOUND_MENU_OPEN_ACTIONS);
	return 0;
}

// ========================================================
// Do the map gui interactions
// Figure out the text options
// NumMenuItems - number of items in the menu
// MenuWindow is always 5 items big, it can move over the Menu
// SFX_CH1 = up down menu option
void DoMapInteraction()
{
	byte waitingForActionRelease = 1;

	ASM_DATA.NextActionCommand = 0;

	// Reset the menu to the top and the first action (always BACK)
	WindowTop = 0;
	WindowStep = 0;
	MenuSelectionIndex = 0;

	DrawMenu();
	DrawPlayerInventory();
	ShowMapScreen();

	while (1)
	{
		wait();

		if (waitingForActionRelease && (ASM_DATA.Joystick0 & JOYSTICK_FIRE) == 0)
		{
			// Fire button was released.
			// Either on the Back or another action.
			// Fire the action
			if (MenuSelectionIndex == 0)
			{
				// Released on the back ==> Exit the menu
				POKE(SFX_CH2, SOUND_MENU_EXIT);
				POKE(FIXED_CH, 255);
				return;
			}
			else 
			{
				// Released on an action
				waitingForActionRelease = 0;
				if (DoMenuAction() == 255)
					return;			// Exit the map menu of 255
				switch (ASM_DATA.NextActionCommand)
				{
					case AFTER_MENU_COMMANDS_MENU_UP:
						StepUp();
						POKE(SFX_CH1, SOUND_MENU_CHANGE);
						break;
				
					case AFTER_MENU_COMMANDS_MENU_DOWN:
						StepDown();
						POKE(SFX_CH1, SOUND_MENU_CHANGE);
						break;

					case AFTER_MENU_COMMANDS_SHOW_MAP:
					{
						POKE(SFX_CH1, SOUND_MENU_REVEAL);
						// Quickly and compact set the VISITED bit on the whole maze
						asm("ldy #255");
						asm("revealLoop: lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
						asm("ora #%w", VISITED);									// Maze[WorkPosition] |= VISITED
						asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 
						asm("dey");
						asm("bne revealLoop");

						DrawMapSmall();

						--PlayerInventory.Maps;
						DrawMenu();
						DrawPlayerInventory();
						break;
					}
				}
				ASM_DATA.NextActionCommand = 0;
			}
		}
		if (ASM_DATA.Joystick0)
		{
			if (ASM_DATA.Joystick0 != ASM_DATA.PrevJoystick0)
			{
				// Some joystick input is being applied
				ASM_DATA.PrevJoystick0 = ASM_DATA.Joystick0;

				if (ASM_DATA.Joystick0 & JOYSTICK_UP)
				{
					StepUp();
					POKE(SFX_CH1, SOUND_MENU_CHANGE);
				}
				else if (ASM_DATA.Joystick0 & JOYSTICK_DOWN)
				{
					StepDown();
					POKE(SFX_CH1, SOUND_MENU_CHANGE);
				}

				if (waitingForActionRelease == 0 && (ASM_DATA.Joystick0 & JOYSTICK_FIRE) )
				{
					// Fire was pressed
					waitingForActionRelease = 1;
				}
			}
		}
		else 
		{
			ASM_DATA.PrevJoystick0 = ASM_DATA.Joystick0;
		}
	}
}

// Draw the small map, showing the visited tiles
// Build the initial map interaction gui
void DoMapScreen(void)
{
	memset(FIXED_MAPSCREEN, 0, (40*21));
	DrawMapSmall();					// Draw the map onto the screen
	BuildMapGui();					// Setup the map's GUI (menu and interactions)
	DoMapInteraction();
}

void RevealFullMap(void)
{
	// When you give up on a level you don't get to hear the awesome level end music :)
	if (ASM_DATA.NextActionCommand != AFTER_MENU_COMMANDS_GIVE_UP)
		asm("jsr %w", FIXED_PLAYLEVELENDMUSIC);

	// Quickly and compact set the VISITED bit on the whole maze
	asm("ldy #255");
	asm("revealLoop: lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
	asm("ora #%w", VISITED);									// Maze[WorkPosition] |= VISITED
	asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 
	asm("dey");
	asm("bne revealLoop");

	memset(FIXED_MAPSCREEN, 0, (40*21));
	DrawMapSmall();					// Draw the map onto the screen
}

// ========================================================
// Open the pouch and draw the contents into section C
byte PouchInteraction(void)
{
	byte waitingForActionRelease = 0;

	POKE(SFX_CH2, SOUND_MENU_OPEN_ACTIONS);

	WaitForFireRelease();

	#define PouchMenuCount  FIXED_POUCHMENUCOUNT
	#define PouchMenuActive FIXED_POUCHMENUACTIVE
	DrawSectionC();

	POKE(FIXED_POUCHMENUCOUNT, PlayerInventory.Bombs);
	POKE(FIXED_POUCHMENUCOUNT+1, PlayerInventory.Maps);
	POKE(FIXED_POUCHMENUCOUNT+2, PlayerInventory.Vortex);

	POKE(PouchMenuActive, 1);

	DrawPouchMenuFromState();

	while (1)
	{
		wait();

		if (waitingForActionRelease && (ASM_DATA.Joystick0 & JOYSTICK_FIRE) == 0)
		{
			// Fire button was released.
			// Either on the Back or another action.
			// Fire the action
			byte count = PEEK(PouchMenuCount + PEEK(PouchMenuActive));
			if (count == 0)
			{
				POKE(SFX_CH2, SOUND_NO_ACTION);
				WaitForFireRelease();
				waitingForActionRelease = 0;
			}
			else {
				// Lets do an action
				switch(PEEK(PouchMenuActive))
				{
					case 0: ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_PLACE_BOMB;
						return 255;

					case 1: ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_SHOW_MAP;
						return 0;

					case 2: ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_VORTEX_TO_END;
						return 255;
				}
			}
		}
		if (ASM_DATA.Joystick0)
		{
			if (ASM_DATA.Joystick0 != ASM_DATA.PrevJoystick0)
			{
				// Some joystick input is being applied
				ASM_DATA.PrevJoystick0 = ASM_DATA.Joystick0;

				if (ASM_DATA.Joystick0 & JOYSTICK_LEFT)
				{
					if (PEEK(PouchMenuActive) == 0)
						POKE(PouchMenuActive, 2);
					else
						POKE(PouchMenuActive, PEEK(PouchMenuActive)-1);
					POKE(SFX_CH1, SOUND_MENU_CHANGE);
				}
				else if (ASM_DATA.Joystick0 & JOYSTICK_RIGHT)
				{
					if (PEEK(PouchMenuActive) == 2)
						POKE(PouchMenuActive, 0);
					else
						POKE(PouchMenuActive, PEEK(PouchMenuActive)+1);
					POKE(SFX_CH1, SOUND_MENU_CHANGE);
				}
				if (ASM_DATA.Joystick0 & JOYSTICK_UP)
				{
					ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_MENU_UP;
					return 0;
				}
				else if (ASM_DATA.Joystick0 & JOYSTICK_DOWN)
				{
					ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_MENU_DOWN;
					return 0;
				}
				DrawPouchMenuFromState();

				if (waitingForActionRelease == 0 && (ASM_DATA.Joystick0 & JOYSTICK_FIRE) )
				{
					// Fire was pressed
					waitingForActionRelease = 1;
				}
			}
		}
		else 
		{
			ASM_DATA.PrevJoystick0 = ASM_DATA.Joystick0;
		}
	}


	return 0;

}



void BuildTradingOptions()
{
	NumTradingOptions = 0;
	CurrentTradingOption = 0;

	// Some ghosts give specific trading options
	switch (ASM_DATA.CurrentObject)
	{
		case OBJECT_GHOST_BOB:
			{
				// 2x Red -> 1x Green
				if (PlayerInventory.Red >= 2) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_RED;
					TradingTypeGiveCount[NumTradingOptions] = 2;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_GREEN;
					TradingTypeGetCount[NumTradingOptions] = 1;
					++NumTradingOptions;
				}
				// 1x Blue -> 4x Red
				if (PlayerInventory.Blue >= 1) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_BLUE;
					TradingTypeGiveCount[NumTradingOptions] = 1;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_RED;
					TradingTypeGetCount[NumTradingOptions] = 4;
					++NumTradingOptions;
				}
				// 9x Red -> 1x Key
				if (PlayerInventory.Red >= 9) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_RED;
					TradingTypeGiveCount[NumTradingOptions] = 9;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_KEY;
					TradingTypeGetCount[NumTradingOptions] = 1;
					++NumTradingOptions;
				}
				break;
			}
		case OBJECT_GHOST_CYCO:
			{
				// 1x Green -> 3x Red
				if (PlayerInventory.Green >= 1) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_GREEN;
					TradingTypeGiveCount[NumTradingOptions] = 1;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_RED;
					TradingTypeGetCount[NumTradingOptions] = 3;
					++NumTradingOptions;
				}
				// 4x Green -> 1x Key
				if (PlayerInventory.Green >= 4) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_GREEN;
					TradingTypeGiveCount[NumTradingOptions] = 4;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_KEY;
					TradingTypeGetCount[NumTradingOptions] = 1;
					++NumTradingOptions;
				}
				// 2x Blue -> 9x Green
				if (PlayerInventory.Blue >= 2) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_BLUE;
					TradingTypeGiveCount[NumTradingOptions] = 2;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_GREEN;
					TradingTypeGetCount[NumTradingOptions] = 9;
					++NumTradingOptions;
				}
				break;
			}
		case OBJECT_GHOST_ALICE:
			{
				// 4x Red -> 1x Blue
				if (PlayerInventory.Red >= 4) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_RED;
					TradingTypeGiveCount[NumTradingOptions] = 4;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_BLUE;
					TradingTypeGetCount[NumTradingOptions] = 1;
					++NumTradingOptions;
				}
				// 1x Blue -> 3x Green
				if (PlayerInventory.Blue >= 1) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_BLUE;
					TradingTypeGiveCount[NumTradingOptions] = 1;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_GREEN;
					TradingTypeGetCount[NumTradingOptions] = 3;
					++NumTradingOptions;
				}
				// 2x Blue -> 1x Key
				if (PlayerInventory.Blue >= 2) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_BLUE;
					TradingTypeGiveCount[NumTradingOptions] = 2;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_KEY;
					TradingTypeGetCount[NumTradingOptions] = 1;
					++NumTradingOptions;
				}
				break;
			}
		case OBJECT_GHOST_SPECULA:
			{
				// 1x Key -> 3x Blue
				if (PlayerInventory.Keys >= 1) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_KEY;
					TradingTypeGiveCount[NumTradingOptions] = 1;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_BLUE;
					TradingTypeGetCount[NumTradingOptions] = 3;
					++NumTradingOptions;
				}
				// 1x Blue -> 3x Green
				if (PlayerInventory.Blue >= 1) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_BLUE;
					TradingTypeGiveCount[NumTradingOptions] = 1;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_GREEN;
					TradingTypeGetCount[NumTradingOptions] = 3;
					++NumTradingOptions;
				}
				// 1x Green -> 1x Red
				if (PlayerInventory.Green >= 1) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_GREEN;
					TradingTypeGiveCount[NumTradingOptions] = 1;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_RED;
					TradingTypeGetCount[NumTradingOptions] = 1;
					++NumTradingOptions;
				}
				// 6x Green -> 8x Random
				if (PlayerInventory.Green >= 6) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_GREEN;
					TradingTypeGiveCount[NumTradingOptions] = 6;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_RANDOM;
					TradingTypeGetCount[NumTradingOptions] = 8;
					++NumTradingOptions;
				}
				break;
			}
		case OBJECT_GHOST_PANDA:
			{
				// 1x Key -> 3x Blue
				if (PlayerInventory.Keys >= 1) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_KEY;
					TradingTypeGiveCount[NumTradingOptions] = 1;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_BLUE;
					TradingTypeGetCount[NumTradingOptions] = 3;
					++NumTradingOptions;
				}
				// 1x Blue -> 3x Green
				if (PlayerInventory.Blue >= 1) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_BLUE;
					TradingTypeGiveCount[NumTradingOptions] = 1;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_GREEN;
					TradingTypeGetCount[NumTradingOptions] = 3;
					++NumTradingOptions;
				}
				// 2x Green -> 6x Red
				if (PlayerInventory.Green >= 2) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_GREEN;
					TradingTypeGiveCount[NumTradingOptions] = 2;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_RED;
					TradingTypeGetCount[NumTradingOptions] = 6;
					++NumTradingOptions;
				}
				// 5x Blue -> 9x Random
				if (PlayerInventory.Blue >= 5) {
					TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_BLUE;
					TradingTypeGiveCount[NumTradingOptions] = 5;
					TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_RANDOM;
					TradingTypeGetCount[NumTradingOptions] = 9;
					++NumTradingOptions;
				}
				break;
			}
	}
	// 1x Green -> Full power bar
	if (PlayerInventory.Green >= 1) {
		TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_GREEN;
		TradingTypeGiveCount[NumTradingOptions] = 1;
		TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_POWER_BAR;
		TradingTypeGetCount[NumTradingOptions] = 1;
		++NumTradingOptions;
	}
	// 1x Blue -> Full power bar
	if (PlayerInventory.Blue >= 1) {
		TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_BLUE;
		TradingTypeGiveCount[NumTradingOptions] = 1;
		TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_POWER_BAR;
		TradingTypeGetCount[NumTradingOptions] = 1;
		++NumTradingOptions;
	}
	// 1x Red -> Full power bar
	if (PlayerInventory.Red >= 1) {
		TradingTypeGiveType[NumTradingOptions] = SECTION_C_ITEM_TYPE_ORB_RED;
		TradingTypeGiveCount[NumTradingOptions] = 1;
		TradingTypeGetType[NumTradingOptions] = SECTION_C_ITEM_TYPE_POWER_BAR;
		TradingTypeGetCount[NumTradingOptions] = 1;
		++NumTradingOptions;
	}
}

void FillTradingState()
{
	if (NumTradingOptions == 0)
	{
		POKE(FIXED_TRADEMENUTYPE, 0);
		POKE(FIXED_TRADEMENUCOUNT, 0);
		POKE((FIXED_TRADEMENUTYPE+1), 0);
		POKE((FIXED_TRADEMENUCOUNT+1), 0);
		return;
	}

	POKE(FIXED_TRADEMENUTYPE, TradingTypeGiveType[CurrentTradingOption]);
	POKE(FIXED_TRADEMENUCOUNT, TradingTypeGiveCount[CurrentTradingOption]);
	POKE((FIXED_TRADEMENUTYPE+1), TradingTypeGetType[CurrentTradingOption]);
	POKE((FIXED_TRADEMENUCOUNT+1), TradingTypeGetCount[CurrentTradingOption]);
}

void ActionTrade()
{
	ASM_DATA.WorkI = TradingTypeGiveCount[CurrentTradingOption];
	// Take away
	switch (TradingTypeGiveType[CurrentTradingOption])
	{
		case SECTION_C_ITEM_TYPE_ORB_RED:
			PlayerInventory.Red -= ASM_DATA.WorkI;
			break;
		case SECTION_C_ITEM_TYPE_ORB_GREEN:
			PlayerInventory.Green -= ASM_DATA.WorkI;
			break;
		case SECTION_C_ITEM_TYPE_ORB_BLUE:
			PlayerInventory.Blue -= ASM_DATA.WorkI;
			break;
		case SECTION_C_ITEM_TYPE_KEY:
			PlayerInventory.Keys -= ASM_DATA.WorkI;
			break;
	}

	// Give
	ASM_DATA.WorkI = TradingTypeGetCount[CurrentTradingOption];
	switch (TradingTypeGetType[CurrentTradingOption])
	{
		case SECTION_C_ITEM_TYPE_ORB_RED:
			PlayerInventory.Red += ASM_DATA.WorkI;
			break;
		case SECTION_C_ITEM_TYPE_ORB_GREEN:
			PlayerInventory.Green += ASM_DATA.WorkI;
			break;
		case SECTION_C_ITEM_TYPE_ORB_BLUE:
			PlayerInventory.Blue += ASM_DATA.WorkI;
			break;
		case SECTION_C_ITEM_TYPE_KEY:
			PlayerInventory.Keys += ASM_DATA.WorkI;
			break;
		case SECTION_C_ITEM_TYPE_POWER_BAR:
			PlayerInventory.PowerLevel = 32;
			break;
		case SECTION_C_ITEM_TYPE_RANDOM:
			{
				// Select a random target
				ASM_DATA.WorkVal = rand() & 3;

				switch (ASM_DATA.WorkVal)
				{
					case 0: PlayerInventory.Red += ASM_DATA.WorkI; break;
					case 1: PlayerInventory.Green += ASM_DATA.WorkI; break;
					case 2: PlayerInventory.Blue += ASM_DATA.WorkI; break;
					case 3: PlayerInventory.Keys += ASM_DATA.WorkI; break;
				}
			}
			break;
	}
	// Limit the numbers
	if (PlayerInventory.Red > 240) PlayerInventory.Red = 240;
	if (PlayerInventory.Green > 240) PlayerInventory.Green = 240;
	if (PlayerInventory.Blue > 240) PlayerInventory.Blue = 240;
	if (PlayerInventory.Keys > 240) PlayerInventory.Keys = 240;
}

// ========================================================
// Open the ghost trading menu and draw the contents into section C
byte TradeInteraction(void)
{
	byte waitingForActionRelease = 0;
	byte activeTradeNr = 0;

	POKE(SFX_CH2, SOUND_MENU_OPEN_ACTIONS);

	// Fill the trading options
	// Red -> Green
	BuildTradingOptions();

	WaitForFireRelease();

	#define TradeMenuType	FIXED_TRACEMENUTYPE
	#define TradeMenuCount	FIXED_TRACEMENUCOUNT
	DrawSectionC();

	FillTradingState();

	DrawTradeMenuFromState();

	while (1)
	{
		wait();

		if (waitingForActionRelease && (ASM_DATA.Joystick0 & JOYSTICK_FIRE) == 0)
		{
			// Fire button was released.
			// Either on the Back or another action.
			// Fire the action
			// Lets do an action
			ActionTrade();
			DrawPlayerInventory();
			DrawThePowerBar();
			DrawSectionC();
			POKE(SFX_CH1, SOUND_USE_AN_ORB);
			BuildTradingOptions();
			FillTradingState();
			DrawTradeMenuFromState();
			waitingForActionRelease = 0;
		}
		if (ASM_DATA.Joystick0)
		{
			if (ASM_DATA.Joystick0 != ASM_DATA.PrevJoystick0)
			{
				// Some joystick input is being applied
				ASM_DATA.PrevJoystick0 = ASM_DATA.Joystick0;

				if (ASM_DATA.Joystick0 & JOYSTICK_LEFT)
				{
					if (CurrentTradingOption == 0)
						CurrentTradingOption = (byte)(NumTradingOptions - 1);
					else
						--CurrentTradingOption;
					POKE(SFX_CH1, SOUND_MENU_CHANGE);
				}
				else if (ASM_DATA.Joystick0 & JOYSTICK_RIGHT)
				{
					++CurrentTradingOption;
					if (CurrentTradingOption == NumTradingOptions)
					{
						CurrentTradingOption = 0;
					}
					POKE(SFX_CH1, SOUND_MENU_CHANGE);
				}
				if (ASM_DATA.Joystick0 & JOYSTICK_UP)
				{
					ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_MENU_UP;
					return 0;
				}
				else if (ASM_DATA.Joystick0 & JOYSTICK_DOWN)
				{
					ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_MENU_DOWN;
					return 0;
				}
				FillTradingState();
				DrawTradeMenuFromState();

				if (waitingForActionRelease == 0 && (ASM_DATA.Joystick0 & JOYSTICK_FIRE) )
				{
					// Fire was pressed
					waitingForActionRelease = 1;
				}
			}
		}
		else 
		{
			ASM_DATA.PrevJoystick0 = ASM_DATA.Joystick0;
		}
	}

	return 0;
}