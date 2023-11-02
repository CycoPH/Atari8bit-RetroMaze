#ifndef _C_ASM_MEMORY_H_
#define _C_ASM_MEMORY_H_

#include "globals.h"

#define offsetof(type, member)  (size_t) (&((type*) 0)->member)
#define offsetof2(a,b) (size_t) &( ((a*)0) -> b)
#define array_offsetof(type, member, index) (offsetof(type, member) + sizeof( ((type *)0)->member[0]) * index)
#define access(member) (size_t) (&((_asm_data*) 0)->member)
// C <--> ASM interface
// Page Zero memory allocation
struct _asm_data {
	// ---- Zero Page ----
	unsigned char _Skip1[0x15];				// $00-$14 Don't touch

	unsigned char PlayerX;					// $15
	unsigned char PlayerY;					// $16
	unsigned char LeftView[4];				// $17-$1A
	unsigned char RightView[4];				// $1B-$1E
	unsigned char MidView[4];				// $1F-$22
	unsigned char PlayerDirection;			// $23	0,1,2,3 = E S W N
	unsigned char Difficulty;				// $24		How much per power pill
	unsigned char JoystickDirection;		// $25 
	unsigned char* ScreenPtr;				// $26,$27	Draw to screen ptr (used to actively)
	unsigned char* PagePtr;					// $28,$29	The location in memory where the current page is (we will draw to this page)
	unsigned char ActionPosition;			// $2A		The position where the next action will take place = maze[pos]
	unsigned char NextActionCommand;		// $2B		What is the next action to perform: Exit level, vortex etc 0 = nothing
	unsigned char PageNr;					// $2C		Which page are we showing? 0 = screen 0, 1 = screen 1, 2 = map
	unsigned char LevelNr;					// $2D		Level Nr
	unsigned char LevelWidth;				// $2E		Width
	unsigned char LevelHeight;				// $2F		Height
	unsigned char PlayerPosition;			// $30		PlayerPosition Where in the maze the player is located (x + y * width)
	unsigned char* MazePtr;					// $31,$32	The location in memory where the maze is created (256 bytes)
	unsigned char WorkPosition;				// $33		temp player position used to evaluate what can be seen
	unsigned char WorkTile;					// $34		What is the MazePtr[WorkPosition] value
	unsigned char CurrentAction;			// $35		What action is to be fired for the current tile
	unsigned char ObjectCount;				// $36		How many objects are there in the level (max 25)
	unsigned char PowerPillsLeft;			// $37		How many power pill are there left to pickup in this level
	unsigned char WaitCounter;				// $38		Only every used to count VBI waits
	unsigned char GhostsLeft;				// $39		How many ghosts are left in the level
	unsigned char LastGhostsLeft;			// $3A		How many ghosts left on the last render
	unsigned char SectionCInverse;			// $3B		When 128 then the currently drawn section C item will be done in inverse
	unsigned char MenuTemp0;				// $3C		Temp var used in menu drawing
	unsigned char MenuTemp1;				// $3D		Temp var used in menu drawing
	unsigned char CurrentObject;			// $3E	0 is interact with nothing (which object type is being interacted with)
	InteractionObject* InteractingWithThisObjPtr;// ($3F,$40) ptr to the object currently being interacted with
	unsigned char soundr;					// = $41	NOISY I/0 FLAG. (ZERO IS QUIET)
	unsigned char critic;					// = $42	DEFINES CRITICAL SECTION (CRITICAL IF NON-Z)
	unsigned char WallDistance;				// = $43	How far is the wall away from the player 1,2,3,4
	unsigned char ActionDistance;			// = $44	The distance to the next object
	unsigned char SectionCBox0Type;			// = $45	What to draw into box 0 (0-7)
	unsigned char SectionCBox1Type;			// = $46	What to draw into box 1 (0-7)
	unsigned char SectionCBox2Type;			// = $47	What to draw into box 2 (0-7)
	unsigned char SectionCBox0Count;		// = $48	How many items are there? (0-99)
	unsigned char SectionCBox1Count;		// = $49	How many items are there? (0-99)
	unsigned char SectionCBox2Count;		// = $4A	How many items are there? (0-99)
	LevelDefinition* WorkLevelPtr;			// $4B,$4C	(ptr) to the current level data
 	unsigned char atract;					// = $4D	ATRACT FLAG
	unsigned char drkmsk;					// = $4E	DARK ATRACT MASK
    unsigned char colrsh;					// = $4F	ATRACT COLOR SHIFTER (EOR'ED WITH PLAYFIELD
	unsigned char WorkX;					// $50		Any temp X location
	unsigned char WorkY;					// $51		Any temp Y location
	unsigned char WorkI;					// $52		Any temp I variable
	unsigned char WorkW;					// $53		Any temp W variable
	unsigned char WorkH;					// $54		Any temp H variable
	unsigned char WorkVal;					// $55		Any "val" variable (just so that we have zero page access)
	unsigned char DrawIt;					// $56		The char to put onto the screen
	unsigned char* FromPtr;					// $57,$58	(ptr) used as a source for copy
	unsigned char* ToPtr;					// $59,$5A	(ptr) used as a dest for copy
	unsigned char LengthL;					// $5B,$5C	(16 bit) length container
	unsigned char LengthH;					// $5B,$5C
	unsigned char LoopI;					// $5D		General loop counter
	unsigned char* PlayerInventoryPtr;		// $5E,$5F	(ptr) to where player's inventory is stored in memory
	unsigned char*  fkdef;					// = $60/$61	FUNCTION KEY DEFINITION TABLE
    unsigned char   palnts;					// = $62		PAL/NTSC INDICATOR (0 // = NTSC)
	unsigned char* MenuDrawPtr;				// $63,$64	(ptr) used to draw menu text items
	unsigned char ByteParam0;				// $65		First byte parameter for a function call
	unsigned char ByteParam1;				// $66		Second byte parameter for a function call
	unsigned char ByteParam2;				// $67		Third byte parameter for a function call
	unsigned char PowerBarMode;				// $68		0 - Orange, 1 = Red, 2 = Green, 3 = Blue, 4 = Gray
	unsigned char VbiMode;					// $69		0 = start screen, 1 = game screen
    unsigned char   ramtop;					// = $6A		RAM SIZE DEFINED BY POWER ON LOGIC
	unsigned char LastPlayerX;				// $6B		Last player X position used to draw the GUI
	unsigned char LastPlayerY;				// $6C		Last player Y position used to draw the GUI
	unsigned char LastPlayerDirection;		// $6D		Last direction the player was facing
	unsigned char LastCurrentObject;		// $6E		Last object to be draw in the indicator
	unsigned char LastJoystickDirection;	// $6F		Last joystick/alien direction indicator drawn
	unsigned char LastScore[4];				// $70-$73	4 bytes of last score data
	unsigned char LastPowerLevel;			// $74		Last power level drawn
	unsigned char LastPosition;				// $75		Last position of player
	unsigned char LastPowerPillsLeft;		// $76		Last power pill counter drawn
	unsigned char VBICounter;				// $77		Vertical blank counter. To be used to seed the rng
	unsigned char LoopJ;					// $78		Any loop counter
	unsigned char*  keydef;					// = $79/$7A	2-BYTE KEY DEFINITION TABLE ADDRESS
	unsigned char ByteParam3;				// $7B		Forth byte parameter for function call or general use
	unsigned char   holdch;					// = $7C		CH IS MOVED HERE IN KGETCH BEFORE CNTL & SH
	unsigned char DemoSeed;					// $7D		What seed is used to generate the demo level. Each time a level is generated the seed is increased
	unsigned char PrevJoystick0;			// $7E		previously processed joystick value
	unsigned char Joystick0;				// $7F		Last joystick 0 value, 0 then nothing

	// From $80-$FF CC65 will use some locations for its ZP values
	// RMT will use from $CB - $DD (19 bytes)
};
typedef struct _asm_data _asm_data;

#define ASM_DATA (*(struct _asm_data*)0x0000)

#define TheMaze ASM_DATA.MazePtr


#endif