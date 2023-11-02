#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "fixed.h"
#include"constants.h"

extern byte ActionDistance;			// How far is the next action object away from the player?
extern byte CurrentAction;
extern byte ExitPosition;

extern byte ObjectLocation[MAX_OBJECT_COUNT+1];		// Where are the objects located (pos = x+y*width)

extern byte MultByWidth[MAX_HEIGHT];
extern short MultScreen[21];

#define ZeroMyMemory() (asm("jsr %w", FIXED_ZEROMYMEMORY))

#define WaitFor60Cycles() (asm("jsr %w", FIXED_WAITFOR60CYCLES))
#define WaitFor30Cycles() (asm("jsr %w", FIXED_WAITFOR30CYCLES))
#define WaitFor30CyclesNoShortcut() (asm("jsr %w", FIXED_WAITFOR30CYCLESNOSHORTCUT))

#define DrawThePowerBar() (asm("jsr %w", FIXED_DRAWTHEPOWERBAR))


#define RemoveAtariBasic() (asm("jsr %w", FIXED_REMOVEATARIBASIC))
#define ResetScore() (asm("jsr %w", FIXED_RESETSCORE))
#define DrawScore() (asm("jsr %w", FIXED_DRAWSCORE))
#define Add10ToScore() (asm("jsr %w", FIXED_ADD10TOSCORE))
#define SetupAsm() (asm("jsr %w", FIXED_SETUPASM))


#define ShowMapScreen() (asm("jsr %w", FIXED_SHOWMAPSCREEN))
#define ShowGameScreen0() (asm("jsr %w", FIXED_SHOWGAMESCREEN0))
#define ShowGameScreen1() (asm("jsr %w", FIXED_SHOWGAMESCREEN1))
#define WaitForVbi() (asm("jsr %w", FIXED_WAITFORVBI))
#define ReadJoystick() (asm("jsr %w", FIXED_READJOYSTICK))
#define WaitForFireRelease() (asm("jsr %w", FIXED_WAITFORFIRERELEASE))
#define DrawTopGui() (asm("jsr %w", FIXED_DRAWTOPGUI))
#define DrawLevelNr() (asm("jsr %w", FIXED_DRAWLEVELNR))

#define RenderLevelDone() (asm("jsr %w", FIXED_RENDERLEVELDONE))
#define RenderTheEnd() (asm("jsr %w", FIXED_RENDERTHEEND))

#define DrawSectionC() (asm("jsr %w", FIXED_DRAWSECTIONC))
#define DrawRandomSectionC() (asm("jsr %w", FIXED_DRAWRANDOMSECTIONC))
#define ClearSectionC() (asm("jsr %w", FIXED_CLEARSECTIONC))
#define DrawSectionCItems() (asm("jsr %w", FIXED_DRAWSECTIONCITEMS))
#define DrawPouchMenuFromState() (asm("jsr %w", FIXED_DRAWPOUCHMENUFROMSTATE));
#define DrawTradeMenuFromState() (asm("jsr %w", FIXED_DRAWTRADEMENUFROMSTATE));

#define AddToOrbsRed() (asm("jsr %w", FIXED_ADDTOORBSRED))
#define AddToOrbsGreen() (asm("jsr %w", FIXED_ADDTOORBSGREEN))
#define AddToOrbsBlue() (asm("jsr %w", FIXED_ADDTOORBSBLUE))
#define AddToOrbsKeys() (asm("jsr %w", FIXED_ADDTOKEYS))

#define ShowGameScreen() (asm("jsr %w", FIXED_SHOWTHEGAMESCREEN))

// Various objects can have an inventory.
// Ghost - hold items for trading and for when destroyed
// Chest - what items are needed for unlock, hold items for pickup
// Lever - what items are needed for action,
// Warp - what items are needed for action
// Pouch - what items are in stock
// Make sure that the ASM offsets for these fields match
typedef struct Inventory {
	uint Keys;				// (2) How many keys 
	uint Red;				// (2) How many red orbs
	uint Green;				// (2) How many green orbs
	uint Blue;				// (2) How many blue orbs
	byte PowerLevel;		// (1) How much power (0-31), 32 => becomes an orb and level goes to 0
	byte Bombs;				// (1) How many bombs in stock
	byte Maps;				// (1) How many maps in stock
	byte Vortex;			// (1) How many vortex jumps in stock
	byte CurrentColor;		// (1) 0 = Red, 1 = Green, 2 = Blue
	byte dummy[3];			// (4) Empty
} Inventory;

extern Inventory PlayerInventory;

typedef struct GhostData
{
	byte AttackType;			// What needs to be spent to kill the ghost 0=Nothing, 1=key, 2=red, 3=green, 4=blue, 5=power
	byte AttackCost;			// How much to spend to kill the ghost
	byte TakeType[3];			// What type of items can be picked up
	byte TakeNum[3];			// How many
} GhostData;

typedef struct ChestData
{
	byte UnlockType;			// What needs to be spent to open the chest.
	byte UnlockCost;			// How much to spend to unlock the chest
	byte TakeType[3];			// What type of items can be picked up
	byte TakeNum[3];			// How many
} ChestData;

typedef struct VortexData
{
	byte UnlockType;			// What needs to be spent to open the vortex
	byte UnlockCost;			// How much to spend to unlock the vortex
} VortexData;

typedef struct SwitchData
{
	byte UnlockType;			// What needs to be spent to use the switch
	byte UnlockCost;			// How much to spend to unlock the switch
} SwitchData;

typedef struct ExitData
{
	byte UnlockType0;			// What needs to be spent to use the exit
	byte UnlockCost0;			// How much to spend to unlock the exit
	byte UnlockType1;			// What needs to be spent to use the exit
	byte UnlockCost1;			// How much to spend to unlock the exit

} ExitData;

typedef struct InteractionObject {
	byte Location;			// (1) Where in the maze is the object located?
	byte Type;				// (1) What type of object is this: 0=nothing, 1-5=ghost, 6=chest, 7=vortex, 8=switch, 9=switch, 10=exit
	// Each type has its data declared here
	union
	{
		GhostData Ghost;
		ChestData Chest;		// Define unlock type, cost and 3x contents
		VortexData Vortex;		// Define unlock type and cost
		SwitchData Switch;		// Define unlock type and cost
		ExitData Exit;			// Define 2x unlock type and cost
	} Data;
} InteractionObject;
extern InteractionObject* MazeObjects;



typedef struct LevelDefinition {
	byte Width;
	byte Height;
	byte Seed;				// This is increased every time we finish the level
	byte NumChest;			// How many chests
	byte ChestUnlockType;	// 0,1 - first lock type, 2,3 = 2nd lock type, 4,5 = 3rd lock type, 7 = just cycle
	byte ExitUnlockType0;	// 0-3 - first lock type, 4-7 = 2nd lock type
	byte ExitUnlockCost0;	
	byte ExitUnlockType1;	// 0-3 - first lock type, 4-7 = 2nd lock type
	byte ExitUnlockCost1;	
	byte dummy[7];
} LevelDefinition;

extern LevelDefinition Levels[NUM_LEVELS+1];			// Index by ASM_DATA.LevelNr  +1 for the demo level

#endif

