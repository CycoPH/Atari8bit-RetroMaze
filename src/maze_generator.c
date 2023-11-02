#include <atari.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <peekpoke.h>
#include "fixed.h"
#include "atari_fn.h"

#include "constants.h"
#include "c_asm_memory.h"
#include "globals.h"

short historyIdx = 0;		// index into the stack where the next Location will go
byte *historyX = (byte *)FIXED_HISTORYX;
byte *historyY = (byte *)FIXED_HISTORYY;

byte available[4];			// which directions are available
byte numAvailable = 0;

byte HowManyItemsPlaced[NUM_POSSIBLE_CHEST_ITEMS];

#define POS ASM_DATA.WorkPosition

void generateMaze(void)
{
	for (ASM_DATA.WorkI = 0, ASM_DATA.WorkVal = 0; ASM_DATA.WorkI < MAX_HEIGHT; ++ASM_DATA.WorkI)
	{
		MultByWidth[ASM_DATA.WorkI] = ASM_DATA.WorkVal;
		ASM_DATA.WorkVal += ASM_DATA.LevelWidth;
	}

	memset(TheMaze, (int)0, 256);
	ASM_DATA.WorkX = ASM_DATA.LevelWidth / 2;
	ASM_DATA.WorkY = ASM_DATA.LevelHeight / 2;

	POS = ASM_DATA.WorkX + MultByWidth[ASM_DATA.WorkY];

	do {
		// C->ASM TheMaze[POS] |= VISITED; // .visited = 1;
		asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
		asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]		
		asm("ora #%w", VISITED);									// Maze[WorkPosition] |= VISITED
		asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 

		// Find out where we can go
		numAvailable = 0;

		// C->ASM
		// if (ASM_DATA.WorkX > 0 && (TheMaze[POS - 1] & VISITED) == 0) {available[numAvailable++] = 'W'; }
		asm("lda %w", offsetof(_asm_data, WorkX));					// A = WorkX
		asm("beq %g", noWest);										// WorkX == 0 => noWest
		asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
		asm("dey");
		asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]		
		asm("and #%w", VISITED);									// Maze[WorkPosition] |= VISITED
		asm("bne %g", noWest);

		// available[numAvailable++] = 'W'; 
		asm("ldy %v", numAvailable);								// Y = numAvailable
		asm("inc %v", numAvailable);								// ++numAvailable
		asm("lda #$57");											// A = 'W' = $57
		asm("sta %v,y", available);									// available[Y] = $57

noWest:		
		// C->ASM if (ASM_DATA.WorkY > 0 && (TheMaze[POS - ASM_DATA.LevelWidth] & VISITED) == 0) { available[numAvailable++] = 'N'; }
		asm("lda %w", offsetof(_asm_data, WorkY));					// Y = WorkY
		asm("beq %g", noNorth);										// WorkY == 0 => noNorth
		asm("lda %w", offsetof(_asm_data, WorkPosition));			// A = WorkPosition
		asm("sec");
		asm("sbc %w", offsetof(_asm_data, LevelWidth));
		asm("tay");													// Y = WorkPosition - LevelWidth
		asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition - LevelWidth]
		asm("and #%w", VISITED);									// Maze[WorkPosition] |= VISITED
		asm("bne %g", noNorth);

		{ 
			// available[numAvailable++] = 'N'; 
			asm("ldy %v", numAvailable);								// Y = numAvailable
			asm("inc %v", numAvailable);								// ++numAvailable
			asm("lda #$4E");											// A = 'N' = $4E
			asm("sta %v,y", available);									// available[Y] = $4E
		}
noNorth:
		// C->ASM if ((byte)(ASM_DATA.WorkX+1) < ASM_DATA.LevelWidth && (TheMaze[POS + 1] & VISITED) == 0) { available[numAvailable++] = 'E'; }
		if ((byte)(ASM_DATA.WorkX+1) < ASM_DATA.LevelWidth && (TheMaze[(byte)(POS + 1)] & VISITED) == 0)
		{
			// available[numAvailable++] = 'E';
			asm("ldy %v", numAvailable);								// Y = numAvailable
			asm("inc %v", numAvailable);								// ++numAvailable
			asm("lda #$45");											// A = 'E' = $45
			asm("sta %v,y", available);									// available[Y] = $45
		}
