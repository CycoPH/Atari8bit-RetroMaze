#include <atari.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "fixed.h"
#include "atari_fn.h"

#include "constants.h"
#include "globals.h"
#include "c_asm_memory.h"
#include "hall.h"
#include "maze_generator.h"
#include "draw_map.h"

void DrawMapSmall()
{
	short yOffset;
	short skipToNextLine;

	skipToNextLine = (40-2) - ASM_DATA.LevelWidth;
	yOffset = MultScreen[((14-2) - ASM_DATA.LevelHeight) >> 1];
	ASM_DATA.WorkVal = skipToNextLine >> 1;

	ASM_DATA.ScreenPtr = (unsigned char*)(FIXED_MAPSCREEN + ASM_DATA.WorkVal + yOffset);

	// Draw the frame around the map
	asm("ldy %w", offsetof(_asm_data, LevelWidth));
	asm("iny");
	asm("lda #18");
	asm("sta (%w),y", offsetof(_asm_data, ScreenPtr));
	asm("dey");
	asm("lda #17");
	asm("_topDrawLoop: sta (%w),y", offsetof(_asm_data, ScreenPtr));
	asm("dey");
	asm("bne _topDrawLoop");
	asm("lda #16");
	asm("sta (%w),y", offsetof(_asm_data, ScreenPtr));

	ASM_DATA.ScreenPtr += 40;

	ASM_DATA.WorkPosition = 0;
	for (ASM_DATA.WorkY = 0; ASM_DATA.WorkY < ASM_DATA.LevelHeight; ++ASM_DATA.WorkY)
	{
		*ASM_DATA.ScreenPtr = 20;
		++ASM_DATA.ScreenPtr;

		for (ASM_DATA.WorkX = 0; ASM_DATA.WorkX < ASM_DATA.LevelWidth; ++ASM_DATA.WorkX)
		{
			// C->ASM ASM_DATA.WorkVal = ASM_DATA.MazePtr[ASM_DATA.WorkPosition];
			asm("ldy %w", offsetof(_asm_data, WorkPosition));	
			asm("lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
			asm("sta %w", offsetof(_asm_data, WorkVal));
			
			// C->ASM ASM_DATA.DrawIt = ASM_DATA.WorkVal & 15;			// This is the map tile to draw
			asm("and  #$0F");
			asm("sta %w", offsetof(_asm_data, DrawIt));

			// Check for player position
			if (ASM_DATA.WorkPosition == ASM_DATA.PlayerPosition)
			{
				// Drawing the position where the player is now
				switch (ASM_DATA.PlayerDirection)
				{
					case 0:	ASM_DATA.DrawIt += 68-1; break;		// EAST
					case 1:	ASM_DATA.DrawIt += 83-1; break;		// SOUTH
					case 2:	ASM_DATA.DrawIt += 98-1; break;		// WEST
					case 3:	ASM_DATA.DrawIt += 113-1; break;	// NORTH
				}
			}
			else if ((ASM_DATA.WorkVal & VISITED) == 0)
			{
				// Not seen this part of the maze yet. 
				// So its drawn blank
				ASM_DATA.DrawIt = 0;
			}
			else if (ASM_DATA.WorkVal & POWER)
			{
				// Not on this tile and have seen it and it has power
				ASM_DATA.DrawIt += 38-1;
			}
			else 
			{
				// Check if its one of the objects
				for (ASM_DATA.WorkI = 0; ASM_DATA.WorkI < ASM_DATA.ObjectCount; ++ASM_DATA.WorkI)
				{
					if (ASM_DATA.WorkPosition == ObjectLocation[ASM_DATA.WorkI])
					{
						ASM_DATA.DrawIt += 23-1;
						// Note that this object was interacted with
						if (ASM_DATA.WorkVal & OBJECT_DONE)
							ASM_DATA.DrawIt += 30;
						break;
					}
				}
			}
			
			*ASM_DATA.ScreenPtr = ASM_DATA.DrawIt;
			++ASM_DATA.ScreenPtr;

			++ASM_DATA.WorkPosition;
		}
		*ASM_DATA.ScreenPtr = 20;
		++ASM_DATA.ScreenPtr;

		ASM_DATA.ScreenPtr += skipToNextLine;
	}

	asm("ldy %w", offsetof(_asm_data, LevelWidth));
	asm("iny");
	asm("lda #21");
	asm("sta (%w),y", offsetof(_asm_data, ScreenPtr));
	asm("dey");
	asm("lda #17");
	asm("_bottomDrawLoop: sta (%w),y", offsetof(_asm_data, ScreenPtr));
	asm("dey");
	asm("bne _bottomDrawLoop");
	asm("lda #19");
	asm("sta (%w),y", offsetof(_asm_data, ScreenPtr));	
}