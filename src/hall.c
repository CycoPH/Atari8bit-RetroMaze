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

unsigned char* ObjectToDrawXL[11] = 
{
	0,
	(unsigned char*)FIXED_GHOST_BOBXL,
	(unsigned char*)FIXED_GHOST_CYCOXL,
	(unsigned char*)FIXED_GHOST_ALICEXL,
	(unsigned char*)FIXED_GHOST_SPECULAXL,
	(unsigned char*)FIXED_GHOST_PANDAXL,
	(unsigned char*)FIXED_CHESTXL,
	(unsigned char*)FIXED_VORTEXXL,
	(unsigned char*)FIXED_SWITCHOFFXL,
	(unsigned char*)FIXED_SWITCHONXL,
	(unsigned char*)FIXED_EXITXL
};
unsigned char* ObjectToDrawM[11] = 
{
	0,
	(unsigned char*)FIXED_GHOST_BOBM,
	(unsigned char*)FIXED_GHOST_CYCOM,
	(unsigned char*)FIXED_GHOST_ALICEM,
	(unsigned char*)FIXED_GHOST_SPECULAM,
	(unsigned char*)FIXED_GHOST_PANDAM,
	(unsigned char*)FIXED_CHESTM,
	(unsigned char*)FIXED_VORTEXM,
	(unsigned char*)FIXED_SWITCHM,
	(unsigned char*)FIXED_SWITCHM,
	(unsigned char*)FIXED_EXITM
};
unsigned char* ObjectToDrawS[11] = 
{
	0,
	(unsigned char*)FIXED_GHOST_BOBS,
	(unsigned char*)FIXED_GHOST_CYCOS,
	(unsigned char*)FIXED_GHOST_ALICES,
	(unsigned char*)FIXED_GHOST_SPECULAS,
	(unsigned char*)FIXED_GHOST_PANDAS,
	(unsigned char*)FIXED_CHESTS,
	(unsigned char*)FIXED_VORTEXS,
	(unsigned char*)FIXED_SWITCHS,
	(unsigned char*)FIXED_SWITCHS,
	(unsigned char*)FIXED_EXITS
};

int ObjectOffsetXL[11] = 
{
	0,
	40*7 + 16, 		// 8x7 bob
	40*7 + 16, 		// 8x7 cyco
	40*7 + 16, 		// 8x7 alice
	40*7 + 16, 		// 8x7 specula
	40*7 + 16, 		// 8x7 panda
	40*8 + 16, 		// 8x6 chest
	40*9 + 16+2, 	// 8x7 vortex
	40*7 + 16, 		// 8x7 switch off
	40*7 + 16, 		// 8x7 switch on
	40*7 + 16		// 8x6 exit
};
byte ObjectWidthXL[11] =  {0,8,8,8,8,8,8,4,8,8,8};
byte ObjectHeightXL[11] = {0,7,7,7,7,7,6,4,7,7,8};

int ObjectOffsetM[11] = 
{
	0,
	40*8 + 17, 		// 6x5 bob
	40*8 + 17, 		// 6x5 cyco
	40*8 + 17, 		// 6x5 alice
	40*8 + 17, 		// 6x5 specula
	40*8 + 17, 		// 6x5 panda
	40*9 + 17, 		// 6x4 chest
	40*10 + 19, 	// 6x5 vortex
	40*8 + 17, 		// 6x5 switch
	40*8 + 17, 		// 6x5 switch
	40*15 + 16		// 8x2 exit
};
byte ObjectWidthM[11] =  {0,6,6,6,6,6,6,2,6,6,8};
byte ObjectHeightM[11] = {0,5,5,5,5,5,4,2,5,5,2};

int ObjectOffsetS[11] = 
{
	0,
	40*9 + 18, 		// 4x3 bob
	40*9 + 18, 		// 4x3 cyco
	40*9 + 18, 		// 4x3 alice
	40*9 + 18, 		// 4x3 specula
	40*9 + 18, 		// 4x3 panda
	40*9 + 18, 		// 4x3 chest
	40*10 + 19, 		// 4x3 vortex
	40*9 + 18, 		// 4x3 switch
	40*9 + 18, 		// 4x3 switch
	40*13 + 18		// 4x2
};
byte ObjectWidthS[11] =  {0,4,4,4,4,4,4,2,4,4,4};
byte ObjectHeightS[11] = {0,3,3,3,3,3,3,1,3,3,2};

#define wallDistance ASM_DATA.WallDistance
#define ActionDistance ASM_DATA.ActionDistance