noEast:		
		if (ASM_DATA.WorkY < (byte)(ASM_DATA.LevelHeight - 1) && (TheMaze[(byte)(POS + ASM_DATA.LevelWidth)] & VISITED) == 0)
		{ 
			// available[numAvailable++] = 'S'; 
			asm("ldy %v", numAvailable);								// Y = numAvailable
			asm("inc %v", numAvailable);								// ++numAvailable
			asm("lda #$53");											// A = 'S' = $53
			asm("sta %v,y", available);									// available[Y] = $53
		}

		if (numAvailable > 0)
		{
			// If there are unvisited exit nodes available
			// add the current location to the history
			historyX[historyIdx] = ASM_DATA.WorkX;
			historyY[historyIdx++] = ASM_DATA.WorkY;

			// Choose a direction at random
			ASM_DATA.WorkVal = rand() & 3;
			while (ASM_DATA.WorkVal >= numAvailable)
				ASM_DATA.WorkVal -= numAvailable;

			ASM_DATA.WorkVal = available[ASM_DATA.WorkVal]; // rand() % numAvailable];

			if (ASM_DATA.WorkVal == 'W') {
				// C->ASM TheMaze[POS] |= WEST;		// Mark West path open
				asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
				asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
				asm("ora #%b", (byte)WEST);									// Maze[WorkPosition] |= WEST
				asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 
				POS--;						// Move West
				ASM_DATA.WorkX--;
				// C->ASM TheMaze[POS] |= EAST;		// Mark East path of new cell open
				asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
				asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
				asm("ora #%b", (byte)EAST);									// Maze[WorkPosition] |= EAST
				asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 

			}
			else if (ASM_DATA.WorkVal == 'N')
			{
				// C-> ASM TheMaze[POS] |= NORTH;		// Mark North path open
				asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
				asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
				asm("ora #%b", (byte)NORTH);								// Maze[WorkPosition] |= NORTH
				asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 

				POS -= ASM_DATA.LevelWidth;			// Move North
				ASM_DATA.WorkY--;
				// C-> ASM TheMaze[POS] |= SOUTH;		// Mark South path of new cell open
				asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
				asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
				asm("ora #%b", (byte)SOUTH);										// Maze[WorkPosition] |= SOUTH
				asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 

			}
			else if (ASM_DATA.WorkVal == 'E')
			{
				// C-> ASM TheMaze[POS] |= EAST;		// Mark East path open
				asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
				asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
				asm("ora #%b", (byte)EAST);									// Maze[WorkPosition] |= EAST
				asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 

				POS++;					// Move East
				ASM_DATA.WorkX++;
				// C -> ASM TheMaze[POS] |= WEST;		// Mark West path of new cell open
				asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
				asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
				asm("ora #%b", (byte)WEST);									// Maze[WorkPosition] |= WEST
				asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 
			}
			else
			{
				// C -> ASM TheMaze[POS] |= SOUTH;		// Mark South path open
				asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
				asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
				asm("ora #%b", (byte)SOUTH);								// Maze[WorkPosition] |= SOUTH
				asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 

				POS += ASM_DATA.LevelWidth;			// Move South
				ASM_DATA.WorkY++;
				
				// C->ASM TheMaze[POS] |= NORTH;		// Mark North path of new cell open
				asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
				asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
				asm("ora #%b", (byte)NORTH);								// Maze[WorkPosition] |= NORTH
				asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 
			}
		}
		else 
		{
			// If there are no valid cells to move to
			// retrace one step back in history
			--historyIdx;
			//if (historyIdx < 0) { printf("No history"); exit(1); }
			ASM_DATA.WorkX = historyX[historyIdx];
			ASM_DATA.WorkY = historyY[historyIdx];
			POS = MultByWidth[ASM_DATA.WorkY];
			POS += ASM_DATA.WorkX;
		}
	} while (historyIdx > 0);

	// C->ASM
	/* for (POS = 255; POS > 0; --POS)
	{
		TheMaze[POS] &= 15; //VISITED;
	}
	TheMaze[POS] |= VISITED;
	*/
	asm("ldy #255");