void walkTheHall(void)
{
	// Clear the MidView[4] values
	asm("ldy     #$03");
	asm("lda     #$00");
	asm("asmClearLoop: sta %w,y", array_offsetof(_asm_data, MidView, 0));
	asm("dey");
	asm("bpl asmClearLoop");

	wallDistance = 0;
	ActionDistance = 0;
	ASM_DATA.ActionPosition = 0;
	ASM_DATA.WorkX = ASM_DATA.PlayerX;		// x & y are the player position, used to walk in a direction
	ASM_DATA.WorkY = ASM_DATA.PlayerY;

	//ASM_DATA.WorkPosition = ASM_DATA.PlayerPosition;
	asm("ldy %w", offsetof(_asm_data, PlayerPosition));			// Y = PlayerPosition
	asm("sty %w", offsetof(_asm_data, WorkPosition));			// WorkPosition = PlayerPosition
	asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
	asm("ora #%w", VISITED);									// Maze[WorkPosition] |= VISITED
	asm("sta %w", offsetof(_asm_data, WorkTile));				// WorkTile = A
	asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 
	asm("and #%b", POSSIBLE_ACTIONS);
	asm("sta %w", offsetof(_asm_data, CurrentAction));			// CurrentAction = maze[pos] & POSSIBLE_ACTIONS;

	//pos = ASM_DATA.PlayerPosition;		// WorkPosition = playerPosition
	//maze[pos] |= VISITED;			// Mark the current location as visited
	//CurrentAction = maze[pos] & POSSIBLE_ACTIONS;

	// Calculate what is visible
	// LeftView[] - indicates what to draw on the left hand side: 1=draw exit, 0=no draw
	// RightView[] - indicates what to draw on the right hand side: 1=draw exit, 0=no draw
	switch (ASM_DATA.PlayerDirection)
	{
		case DIRECTION_EAST:
		{
			ASM_DATA.LeftView[0] = ASM_DATA.WorkTile & NORTH;
			ASM_DATA.RightView[0] = ASM_DATA.WorkTile & SOUTH;
			ASM_DATA.MidView[0] = ASM_DATA.WorkTile & POSSIBLE_ACTIONS;
			if (ASM_DATA.WorkTile & OBJECT)
			{
				ActionDistance = 1;
				ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
			}
			// Walk EAST for 4 steps
			while(ASM_DATA.WorkTile & EAST && wallDistance < 4)
			{
				++ASM_DATA.WorkPosition;
				// Get the new WorkTile
				asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
				asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
				asm("sta %w", offsetof(_asm_data, WorkTile));				// WorkTile = A

				++ASM_DATA.WorkX;
				if (ASM_DATA.WorkX < ASM_DATA.LevelWidth)
				{
					++wallDistance;
					if (wallDistance < 3)
					{
						// maze[ASM_DATA.WorkPosition] |= VISITED;		// Mark the location as visited (because we can see it)
						// Get the new WorkTile
						asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
						asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
						asm("ora #%w", VISITED);									// Maze[WorkPosition] |= VISITED
						//asm("sta %w", offsetof(_asm_data, WorkTile));				// WorkTile = A
						asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 
						
						// ASM_DATA.LeftView[wallDistance] = ASM_DATA.WorkTile & NORTH;
						// ASM_DATA.RightView[wallDistance] = ASM_DATA.WorkTile & SOUTH;
						asm("ldx %w", offsetof(_asm_data, WallDistance));
						asm("tay");
						asm("and #%b", NORTH);
						asm("sta %w,x", array_offsetof(_asm_data, LeftView, 0));
						asm("tya");
						asm("and #%b", SOUTH);
						asm("sta %w,x", array_offsetof(_asm_data, RightView, 0));

						if (!ActionDistance && ASM_DATA.WorkTile & OBJECT)
						{
							ActionDistance = wallDistance + 1;
							ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
						}
					}
					if (wallDistance < 4)
					{
						// ASM_DATA.MidView[wallDistance] = ASM_DATA.WorkTile & POSSIBLE_ACTIONS;
						asm("ldx %w", offsetof(_asm_data, WallDistance));
						asm("lda %w", offsetof(_asm_data, WorkTile));
						asm("and #%b", POSSIBLE_ACTIONS);
						asm("sta %w,x", array_offsetof(_asm_data, MidView, 0));
					}
				}
			}
			// C version
			/*
			leftView[0] = maze[pos] & NORTH;
			rightView[0] = maze[pos] & SOUTH;
			midView[0] = maze[pos] & (POWER | OBJECT);
			if (maze[pos] & OBJECT)
			{
				actionDistance = 1;
				actionPosition = pos;
			}
			while(maze[pos] & EAST && wallDistance < 4)
			{
				++pos;
				++x;
				if (x < width)
				{
					++wallDistance;
					if (wallDistance < 3)
					{
						maze[pos] |= VISITED;		// Mark the location as visited (because we can see it)
						leftView[wallDistance] = maze[pos] & NORTH;
						rightView[wallDistance] = maze[pos] & SOUTH;
						if (!actionDistance && maze[pos] & OBJECT) 
						{
							actionDistance = wallDistance + 1;
							actionPosition = pos;
						}
					}
					if (wallDistance < 4)
						midView[wallDistance] = maze[pos] & (POWER | OBJECT);
				}
			}
			*/
			break;
		}
		case DIRECTION_SOUTH:
		{
			// Rewritten ASM version
			ASM_DATA.LeftView[0] = ASM_DATA.WorkTile & EAST;
			ASM_DATA.RightView[0] = ASM_DATA.WorkTile & WEST;
			ASM_DATA.MidView[0] = ASM_DATA.WorkTile & POSSIBLE_ACTIONS;
			if (ASM_DATA.WorkTile & OBJECT)
			{
				ActionDistance = 1;
				ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
			}
			while(ASM_DATA.WorkTile & SOUTH && wallDistance < 4)
			{
				// Move SOUTH
				ASM_DATA.WorkPosition += ASM_DATA.LevelWidth;
				// Get the new WorkTile
				asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
				asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
				asm("sta %w", offsetof(_asm_data, WorkTile));				// WorkTile = A

				++ASM_DATA.WorkY;
				if (ASM_DATA.WorkY < ASM_DATA.LevelHeight)
				{
					++wallDistance;
					if (wallDistance < 3)
					{
						//maze[pos] |= VISITED;		// Mark the location as visited (because we can see it)
						asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
						asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
						asm("ora #%w", VISITED);									// Maze[WorkPosition] |= VISITED
						//asm("sta %w", offsetof(_asm_data, WorkTile));				// WorkTile = A
						asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 

						//ASM_DATA.LeftView[wallDistance] = ASM_DATA.WorkTile & EAST;
						//ASM_DATA.RightView[wallDistance] = ASM_DATA.WorkTile & WEST;
						asm("ldx %w", offsetof(_asm_data, WallDistance));
						//asm("lda %w", offsetof(_asm_data, WorkTile));			// A is already the WorkTile
						asm("tay");
						asm("and #%b", EAST);
						asm("sta %w,x", array_offsetof(_asm_data, LeftView, 0));
						asm("tya");
						asm("and #%b", WEST);
						asm("sta %w,x", array_offsetof(_asm_data, RightView, 0));

						if (!ActionDistance && ASM_DATA.WorkTile & OBJECT) 
						{
							ActionDistance = wallDistance + 1;
							ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
						}
					}
					if (wallDistance < 4) {
						//ASM_DATA.MidView[wallDistance] = ASM_DATA.WorkTile & POSSIBLE_ACTIONS;
						asm("ldx %w", offsetof(_asm_data, WallDistance));
						asm("lda %w", offsetof(_asm_data, WorkTile));
						asm("and #%b", POSSIBLE_ACTIONS);
						asm("sta %w,x", array_offsetof(_asm_data, MidView, 0));
					}
				}
			}
			break;
			// C version
			/*
			ASM_DATA.LeftView[0] = ASM_DATA.WorkTile & EAST;
			ASM_DATA.RightView[0] = ASM_DATA.WorkTile & WEST;
			ASM_DATA.MidView[0] = ASM_DATA.WorkTile & POSSIBLE_ACTIONS;
			if (ASM_DATA.WorkTile & OBJECT)
			{
				ActionDistance = 1;
				ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
			}
			while(maze[ASM_DATA.WorkPosition] & SOUTH && wallDistance < 4)
			{
				ASM_DATA.WorkPosition += ASM_DATA.LevelWidth;
				++y;
				if (y < ASM_DATA.LevelHeight)
				{
					++wallDistance;
					if (wallDistance < 3)
					{
						maze[ASM_DATA.WorkPosition] |= VISITED;		// Mark the location as visited (because we can see it)
						ASM_DATA.LeftView[wallDistance] = maze[ASM_DATA.WorkPosition] & EAST;
						ASM_DATA.RightView[wallDistance] = maze[ASM_DATA.WorkPosition] & WEST;
						if (!ActionDistance && maze[ASM_DATA.WorkPosition] & OBJECT) 
						{
							ActionDistance = wallDistance + 1;
							ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
						}
					}
					if (wallDistance < 4)
						ASM_DATA.MidView[wallDistance] = maze[ASM_DATA.WorkPosition] & POSSIBLE_ACTIONS;
				}
			}
			*/
			break;
		}
		case DIRECTION_WEST:
		{
			ASM_DATA.LeftView[0] = ASM_DATA.WorkTile & SOUTH;
			ASM_DATA.RightView[0] = ASM_DATA.WorkTile & NORTH;
			ASM_DATA.MidView[0] = ASM_DATA.WorkTile & POSSIBLE_ACTIONS;
			if (ASM_DATA.WorkTile & OBJECT)
			{
				ActionDistance = 1;
				ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
			}
			// Walk WEST for 4 steps
			while(ASM_DATA.WorkTile & WEST && wallDistance < 4)
			{
				--ASM_DATA.WorkPosition;
				// Get the new WorkTile
				asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
				asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
				asm("sta %w", offsetof(_asm_data, WorkTile));				// WorkTile = A

				--ASM_DATA.WorkX;
				if (ASM_DATA.WorkX >= 0 && ASM_DATA.WorkX < ASM_DATA.LevelWidth)
				{
					++wallDistance;
					if (wallDistance < 3)
					{
						// maze[ASM_DATA.WorkPosition] |= VISITED;		// Mark the location as visited (because we can see it)
						// Get the new WorkTile
						asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
						asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
						asm("ora #%w", VISITED);									// Maze[WorkPosition] |= VISITED
						//asm("sta %w", offsetof(_asm_data, WorkTile));				// WorkTile = A
						asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 
						
						// ASM_DATA.LeftView[wallDistance] = ASM_DATA.WorkTile & SOUTH;
						// ASM_DATA.RightView[wallDistance] = ASM_DATA.WorkTile & NORTH;
						asm("ldx %w", offsetof(_asm_data, WallDistance));
						// asm("lda %w", offsetof(_asm_data, WorkTile));
						asm("tay");
						asm("and #%b", SOUTH);
						asm("sta %w,x", array_offsetof(_asm_data, LeftView, 0));
						asm("tya");
						asm("and #%b", NORTH);
						asm("sta %w,x", array_offsetof(_asm_data, RightView, 0));

						if (!ActionDistance && ASM_DATA.WorkTile & OBJECT)
						{
							ActionDistance = wallDistance + 1;
							ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
						}
					}
					if (wallDistance < 4)
					{
						// ASM_DATA.MidView[wallDistance] = ASM_DATA.WorkTile & POSSIBLE_ACTIONS;
						asm("ldx %w", offsetof(_asm_data, WallDistance));
						asm("lda %w", offsetof(_asm_data, WorkTile));
						asm("and #%b", POSSIBLE_ACTIONS);
						asm("sta %w,x", array_offsetof(_asm_data, MidView, 0));
					}
				}
			}			
			// C version
			/*
			ASM_DATA.LeftView[0] = ASM_DATA.WorkTile & SOUTH;
			ASM_DATA.RightView[0] = ASM_DATA.WorkTile & NORTH;
			ASM_DATA.MidView[0] = ASM_DATA.WorkTile & POSSIBLE_ACTIONS;
			if (ASM_DATA.WorkTile & OBJECT)
			{
				ActionDistance = 1;
				ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
			}
			while(maze[ASM_DATA.WorkPosition] & WEST && wallDistance < 4)
			{
				--ASM_DATA.WorkPosition;
				--x;
				if (x >= 0 && x < ASM_DATA.LevelWidth)
				{
					++wallDistance;
					if (wallDistance < 3)
					{
						maze[ASM_DATA.WorkPosition] |= VISITED;		// Mark the location as visited (because we can see it)
						ASM_DATA.LeftView[wallDistance] = maze[ASM_DATA.WorkPosition] & SOUTH;
						ASM_DATA.RightView[wallDistance] = maze[ASM_DATA.WorkPosition] & NORTH;
						if (!ActionDistance && maze[ASM_DATA.WorkPosition] & OBJECT) 
						{
							ActionDistance = wallDistance + 1;
							ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
						}
					}
					if (wallDistance < 4)
						ASM_DATA.MidView[wallDistance] = maze[ASM_DATA.WorkPosition] & POSSIBLE_ACTIONS;
				}
			}
			*/
			break;
		}
		case DIRECTION_NORTH:
		{
			// Rewritten ASM version
			ASM_DATA.LeftView[0] = ASM_DATA.WorkTile & WEST;
			ASM_DATA.RightView[0] = ASM_DATA.WorkTile & EAST;
			ASM_DATA.MidView[0] = ASM_DATA.WorkTile & POSSIBLE_ACTIONS;
			if (ASM_DATA.WorkTile & OBJECT)
			{
				ActionDistance = 1;
				ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
			}
			while(ASM_DATA.WorkTile & NORTH && wallDistance < 4)
			{
				// Move NORTH
				ASM_DATA.WorkPosition -= ASM_DATA.LevelWidth;
				// Get the new WorkTile
				asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
				asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
				asm("sta %w", offsetof(_asm_data, WorkTile));				// WorkTile = A

				--ASM_DATA.WorkY;
				if (ASM_DATA.WorkY >= 0 && ASM_DATA.WorkY < ASM_DATA.LevelHeight)
				{
					++wallDistance;
					if (wallDistance < 3)
					{
						//maze[pos] |= VISITED;		// Mark the location as visited (because we can see it)
						asm("ldy %w", offsetof(_asm_data, WorkPosition));			// Y = WorkPosition
						asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[WorkPosition]
						asm("ora #%w", VISITED);									// Maze[WorkPosition] |= VISITED
						//asm("sta %w", offsetof(_asm_data, WorkTile));				// WorkTile = A
						asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// Maze[WorkPosition] = WorkTile 

						//ASM_DATA.LeftView[wallDistance] = ASM_DATA.WorkTile & EAST;
						//ASM_DATA.RightView[wallDistance] = ASM_DATA.WorkTile & WEST;
						asm("ldx %w",offsetof(_asm_data, WallDistance));
						//asm("lda %w", offsetof(_asm_data, WorkTile));			// A is already the WorkTile
						asm("tay");
						asm("and #%b", WEST);
						asm("sta %w,x", array_offsetof(_asm_data, LeftView, 0));
						asm("tya");
						asm("and #%b", EAST);
						asm("sta %w,x", array_offsetof(_asm_data, RightView, 0));

						if (!ActionDistance && ASM_DATA.WorkTile & OBJECT) 
						{
							ActionDistance = wallDistance + 1;
							ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
						}
					}
					if (wallDistance < 4) {
						//ASM_DATA.MidView[wallDistance] = ASM_DATA.WorkTile & POSSIBLE_ACTIONS;
						asm("ldx %w", offsetof(_asm_data, WallDistance));
						asm("lda %w", offsetof(_asm_data, WorkTile));
						asm("and #%b", POSSIBLE_ACTIONS);
						asm("sta %w,x", array_offsetof(_asm_data, MidView, 0));
					}
				}
			}
			// C version
			/*
			ASM_DATA.LeftView[0] = ASM_DATA.WorkTile & WEST;
			ASM_DATA.RightView[0] = ASM_DATA.WorkTile & EAST;
			ASM_DATA.MidView[0] = ASM_DATA.WorkTile & POSSIBLE_ACTIONS;
			if (ASM_DATA.WorkTile & OBJECT)
			{
				ActionDistance = 1;
				ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
			}
			while(maze[ASM_DATA.WorkPosition] & NORTH && wallDistance < 4)
			{
				ASM_DATA.WorkPosition -= ASM_DATA.LevelWidth;
				--y;
				if (y >= 0 && y < ASM_DATA.LevelHeight)
				{
					++wallDistance;
					if (wallDistance < 3)
					{
						maze[ASM_DATA.WorkPosition] |= VISITED;		// Mark the location as visited (because we can see it)
						ASM_DATA.LeftView[wallDistance] = maze[ASM_DATA.WorkPosition] & WEST;
						ASM_DATA.RightView[wallDistance] = maze[ASM_DATA.WorkPosition] & EAST;
						if (!ActionDistance && maze[ASM_DATA.WorkPosition] & OBJECT) 
						{
							ActionDistance = wallDistance + 1;
							ASM_DATA.ActionPosition = ASM_DATA.WorkPosition;
						}
					}
					if (wallDistance < 4)
						ASM_DATA.MidView[wallDistance] = maze[ASM_DATA.WorkPosition] & POSSIBLE_ACTIONS;
				}
			}
			*/
			break;
		}
	}
}