nextClearLoop:	
	asm("lda (%w),y", offsetof(_asm_data, MazePtr));
	asm("and #15");												// Maze[WorkPosition] &= 15
	asm("sta (%w),y", offsetof(_asm_data, MazePtr));
	asm("dey");
	asm("bne %g", nextClearLoop);
	asm("lda (%w),y", offsetof(_asm_data, MazePtr));
	asm("ora #%b", (byte)VISITED);								// Maze[WorkPosition] |= VISITED
	asm("sta (%w),y", offsetof(_asm_data, MazePtr));

	ASM_DATA.PlayerX = 0;
	ASM_DATA.PlayerY = 0;
	ASM_DATA.PlayerPosition = 0;
	ExitPosition = ASM_DATA.LevelWidth * ASM_DATA.LevelHeight - 1;

	if (TheMaze[0] & EAST) ASM_DATA.PlayerDirection = 0; // EAST
	else ASM_DATA.PlayerDirection = 1; // SOUTH
	
	// C->ASM TheMaze[ExitPosition] |= VISITED;
	asm("ldy %v", ExitPosition);								// Y = ExitPosition
	asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[ExitPosition]
	asm("ora #%b", (byte)VISITED);								// Maze[ExitPosition] |= VISITED
	asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[ExitPosition] = WorkTile 
}

// Either place an object or a power-pill
// Objects are placed at dead-end nodes
// Start is top left (0,0)
// End is bottom right (w-1,h-1)
// All other locations get a power pill
byte NumItemsToCollectChanceTable[8] = {1,1,1,1,2,2,3,1};
byte NumItemsToCollectHighChanceTable[8] = {1,2,3,4,4,4,5,1};

#ifdef DEMO_VERSION
byte NumUnlockCost[NUM_LEVELS+1] = {
	0,
	1,
	2,
	3,
	6
};
#else
byte NumUnlockCost[NUM_LEVELS+1] = {
	0,
	1,1,1,1,1,
	2,2,2,
	3,3,3,
	4,4,4,
	6};