// --------------------------------------------------------
// Draw the locks for the current LARGE object
// ASM_DATA.InteractingWithThisObjPtr is the ptr to the object data
// ASM_DATA.ToPtr is the top left location of the object on the screen
// ASM_DATA.WorkW
void DrawLockedState()
{
	switch (ASM_DATA.InteractingWithThisObjPtr->Type)
	{
		case OBJECT_CHEST:
		{
			// This is a chest
			asm("jsr %w", FIXED_DRAWCHESTLOCK);
			break;
		}

		case OBJECT_VORTEX:
		{
			// This is a vortex
			asm("jsr %w", FIXED_DRAWVORTEXLOCK);
			break;
		}

		case OBJECT_SWITCH_OFF:
		case OBJECT_SWITCH_ON:
			asm("jsr %w", FIXED_DRAWSWITCHLOCK);
			break;

		case OBJECT_EXIT:
		{
			// This is the Exit
			// It might be locked
			// It can have 0, 1 or 2 lock types
			asm("jsr %w", FIXED_DRAWEXITLOCK);
			break;
		}
	}
	
}


#define screenPtr ASM_DATA.ScreenPtr

void drawTheHall()
{
	// Copy the original 3D view
	/*
	if (ASM_DATA.PageNr == 0)
	{
		ASM_DATA.PagePtr = (unsigned char *)FIXED_GAMESCREEN1;
	}
	else
	{
		ASM_DATA.PagePtr = (unsigned char *)FIXED_GAMESCREEN0;
	}

	memcpy(ASM_DATA.PagePtr, (char*)FIXED_ORIGINAL3DVIEW, 840);
	*/
	asm("jsr %w", FIXED_DRAWTHEHALL);

	/*
	switch (wallDistance)
	{
		case 0:
		{
			// Make the closest wall a full solid one
			screenPtr = ASM_DATA.PagePtr + WALL_0_TL_OFFSET;
			*screenPtr = SOLID_WALL_TL;
			++screenPtr;
			*screenPtr = WALL_CONNECT_H;
			screenPtr += WALL_0_HORIZ_WIDTH;
			*screenPtr = WALL_CONNECT_H;
			++screenPtr;
			*screenPtr = SOLID_WALL_TR;

			screenPtr = ASM_DATA.PagePtr + 4*40 + 7;
			for (ASM_DATA.WorkY = 4; ASM_DATA.WorkY < 17; ++ASM_DATA.WorkY)
			{
				for (ASM_DATA.WorkX = WALL_0_HORIZ_WIDTH+1; ASM_DATA.WorkX > 0; --ASM_DATA.WorkX)
				{
					*screenPtr = 0;
					++screenPtr;
				}
				screenPtr += (40 - WALL_0_HORIZ_WIDTH -1);
			}
			--screenPtr;
			*screenPtr = SOLID_WALL_BL;
			++screenPtr;
			for (ASM_DATA.WorkX = WALL_0_HORIZ_WIDTH+1; ASM_DATA.WorkX > 0; --ASM_DATA.WorkX)
			{
				*screenPtr = WALL_CONNECT_H;
				++screenPtr;
			}
			*screenPtr = SOLID_WALL_BR;
			break;
		}
		case 1:
		{
			// Make the 2nd closest wall solid:
			// 12,6 -> 27,14
			// Make the closest wall a full solid one
			screenPtr = ASM_DATA.PagePtr + WALL_1_TL_OFFSET;
			*screenPtr = SOLID_WALL_TL;
			++screenPtr;
			*screenPtr = WALL_CONNECT_H;
			screenPtr += WALL_1_HORIZ_WIDTH;
			*screenPtr = WALL_CONNECT_H;
			++screenPtr;
			*screenPtr = SOLID_WALL_TR;

			screenPtr = ASM_DATA.PagePtr + WALL_1_CLEAR_LEFT;
			for (ASM_DATA.WorkY = 7; ASM_DATA.WorkY < 14; ++ASM_DATA.WorkY)
			{
				for (ASM_DATA.WorkX = WALL_1_HORIZ_WIDTH+1; ASM_DATA.WorkX > 0; --ASM_DATA.WorkX)
				{
					*screenPtr = 0;
					++screenPtr;
				}
				screenPtr += (40 - WALL_1_HORIZ_WIDTH -1);
			}
			--screenPtr;
			*screenPtr = SOLID_WALL_BL;
			++screenPtr;
			for (ASM_DATA.WorkX = WALL_1_HORIZ_WIDTH+1; ASM_DATA.WorkX > 0; --ASM_DATA.WorkX)
			{
				*screenPtr = WALL_CONNECT_H;
				++screenPtr;
			}
			*screenPtr = SOLID_WALL_BR;
			break;
		}

		case 2:
		{
			// Make the 3nd closest wall solid:
			// 16,8 -> 23,12
			// Make the closest wall a full solid one
			screenPtr = ASM_DATA.PagePtr + WALL_2_TL_OFFSET;
			*screenPtr = SOLID_WALL_TL;
			++screenPtr;
			*screenPtr = WALL_CONNECT_H;
			screenPtr += WALL_2_HORIZ_WIDTH;
			*screenPtr = WALL_CONNECT_H;
			++screenPtr;
			*screenPtr = SOLID_WALL_TR;

			screenPtr = ASM_DATA.PagePtr + WALL_2_CLEAR_LEFT;
			for (ASM_DATA.WorkY = 9; ASM_DATA.WorkY < 12; ++ASM_DATA.WorkY)
			{
				for (ASM_DATA.WorkX = WALL_2_HORIZ_WIDTH+1; ASM_DATA.WorkX > 0; --ASM_DATA.WorkX)
				{
					*screenPtr = 0;
					++screenPtr;
				}
				screenPtr += (40 - WALL_2_HORIZ_WIDTH -1);
			}
			--screenPtr;
			*screenPtr = SOLID_WALL_BL;
			++screenPtr;
			for (ASM_DATA.WorkX = WALL_2_HORIZ_WIDTH+1; ASM_DATA.WorkX > 0; --ASM_DATA.WorkX)
			{
				*screenPtr = WALL_CONNECT_H;
				++screenPtr;
			}
			*screenPtr = SOLID_WALL_BR;
			break;
		}
	}
	*/

	// This is drawing the side exits. From the closest to the farthest.
	/*
	if (ASM_DATA.LeftView[0])
	{
		// Top
		screenPtr = ASM_DATA.PagePtr + LEFT_EXIT_0_TOP_OFFSET;
		*screenPtr = 27;	// T rotated left
		++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = 8; ++screenPtr;
		*screenPtr = wallDistance ? 31 : 43;

		// Bottom
		screenPtr = ASM_DATA.PagePtr + LEFT_EXIT_0_BOTTOM_OFFSET;
		// Line 17 |----- x
		*screenPtr = 27; ++screenPtr;	// T rotated left
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = wallDistance ? 25 : 80;
		// Line 18
		screenPtr += (40-3);
		*screenPtr = 0; ++screenPtr;
		*screenPtr = 0; ++screenPtr;
		*screenPtr = 0; ++screenPtr;
		// Line 19
		screenPtr += (40-5);
		*screenPtr = 0; ++screenPtr;
		*screenPtr = 0; ++screenPtr;
		*screenPtr = 0; ++screenPtr;
		// Line 20
		screenPtr += (40-4);
		*screenPtr = 35; ++screenPtr;
		*screenPtr = WALL_CONNECT_H;
	}

	if (ASM_DATA.RightView[0])
	{
		// Top
		screenPtr = ASM_DATA.PagePtr + RIGHT_EXIT_0_TOP_OFFSET;
		*screenPtr = wallDistance ? 32 : 44;
		++screenPtr;
		*screenPtr = 7; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = 28;

		// Bottom
		screenPtr = ASM_DATA.PagePtr + RIGHT_EXIT_0_BOTTOM_OFFSET;
		// Line 17
		*screenPtr = wallDistance ? 26 : 80; ++screenPtr;	// Handle Wall drawn in
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = 28;
		// Line 18
		screenPtr += (40-5);
		*screenPtr = 0; ++screenPtr;
		*screenPtr = 0; ++screenPtr;
		*screenPtr = 0; ++screenPtr;
		// Line 19
		screenPtr += (40-1);
		*screenPtr = 0; ++screenPtr;
		*screenPtr = 0; ++screenPtr;
		*screenPtr = 0; ++screenPtr;
		// Line 20
		screenPtr += (40-1);
		*screenPtr = WALL_CONNECT_H; ++screenPtr;
		*screenPtr = 36;
	}
	*/

	/*

	if (wallDistance > 0)
	{
		// Left middle exit
		if (ASM_DATA.LeftView[1])
		{
			screenPtr = ASM_DATA.PagePtr + LEFT_EXIT_1_TOP_OFFSET;
			// Line 6
			*screenPtr = 29; screenPtr += 40;				// Line 1
			*screenPtr = WALL_CONNECT_V; screenPtr += 40;	// Line 2
			*screenPtr = 27; ++screenPtr;					// Line 3 |---
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = 8; ++screenPtr;
			*screenPtr = wallDistance == 1 ? 43 : 31; 
			screenPtr += (40-5);
			for (ASM_DATA.WorkY = 10; ASM_DATA.WorkY > 0; --ASM_DATA.WorkY)
			{
				*screenPtr = WALL_CONNECT_V;
				screenPtr += 40;
			}
			screenPtr -= 39;
			*screenPtr = 0; ++screenPtr;
			*screenPtr = 0;
			screenPtr -= 40;
			*screenPtr = 0; ++screenPtr;
			*screenPtr = 0; ++screenPtr;
			*screenPtr = 0;
			screenPtr -= 44;
			*screenPtr = 27; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = wallDistance == 1 ? 80 : 25;	// Inverted T
		}

		// Right middle exit
		if (ASM_DATA.RightView[1])
		{
			screenPtr = ASM_DATA.PagePtr + RIGHT_EXIT_1_TOP_OFFSET;
			// Line 6 x:27
			*screenPtr = wallDistance == 1 ? 44 : 32; ++screenPtr;
			*screenPtr = 7; ++screenPtr; // 28,6
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = 28;
			// Line 5
			screenPtr -= 40;
			*screenPtr = WALL_CONNECT_V;
			// Line 4
			screenPtr -= 40;
			*screenPtr = 30;
			// Line 11 Draw wall section
			screenPtr += (3*40);
			for (ASM_DATA.WorkY = 10; ASM_DATA.WorkY > 0; --ASM_DATA.WorkY)
			{
				*screenPtr = WALL_CONNECT_V;
				screenPtr += 40;
			}
			screenPtr -= 41;
			*screenPtr = 0; --screenPtr;
			*screenPtr = 0; screenPtr -= 40;
			*screenPtr = 0; --screenPtr;
			*screenPtr = 0; --screenPtr;
			*screenPtr = 0;
			screenPtr -= 40;
			*screenPtr = 0; --screenPtr;
			*screenPtr = wallDistance == 1 ? 80 : 26; ++screenPtr; // Inverted T
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = 28;
		}
	}
	*/


	/*
	if (wallDistance > 1)
	{
		// Left far exit
		if (ASM_DATA.LeftView[2])
		{
			screenPtr = ASM_DATA.PagePtr + LEFT_EXIT_2_TOP_OFFSET;
			// Line 6
			*screenPtr = 29; screenPtr += 40;				// Line 1
			*screenPtr = 27; ++screenPtr;					// Line 3 |---
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = 8; ++screenPtr;
			*screenPtr = wallDistance == 2 ? 43 : 31; 
			screenPtr += (40-3);
			for (ASM_DATA.WorkY = 5; ASM_DATA.WorkY > 0; --ASM_DATA.WorkY)
			{
				*screenPtr = WALL_CONNECT_V;
				screenPtr += 40;
			}
			screenPtr -= 39;
			*screenPtr = 0; ++screenPtr;
			*screenPtr = 0;
			screenPtr -= 42;
			*screenPtr = 27; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = wallDistance == 2 ? 80 : 25;	// Inverted T
		}
		// Right far exit
		if (ASM_DATA.RightView[2])
		{
			screenPtr = ASM_DATA.PagePtr + RIGHT_EXIT_2_TOP_OFFSET;
			// Line 8 x:23
			*screenPtr = wallDistance == 2 ? 44 : 32; ++screenPtr;
			*screenPtr = 7; ++screenPtr; // 24,8
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = 28;
			// Line 7
			screenPtr -= 40;
			*screenPtr = 30;
			// Line 11 Draw wall section
			screenPtr += (2*40);
			for (ASM_DATA.WorkY = 5; ASM_DATA.WorkY > 0; --ASM_DATA.WorkY)
			{
				*screenPtr = WALL_CONNECT_V;
				screenPtr += 40;
			}
			screenPtr -= 41;
			*screenPtr = 0; --screenPtr;
			*screenPtr = 0; screenPtr -= 40;
			*screenPtr = 0; --screenPtr;
			*screenPtr = wallDistance == 2 ? 80 : 26; ++screenPtr; // Inverted T
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = WALL_CONNECT_H; ++screenPtr;
			*screenPtr = 28;
		}
	}
	*/

	// ----------------------------------------------------
	// Draw the power dots in the middle of the screen
	if (wallDistance >= 3 && (ASM_DATA.MidView[3] & POWER))
	{
		// Draw a power dot as distance 3
		ASM_DATA.ScreenPtr = ASM_DATA.PagePtr + (40*11 + 19);
		asm("ldy #0");
		asm("lda #%b", POWER_LEFT_AT_DIST_3);
		asm("sta (%w),y", access(ScreenPtr));
		asm("iny");
		asm("lda #%b", POWER_RIGHT_AT_DIST_3);
		asm("sta (%w),y", access(ScreenPtr));
	}
	if (wallDistance >= 2 && (ASM_DATA.MidView[2] & POWER))
	{
		// Draw a power dot as distance 2
		ASM_DATA.ScreenPtr = ASM_DATA.PagePtr + (40*13 + 19);
		asm("ldy #0");
		asm("lda #%b", POWER_LEFT_AT_DIST_2);
		asm("sta (%w),y", access(ScreenPtr));
		asm("iny");
		asm("lda #%b", POWER_RIGHT_AT_DIST_2);
		asm("sta (%w),y", access(ScreenPtr));
	}
	if (wallDistance >= 1 && (ASM_DATA.MidView[1] & POWER))
	{
		// Draw a power dot as distance 1
		ASM_DATA.ScreenPtr = ASM_DATA.PagePtr + (40*15 + 19);
		asm("ldy #0");
		asm("lda #%b", POWER_LEFT_LN1_AT_DIST_1);
		asm("sta (%w),y", access(ScreenPtr));
		asm("iny");
		asm("lda #%b", POWER_RIGHT_LN1_AT_DIST_1);
		asm("sta (%w),y", access(ScreenPtr));
		asm("ldy #40");
		asm("lda #%b", POWER_LEFT_LN2_AT_DIST_1);
		asm("sta (%w),y", access(ScreenPtr));
		asm("iny");
		asm("lda #%b", POWER_RIGHT_LN2_AT_DIST_1);
		asm("sta (%w),y", access(ScreenPtr));
	}

	// Determine which object we are about to interface with
	// Need to be on the tile that contains the object to interface with it
	// "ActionDistance" is the distance to the next object
	ASM_DATA.CurrentObject = 0;		//	0 = no object to interact with, anything else then .InteractingWithThisObjPtr is a ptr to the data for the object
	ASM_DATA.InteractingWithThisObjPtr = 0;

	// ASM_DATA.WorkVal = What object is going to be in the view
	// ASM_DATA.ActionPosition = where in the maze the action/view is located (NOT the player's position)

	if (ActionDistance) 
	{
		// Something is in our view.
		// Draw something in the correct size
		// 1 - BIG, very close
		// 2 - Med, one unit away
		// 3 - Small, two units away
		// Find out what to draw by comparing the "ActionPosition" to the ObjectLocation
		ASM_DATA.WorkVal = 0;		// This is the type of object to draw (Ghost, Chest etc [1-10])
		for (ASM_DATA.WorkI = 0; ASM_DATA.WorkI < ASM_DATA.ObjectCount; ++ASM_DATA.WorkI)
		{
			ASM_DATA.InteractingWithThisObjPtr = &MazeObjects[ASM_DATA.WorkI];

			if (ASM_DATA.ActionPosition == ASM_DATA.InteractingWithThisObjPtr->Location)
			{
				ASM_DATA.WorkVal = ASM_DATA.InteractingWithThisObjPtr->Type;
				break;
			}
		}

		if (ASM_DATA.WorkVal)
		{
			if (ActionDistance == 1)
			{
				// This is going to be the big one
				// This will be the one to interact with
				screenPtr = ASM_DATA.PagePtr + ObjectOffsetXL[ASM_DATA.WorkVal];
				ASM_DATA.FromPtr = ObjectToDrawXL[ASM_DATA.WorkVal];
				ASM_DATA.WorkW = ObjectWidthXL[ASM_DATA.WorkVal];
				ASM_DATA.WorkH = ObjectHeightXL[ASM_DATA.WorkVal];
				ASM_DATA.WorkI = 40 - ASM_DATA.WorkW;
				ASM_DATA.CurrentObject = ASM_DATA.WorkVal;
			}
			else if (ActionDistance == 2)
			{
				screenPtr = ASM_DATA.PagePtr + ObjectOffsetM[ASM_DATA.WorkVal];
				ASM_DATA.FromPtr = ObjectToDrawM[ASM_DATA.WorkVal];
				ASM_DATA.WorkW = ObjectWidthM[ASM_DATA.WorkVal];
				ASM_DATA.WorkH = ObjectHeightM[ASM_DATA.WorkVal];
				ASM_DATA.WorkI = 40 - ASM_DATA.WorkW;
			}
			else
			{
				screenPtr = ASM_DATA.PagePtr + ObjectOffsetS[ASM_DATA.WorkVal];
				ASM_DATA.FromPtr = ObjectToDrawS[ASM_DATA.WorkVal];
				ASM_DATA.WorkW = ObjectWidthS[ASM_DATA.WorkVal];
				ASM_DATA.WorkH = ObjectHeightS[ASM_DATA.WorkVal];
				ASM_DATA.WorkI = 40 - ASM_DATA.WorkW;
			}
			// Render the interaction object in big, medium, or small
			ASM_DATA.ToPtr = screenPtr;			// So that we can reuse it later			
			for (ASM_DATA.WorkY = 0; ASM_DATA.WorkY < ASM_DATA.WorkH; ++ASM_DATA.WorkY)
			{
				for (ASM_DATA.WorkX = 0; ASM_DATA.WorkX < ASM_DATA.WorkW; ++ASM_DATA.WorkX) {
					*screenPtr = *ASM_DATA.FromPtr; 
					++screenPtr;
					++ASM_DATA.FromPtr;
				}
				screenPtr += ASM_DATA.WorkI;
			}
			if (ActionDistance == 1)
				DrawLockedState();
		}
	}
}