#endif
void PlaceObjects(void)
{
	byte lockType;
	byte val;
	byte listIsFull;

	byte numSpotsLeft;
	// Make sure we init the chest item counters
	HowManyItemsPlaced[0] = 0;		// none
	HowManyItemsPlaced[1] = 0;		// red orb
	HowManyItemsPlaced[2] = 0;		// green orb
	HowManyItemsPlaced[3] = 0;		// blue orb
	HowManyItemsPlaced[4] = 0;		// key
	HowManyItemsPlaced[5] = 0;		// bomb
	HowManyItemsPlaced[6] = 0;		// map
	HowManyItemsPlaced[7] = 0;		// vortex

	ASM_DATA.WorkLevelPtr = &Levels[ASM_DATA.LevelNr];			// We don't care about level 0 (demo)

	srand(ASM_DATA.VBICounter);			// Init the random number generator

	// Clear the memory where the object data is going to go.
	memset(MazeObjects, 0, sizeof(InteractionObject) * (MAX_OBJECT_COUNT+1));
	memset(ObjectLocation, 0, sizeof(ObjectLocation));			// Clear the object container

	ASM_DATA.ObjectCount = 0;
	listIsFull = 0;

	POS = 0;
	ASM_DATA.PowerPillsLeft = 0;
	ASM_DATA.GhostsLeft = 0;

	// Run over the whole maze left-> top->bottom and find dead-ends
	// Dead-ends only have one exit direction.
	// Place an object into each dead-end
	// Place a max of 30 objects
	// If there is no object and its not the start or end then place a power pill

	for (ASM_DATA.WorkY = 0; ASM_DATA.WorkY < ASM_DATA.LevelHeight; ++ASM_DATA.WorkY)
	{
		for (ASM_DATA.WorkX = 0; ASM_DATA.WorkX < ASM_DATA.LevelWidth; ++ASM_DATA.WorkX, ++POS)
		{
			if (POS == 0 || POS == ExitPosition) continue;

			// C->ASM ASM_DATA.WorkVal = TheMaze[POS] & DIRECTIONS;
			asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
			asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
			asm("and #%b", (byte)DIRECTIONS);							// Maze[WorkPosition] &= DIRECTIONS
			asm("sta %b", (byte)offsetof(_asm_data, WorkVal));				// Maze[WorkPosition] = WorkTile 

			if (listIsFull == 0 &&
				( (ASM_DATA.WorkVal == EAST) || (ASM_DATA.WorkVal == SOUTH) || (ASM_DATA.WorkVal == WEST) || (ASM_DATA.WorkVal == NORTH) ))
			{
				// Only ONE direction is active
				// So this is a dead-end
				// C->ASM TheMaze[POS] |= OBJECT;
				asm("ldy %w", (byte)offsetof(_asm_data, WorkPosition));		// Y = WorkPosition
				asm("lda (%b),y", (byte)offsetof(_asm_data, MazePtr));		// A = Maze[WorkPosition]
				asm("ora #%b", (byte)OBJECT);								// Maze[WorkPosition] |= OBJECT
				asm("sta (%b),y", (byte)offsetof(_asm_data, MazePtr));		// Maze[WorkPosition] = WorkTile 

				ObjectLocation[ASM_DATA.ObjectCount] = POS;

				// Place an object at this location
				MazeObjects[ASM_DATA.ObjectCount].Location = POS;

				// Don't go over XYZ objects
				++ASM_DATA.ObjectCount;
				if (ASM_DATA.ObjectCount == MAX_OBJECT_COUNT)
					listIsFull = 1;
			}
			else
			{
				// Place a dot here
				// C->ASM TheMaze[POS] |= POWER;
				asm("ldy %w", (byte)offsetof(_asm_data, WorkPosition));		// Y = WorkPosition
				asm("lda (%b),y", (byte)offsetof(_asm_data, MazePtr));		// A = Maze[WorkPosition]
				asm("ora #%b", (byte)POWER);								// Maze[WorkPosition] |= POWER
				asm("sta (%b),y", (byte)offsetof(_asm_data, MazePtr));		// Maze[WorkPosition] = WorkTile 

				// C->ASM ++ASM_DATA.PowerPillsLeft
				asm("inc %w", offsetof(_asm_data, PowerPillsLeft));
			}
		}
	}
	// objectCount is how many dead-ends there are in the maze

	// ====================================================
	// Place some chests
	// Need at least 1 chest for keys
	// How many chests are there to be allocated?
	if (ASM_DATA.ObjectCount > 0 && ASM_DATA.LevelNr)
	{
		// For non-demo level data use the level configuration to allocate objects to the level
		numSpotsLeft = ASM_DATA.ObjectCount;
		// There are objects.
		// First allocate chests
		ASM_DATA.WorkI = ASM_DATA.WorkLevelPtr->NumChest;		// I = counter of how many chests to place
		if (ASM_DATA.WorkI > ASM_DATA.ObjectCount)
			ASM_DATA.WorkI = ASM_DATA.ObjectCount;
			
		// On each chest the unlock type gets taken from 2 bits
		// 0,1 -> 2,3 -> 4,5 -> 6,7 -> 0,1
		// Bits indicating what the lock types are
		lockType = ASM_DATA.WorkLevelPtr->ChestUnlockType;

		// Run over the number of chests to place
		for (; ASM_DATA.WorkI > 0; --ASM_DATA.WorkI)
		{
			byte chestItems[3];
			// Loop pulling indexes until there is no object type set yet
			do {
				// Calc the location of the chest
				ASM_DATA.WorkVal = rand() % ASM_DATA.ObjectCount;
				ASM_DATA.InteractingWithThisObjPtr = &MazeObjects[ASM_DATA.WorkVal];

			} while (ASM_DATA.InteractingWithThisObjPtr->Type);


			--numSpotsLeft;
			ASM_DATA.InteractingWithThisObjPtr->Type = OBJECT_CHEST;		// Mark object @ WorkVal as a chest

			// Should the chest be locked?
			// Bottom two bits set the lock type
			val = lockType & 3;
			ASM_DATA.InteractingWithThisObjPtr->Data.Chest.UnlockType = val;

			if (val)
			{
				byte unlockCount = NumUnlockCost[ASM_DATA.LevelNr];
				// Set the unlock counter
				ASM_DATA.InteractingWithThisObjPtr->Data.Chest.UnlockCost = unlockCount;
			}

			// Cycle through the lock types
			val <<= 6;
			lockType >>= 2;
			lockType |= val;

			// Give the chest some contents
			do {
				chestItems[0] = (rand() & 7)+1;
				chestItems[1] = (rand() & 7)+1;
				chestItems[2] = (rand() & 7)+1;

				if (chestItems[0] > CHEST_ITEM_TYPE_VORTEX)
					chestItems[0] = CHEST_ITEM_TYPE_ORB_RED;
				if (chestItems[1] > CHEST_ITEM_TYPE_VORTEX)
					chestItems[1] = CHEST_ITEM_TYPE_ORB_GREEN;
				if (chestItems[2] > CHEST_ITEM_TYPE_VORTEX)
					chestItems[2] = CHEST_ITEM_TYPE_ORB_BLUE;
			} while (
				(chestItems[0] == chestItems[1]) || 
				(chestItems[0] == chestItems[2]) ||
				(chestItems[1] == chestItems[2])
			);

			ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeType[0] = chestItems[0];
			ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeType[1] = chestItems[1];
			ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeType[2] = chestItems[2];
			
			for (ASM_DATA.LoopI = 0; ASM_DATA.LoopI < 3; ++ASM_DATA.LoopI)
			{
				val = NumItemsToCollectChanceTable[(byte)rand() & 7];
				ASM_DATA.InteractingWithThisObjPtr->Data.Chest.TakeNum[ASM_DATA.LoopI] = val;
				HowManyItemsPlaced[chestItems[ASM_DATA.LoopI]] += val;
			}
		}

		// Chests are done.
		// Allocate the rest of the objects randomly
		// Select from Bob -> Switch off
		// When a chest is chosen it is converted to a vortex
		for (ASM_DATA.WorkI = 0; ASM_DATA.WorkI < ASM_DATA.ObjectCount; ++ASM_DATA.WorkI)
		{
			ASM_DATA.InteractingWithThisObjPtr = &MazeObjects[ASM_DATA.WorkI];
			if (ASM_DATA.InteractingWithThisObjPtr->Type == 0)
			{
				byte obj = ((byte)rand() % 8) + 1;		// 1-8 range to select bob to SwitchOff
				if (obj == OBJECT_CHEST)			// Already have done chests, make them vortex
					++obj;
				ASM_DATA.InteractingWithThisObjPtr->Type = obj;		// Assign the type of object to place

				// What is the general lock cost of objects
				val = NumUnlockCost[ASM_DATA.LevelNr];
				switch(obj)
				{
					case OBJECT_VORTEX:
					{
						// All vortexes are POWER locked and they always cost 4 to travel
						ASM_DATA.InteractingWithThisObjPtr->Data.Vortex.UnlockType = CHEST_LOCKED_TYPE_POWER;
						ASM_DATA.InteractingWithThisObjPtr->Data.Vortex.UnlockCost = val;
						break;
					}

					case OBJECT_SWITCH_OFF:
					case OBJECT_SWITCH_ON:
					{
						// All switches are POWER locked and they always cost 4 to travel
						if (ASM_DATA.LevelNr < 5)
							val = 0;
						ASM_DATA.InteractingWithThisObjPtr->Data.Vortex.UnlockType = CHEST_LOCKED_TYPE_POWER;
						ASM_DATA.InteractingWithThisObjPtr->Data.Vortex.UnlockCost = val;
						break;
					}

					case OBJECT_GHOST_BOB:
					{
						// Bob is the pussy cat among the ghosts.
						// Easy to kill and lots of loot
						// Only trades 2 for 1 up the ladder
						// 2x red -> 1x green
						// 2x green -> 1x blue
						// 2x blue -> 1x key
						ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackType = ATTACK_TYPE_POWER;
						ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost = val;
						break;
					}

					case OBJECT_GHOST_CYCO:
					{
						lockType = ATTACK_TYPE_RED;
						if (ASM_DATA.LevelNr < 5)
							lockType = CHEST_LOCKED_TYPE_POWER;
						ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackType = lockType;
						ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost = val;
						break;
					}
					case OBJECT_GHOST_ALICE: 
					{
						lockType = ATTACK_TYPE_GREEN;
						if (ASM_DATA.LevelNr < 5)
							lockType = CHEST_LOCKED_TYPE_POWER;
						ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackType = lockType;
						ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost = val;
						break;
					}
					case OBJECT_GHOST_SPECULA:
					{
						lockType = ATTACK_TYPE_BLUE;
						if (ASM_DATA.LevelNr < 5)
							lockType = CHEST_LOCKED_TYPE_POWER;						
						ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackType = lockType;
						ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost = val;
						break;						
					}
					case OBJECT_GHOST_PANDA:
					{
						ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackType = ATTACK_TYPE_ORBS;
						ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost = val + 2;
						break;
					}
				}

				// Allocate the items to collect when a ghost is killed
				if (obj >= OBJECT_GHOST_BOB && obj <= OBJECT_GHOST_PANDA)
				{
					byte collectable[3];
					do {
						// Collect red, green, blue orbs or keys
						collectable[0] = (rand() & 3)+1;
						collectable[1] = (rand() & 3)+1;
						collectable[2] = (rand() & 7)+1;	// Collect anything

						if (collectable[2] > CHEST_ITEM_TYPE_VORTEX)
							collectable[2] = CHEST_ITEM_TYPE_KEY;
					} while (
						(collectable[0] == collectable[1]) || 
						(collectable[0] == collectable[2]) ||
						(collectable[1] == collectable[2])
					);

					ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.TakeType[0] = collectable[0];
					ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.TakeType[1] = collectable[1];
					ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.TakeType[2] = collectable[2];

					++ASM_DATA.GhostsLeft;

					// Ghosts: Bob and Panda have more items to pickup once they are dead
					for (ASM_DATA.LoopI = 0; ASM_DATA.LoopI < 3; ++ASM_DATA.LoopI)
					{
						val = rand() & 7;
						if (obj == OBJECT_GHOST_BOB || obj == OBJECT_GHOST_PANDA)
						{
							val = NumItemsToCollectChanceTable[val];
						}
						else val = NumItemsToCollectChanceTable[val];
						ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.TakeNum[ASM_DATA.LoopI] = val;
						HowManyItemsPlaced[collectable[ASM_DATA.LoopI]] += val;
					}
				}
			}
		}
	}
	else {
		// This is a demo level, just randomly allocate objects
		for (ASM_DATA.WorkI = 0; ASM_DATA.WorkI < ASM_DATA.ObjectCount; ++ASM_DATA.WorkI)
		{
			byte obj = (rand() % 9) + 1;
			MazeObjects[ASM_DATA.WorkI].Type = obj;
		}
	}

	// Which are the mostly placed unlock items in the level?
	// HowManyItemsPlaced

	// The last object in the list is always the exit
	ASM_DATA.InteractingWithThisObjPtr = &MazeObjects[ASM_DATA.ObjectCount];
	ASM_DATA.InteractingWithThisObjPtr->Location = ExitPosition;
	ASM_DATA.InteractingWithThisObjPtr->Type = OBJECT_EXIT;
	ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockType0 = ASM_DATA.WorkLevelPtr->ExitUnlockType0;
	ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost0 = ASM_DATA.WorkLevelPtr->ExitUnlockCost0;
	ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockType1 = ASM_DATA.WorkLevelPtr->ExitUnlockType1;
	ASM_DATA.InteractingWithThisObjPtr->Data.Exit.UnlockCost1 = ASM_DATA.WorkLevelPtr->ExitUnlockCost1;

	ObjectLocation[ASM_DATA.ObjectCount] = ExitPosition;

	// C-> ASM 
	// TheMaze[ExitPosition] |= OBJECT;
	asm("ldy %v", ExitPosition);								// Y = ExitPosition
	asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[ExitPosition]		
	asm("ora #%w", OBJECT);										// Maze[ExitPosition] |= OBJECT
	asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[ExitPosition] = WorkTile 

	++ASM_DATA.ObjectCount;
}