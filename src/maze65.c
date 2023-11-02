#include <atari.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "fixed.h"
#include "atari_fn.h"
#include <peekpoke.h>

#include "constants.h"
#include "globals.h"
#include "c_asm_memory.h"
#include "hall.h"
#include "maze_generator.h"
#include "draw_map.h"
#include "map_gui.h"

// Level Notes:
// Chest unlock cost defined by LevelNr
// 		1-5 cost 2
// 		6-10 cost 4
// 		> 10 cost 6
// Chest contents is randomly selected, but never the same two (so always three different items [Red, Green, Blue, Key, Bomb, Map, Vortex])
// Other objects are allocated to be:
//	Ghosts: Bob-1, Cyco-2, Alice-3, Specula-4, Panda-5
//	Vortex-7
//	Switch-8
// Vortex has a power activation cost defined by LevelNr
//		1-3 cost 0
//		4-5 cost 2
//		6-10 cost 4
//		>10 cost 6
// Switch has a power activation cost defined by LevelNr
// 		1-5 cost 2
// 		6-10 cost 4
// 		> 10 cost 6
// Ghost: Bob
//
// Level 1	5x5		2 chests None/POWER		Exit POWER/6 RED/1

// NOTES:
// Max height of a level is 12
// Width * Height HAS TO BE LESS THAN 256
#ifdef DEMO_VERSION
LevelDefinition Levels[NUM_LEVELS+1] = {
//   W H Seed	#chest
	{5,5,128,		2},
	// Level 1
	{5,5,0, 	2, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_NONE, CHEST_LOCKED_TYPE_POWER, 0,0), 							EXIT_LOCKED_TYPE_POWER, 6, 	EXIT_LOCKED_TYPE_RED, 	1},		// Level 1
	// Level 6
	{10,10,0,	3, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_POWER, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_ORBS, 0),	EXIT_LOCKED_TYPE_BLUE,	2,	EXIT_LOCKED_TYPE_GREEN,	1},
	// Level 11
	{34,7,0,	5, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_NONE, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_KEYS, CHEST_LOCKED_TYPE_KEYS),	EXIT_LOCKED_TYPE_KEYS,	4,	EXIT_LOCKED_TYPE_GREEN,	6},
	{38,6,0,	5, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_KEYS, CHEST_LOCKED_TYPE_KEYS),	EXIT_LOCKED_TYPE_KEYS,	2,	EXIT_LOCKED_TYPE_GREEN,	2}		// Level 15
};	
#else
LevelDefinition Levels[NUM_LEVELS+1] = {
//   W H Seed	#chest
	{5,5,128,		2},
	// Level 1
	{5,5,0, 	2, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_NONE, CHEST_LOCKED_TYPE_POWER, 0,0), 							EXIT_LOCKED_TYPE_POWER, 6, 	EXIT_LOCKED_TYPE_RED, 	1},		// Level 1
	{6,6,0,		2, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_NONE, CHEST_LOCKED_TYPE_POWER, 0,0),							EXIT_LOCKED_TYPE_RED,	2,	EXIT_LOCKED_TYPE_GREEN,	2},
	{7,7,0,		2, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_POWER, 0,0), 							EXIT_LOCKED_TYPE_RED,	3,	EXIT_LOCKED_TYPE_GREEN,	2},
	{8,8,0,		2, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_POWER, 0,0),							EXIT_LOCKED_TYPE_RED,	2,	EXIT_LOCKED_TYPE_GREEN,	3},
	{9,9,0,		3, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_POWER, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_ORBS, 0),	EXIT_LOCKED_TYPE_RED,	2,	EXIT_LOCKED_TYPE_GREEN,	2},		// Level 5
	// Level 6
	{10,10,0,	3, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_POWER, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_ORBS, 0),	EXIT_LOCKED_TYPE_BLUE,	2,	EXIT_LOCKED_TYPE_GREEN,	1},
	{11,11,0,	3, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_KEYS, 0),		EXIT_LOCKED_TYPE_BLUE,	4,	EXIT_LOCKED_TYPE_GREEN,	3},
	{12,12,0,	4, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_KEYS, CHEST_LOCKED_TYPE_KEYS),	EXIT_LOCKED_TYPE_RED,	5,	EXIT_LOCKED_TYPE_BLUE,	2},
	{14,12,0,	4, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_POWER, CHEST_LOCKED_TYPE_KEYS),	EXIT_LOCKED_TYPE_POWER,	9,	EXIT_LOCKED_TYPE_BLUE,	3},
	{16,12,0,	5, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_NONE, CHEST_LOCKED_TYPE_POWER, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_KEYS),	EXIT_LOCKED_TYPE_KEYS,	2,	EXIT_LOCKED_TYPE_GREEN,	2},		// Level10
	// Level 11
	{20,12,0,	5, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_KEYS, CHEST_LOCKED_TYPE_KEYS),	EXIT_LOCKED_TYPE_RED,	5,	EXIT_LOCKED_TYPE_GREEN,	5},		
	{24,10,0,	5, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_KEYS, CHEST_LOCKED_TYPE_KEYS),	EXIT_LOCKED_TYPE_GREEN,	5,	EXIT_LOCKED_TYPE_BLUE,	5},
	{30,8,0,	5, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_KEYS, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_KEYS),	EXIT_LOCKED_TYPE_BLUE,	4,	EXIT_LOCKED_TYPE_KEYS,	2},
	{34,7,0,	5, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_NONE, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_KEYS, CHEST_LOCKED_TYPE_KEYS),	EXIT_LOCKED_TYPE_KEYS,	4,	EXIT_LOCKED_TYPE_GREEN,	6},
	{38,6,0,	5, MAKE_CHEST_LOCKS(CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_ORBS, CHEST_LOCKED_TYPE_KEYS, CHEST_LOCKED_TYPE_KEYS),	EXIT_LOCKED_TYPE_KEYS,	2,	EXIT_LOCKED_TYPE_GREEN,	2}		// Level 15
};
#endif

Inventory PlayerInventory;		// What is the playing carrying

byte ActionDistance;			// How far is the next action object away from the player?
//byte wallDistance;

// Object information
byte ObjectLocation[MAX_OBJECT_COUNT+1];		// Where are the objects located (pos = x+y*width)
InteractionObject* MazeObjects = (InteractionObject*)FIXED_HISTORYY;

byte MultByWidth[MAX_HEIGHT];
short MultScreen[21] = {0,40,80,120,160,200,240,280,320,360,400,440,480,520,560,600,640,680,720,760,800};


// Place items in Zero-page
#pragma bss-name (push,"ZEROPAGE")
#pragma data-name (push,"ZEROPAGE")
byte pos;
#pragma bss-name (pop)
#pragma data-name (pop)
#pragma zpsym ("pos")

byte mode;					// 0 - 3D , 1 - Map
byte ExitPosition;			// Where is the exit located?

byte MaxLevelCompleted = MAX_LEVEL_UNLOCKED_AT_START;	// What is the maximum level you can start at

void wait()
{
	WaitForVbi();
	ReadJoystick();
}

void SwopPages()
{
	ASM_DATA.PageNr = (ASM_DATA.PageNr + 1) & 1;

	if (ASM_DATA.PageNr == 0)
	{
		ShowGameScreen0();
	}
	else
	{
		ShowGameScreen1();
	}
}

void ResetGame()
{
	ASM_DATA.CurrentObject = 0;
	ASM_DATA.JoystickDirection = 0;
	ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_NONE;

	PlayerInventory.Keys = 0;
	PlayerInventory.Red = 0;
	PlayerInventory.Green = 0;
	PlayerInventory.Blue = 0;
	PlayerInventory.PowerLevel = 0;
	PlayerInventory.CurrentColor = 0;

	PlayerInventory.Bombs = 1;
	PlayerInventory.Maps = 1;
	PlayerInventory.Vortex = 1;
	ASM_DATA.LevelNr = 1;

	ResetScore();
}

void drawGui()
{
	DrawTopGui();
	DrawScore();
}

// --------------------------------------------------------
// A power pill is being picked up
// - Clear the maze location of the POWER bit
// - Add 10 score
// - Depending on the difficulty add 3/2/1 to the power level
// - If the power is full (> 32) then make an orb out of the power
// - First Red, then Green, then Blue orbs, then back to Red
void ActionPickupPowerPill()
{
	// Clear the power pill from the maze
	// C->ASM TheMaze[ASM_DATA.PlayerPosition] &= ~POWER;
	asm("ldy %w", offsetof(_asm_data, PlayerPosition));			// Y = PlayerPosition
	asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[PlayerPosition]		
	asm("and #%b", (byte)~POWER);
	asm("sta (%w),y", offsetof(_asm_data, MazePtr));			// TheMaze[ASM_DATA.PlayerPosition] &= ~POWER

	Add10ToScore();
	POKE(SFX_CH1, SOUND_PICKUP_AN_POWER_PILL);
	--ASM_DATA.PowerPillsLeft;

	// Add the power to the inventory
	switch (ASM_DATA.Difficulty)
	{
		case DIFFICULTY_LEVEL_HARD:
			++PlayerInventory.PowerLevel;
			break;

		case DIFFICULTY_LEVEL_MEDIUM:
			PlayerInventory.PowerLevel = PlayerInventory.PowerLevel + 2;
			break;

		default:
			PlayerInventory.PowerLevel = PlayerInventory.PowerLevel + 3;
	}
	if (PlayerInventory.PowerLevel > 32)
	{
		// Filled up the power bar.
		// Convert it into a Red, Green or Blue orb
		if (PlayerInventory.CurrentColor == 0)
			++PlayerInventory.Red;
		else if (PlayerInventory.CurrentColor == 1)
			++PlayerInventory.Green;
		else if (PlayerInventory.CurrentColor == 2)
			++PlayerInventory.Blue;
		PlayerInventory.PowerLevel -= 32;
		++PlayerInventory.CurrentColor;
		if (PlayerInventory.CurrentColor == 3)
			PlayerInventory.CurrentColor = 0;
		ASM_DATA.PowerBarMode = PlayerInventory.CurrentColor + 1;		// Tell the gui the color of the orb that will be created next

		POKE(SFX_CH1, SOUND_MAKE_AN_ORB);
	}
}

void doActions()
{
	// Get the current tile's value
	// C->ASM ASM_DATA.WorkVal = TheMaze[ASM_DATA.PlayerPosition];
	asm("ldy %w", offsetof(_asm_data, PlayerPosition));	
	asm("lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
	asm("sta %w", offsetof(_asm_data, WorkVal));

	// Check for a power pickup action
	if (ASM_DATA.WorkVal & POWER)
	{
		ActionPickupPowerPill();
	}
}

void ResetLevelGUI()
{
	ASM_DATA.LastPlayerX = 255;
	ASM_DATA.LastPlayerY = 255;
	ASM_DATA.LastPlayerDirection = 255;
	ASM_DATA.LastCurrentObject = 255;
	ASM_DATA.LastJoystickDirection = 255;
	ASM_DATA.LastScore[0] = 255;
	ASM_DATA.LastScore[1] = 255;
	ASM_DATA.LastScore[2] = 255;
	ASM_DATA.LastScore[3] = 255;
	ASM_DATA.LastPowerLevel = 255;
	ASM_DATA.LastPowerPillsLeft = 255;
	ASM_DATA.LastGhostsLeft = 255;
	ASM_DATA.PowerBarMode = PlayerInventory.CurrentColor + 1;
}

void GenDemoLevel()
{
	// Generate a start screen maze
	byte temp;
	temp = ASM_DATA.LevelNr;			// Save the current level
	ASM_DATA.LevelNr = 0;

	ASM_DATA.LevelWidth = Levels[0].Width;
	ASM_DATA.LevelHeight = Levels[0].Height;
	srand(Levels[0].Seed);
	++Levels[0].Seed;
	++ASM_DATA.DemoSeed;

	generateMaze();
	PlaceObjects();
	ASM_DATA.LevelNr = temp;
}

// ========================================================
// Show the game start screen
// Generate a demo level and walk through it
// Allow the player to select the start level
void ShowStartScreen()
{
	byte delay = 1;
	byte takeStep = 1;
	byte stepsTaken = 0;

	byte prevConsole = 7;

	// Generate a start screen maze
	GenDemoLevel();

	// Copy the original 3D view
	memset((unsigned char *)FIXED_GAMESCREEN0, 0, 40*21*3);
	if (ASM_DATA.PageNr == 0)
	{
		ASM_DATA.PagePtr = (unsigned char *)FIXED_GAMESCREEN1;
	}
	else
	{	
		ASM_DATA.PagePtr = (unsigned char *)FIXED_GAMESCREEN0;
	}

	asm("jsr %w", FIXED_SHOWTHESTARTSCREEN);
	asm("jsr %w", FIXED_DRAWSELECTEDLEVELNR);
	asm("jsr %w", FIXED_DRAWSELECTEDDIFFICULTYLEVEL);

	asm("jsr %w", FIXED_INITMUSIC);
	wait();
	asm("jsr %w", FIXED_PLAYINTROMUSIC);
	// Fire will start the game
	while (1)
	{
		wait();
		if (ASM_DATA.Joystick0)
		{
			// Something is happening
			if (ASM_DATA.Joystick0 != ASM_DATA.PrevJoystick0)
			{
				ASM_DATA.PrevJoystick0 = ASM_DATA.Joystick0;
				if (ASM_DATA.Joystick0 & JOYSTICK_UP)
				{
					++ASM_DATA.LevelNr;
					if (ASM_DATA.LevelNr > MaxLevelCompleted)
						ASM_DATA.LevelNr = MaxLevelCompleted;
					asm("jsr %w", FIXED_DRAWSELECTEDLEVELNR);
				}
				else if (ASM_DATA.Joystick0 & JOYSTICK_DOWN)
				{
					--ASM_DATA.LevelNr;
					if (ASM_DATA.LevelNr == 0)
						++ASM_DATA.LevelNr;
					asm("jsr %w", FIXED_DRAWSELECTEDLEVELNR);
				}
				if (ASM_DATA.Joystick0 & JOYSTICK_LEFT)
				{
					if (ASM_DATA.Difficulty == 0)
						ASM_DATA.Difficulty = 2;
					else 
						--ASM_DATA.Difficulty;

					asm("jsr %w", FIXED_DRAWSELECTEDDIFFICULTYLEVEL);
				}
				if (ASM_DATA.Joystick0 & JOYSTICK_RIGHT)
				{
					// Select was pressed and released
					++ASM_DATA.Difficulty;
					if (ASM_DATA.Difficulty > 2)
						ASM_DATA.Difficulty = 0;

					asm("jsr %w", FIXED_DRAWSELECTEDDIFFICULTYLEVEL);
				}
				if (ASM_DATA.Joystick0 & JOYSTICK_FIRE)
					break;
			}
		}
		else {
			ASM_DATA.PrevJoystick0 = ASM_DATA.Joystick0;
		}

		ASM_DATA.WorkVal = PEEK(FIXED_CONSOL);
		if (ASM_DATA.WorkVal != 7)
		{
			prevConsole = ASM_DATA.WorkVal;
		}
		else {
			// Some button was release
			if (prevConsole != 7)
			{
				// Fire the action on up
				if ((byte)(prevConsole & 1) == 0)
				{
					// Start was pressed and released
					break;
				}
				if ((byte)(prevConsole & 2) == 0)
				{
					// Select was pressed and released
					++ASM_DATA.Difficulty;
					if (ASM_DATA.Difficulty >= 3)
						ASM_DATA.Difficulty = 0;

					asm("jsr %w", FIXED_DRAWSELECTEDDIFFICULTYLEVEL);
				}

			}
			prevConsole = ASM_DATA.WorkVal;
		} 
			

		// Wait for the next walking step
		--delay;
		if (delay != 0) 
			continue;
		delay = DEMO_SPEED;

		// Take a step
		walkTheHall();
		drawTheHall();
		if (ASM_DATA.CurrentObject != OBJECT_EXIT)
			asm("jsr %w", FIXED_RENDERPRESSFIRE);
		SwopPages();

		if (ASM_DATA.CurrentObject || stepsTaken == 30)
		{
			delay = 50;

			if (ASM_DATA.CurrentObject == OBJECT_EXIT || stepsTaken == 30)
			{
				GenDemoLevel();
				stepsTaken = 0;
				continue;
			}
		}

		// C->ASM ASM_DATA.WorkVal = TheMaze[ASM_DATA.PlayerPosition];
		asm("ldy %w", offsetof(_asm_data, PlayerPosition));	
		asm("lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
		asm("sta %w", offsetof(_asm_data, WorkVal));		
		if (takeStep) 
		{
			++stepsTaken;

			switch (ASM_DATA.PlayerDirection)
			{
				case DIRECTION_EAST:
				{
					if ((ASM_DATA.PlayerX + 1 < ASM_DATA.LevelWidth) && (ASM_DATA.WorkVal & EAST)) {
						++ASM_DATA.PlayerX;
						++ASM_DATA.PlayerPosition;
					}
					break;
				}
				case DIRECTION_SOUTH:
				{
					if (ASM_DATA.PlayerY + 1 < ASM_DATA.LevelHeight && (ASM_DATA.WorkVal & SOUTH)) {
						++ASM_DATA.PlayerY;
						ASM_DATA.PlayerPosition += ASM_DATA.LevelWidth;
					}
					break;
				}

				case DIRECTION_WEST:
				{
					if (ASM_DATA.PlayerX > 0 && (ASM_DATA.WorkVal & WEST)) {
						--ASM_DATA.PlayerX;
						--ASM_DATA.PlayerPosition;
					}
					break;
				}

				case DIRECTION_NORTH:
				{
					if (ASM_DATA.PlayerY > 0 && (ASM_DATA.WorkVal & NORTH)) {
						--ASM_DATA.PlayerY;
						ASM_DATA.PlayerPosition -= ASM_DATA.LevelWidth;
					}
					break;
				}
			}
			takeStep = 0;
		}
		else
		{
			// C->ASM ASM_DATA.WorkVal = TheMaze[ASM_DATA.PlayerPosition];
			//asm("ldy %w", offsetof(_asm_data, PlayerPosition));	
			//asm("lda (%w),y", offsetof(_asm_data, MazePtr));	// A = Maze[WorkPosition]
			//asm("sta %w", offsetof(_asm_data, WorkVal));

			switch (ASM_DATA.PlayerDirection)
			{
				case DIRECTION_EAST:
				{
					if (ASM_DATA.WorkVal & SOUTH)
					{
						++ASM_DATA.PlayerDirection;
						takeStep = 1;
					}
					else if (ASM_DATA.WorkVal & NORTH) 
					{
						--ASM_DATA.PlayerDirection;
						takeStep = 1;
					}
					else if (ASM_DATA.WorkVal & EAST)
					{
						takeStep = 1;
					}
					else
						++ASM_DATA.PlayerDirection;
					break;
				}
				case DIRECTION_SOUTH:
				{
					if (ASM_DATA.WorkVal & WEST) 
					{
						++ASM_DATA.PlayerDirection;
						takeStep = 1;
					}
					else if (ASM_DATA.WorkVal & EAST) 
					{
						--ASM_DATA.PlayerDirection;
						takeStep = 1;
					}
					else if (ASM_DATA.WorkVal & SOUTH)
					{
						takeStep = 1;
					}
					else
						++ASM_DATA.PlayerDirection;
					break;
				}
				case DIRECTION_WEST:
				{
					if (ASM_DATA.WorkVal & NORTH) 
					{
						++ASM_DATA.PlayerDirection;
						takeStep = 1;
					}
					else if (ASM_DATA.WorkVal & SOUTH)
					{
						--ASM_DATA.PlayerDirection;
						takeStep = 1;
					}
					else if (ASM_DATA.WorkVal & WEST)
					{
						takeStep = 1;
					}
					else
						++ASM_DATA.PlayerDirection;
					break;
				}
				case DIRECTION_NORTH:
				{
					if (ASM_DATA.WorkVal & EAST)
					{
						++ASM_DATA.PlayerDirection;
						takeStep = 1;
					}
					else if (ASM_DATA.WorkVal & WEST)
					{
						--ASM_DATA.PlayerDirection;
						takeStep = 1;
					}
					else if (ASM_DATA.WorkVal & NORTH)
					{
						takeStep = 1;
					}
					else
						++ASM_DATA.PlayerDirection;
					break;
				}
			}

			
		}

		ASM_DATA.PlayerDirection &= 3;
	}

	WaitForFireRelease();

	asm("jsr %w", FIXED_INITMUSIC);
	wait();

	//asm("jsr %w", FIXED_FADEOUTSTARTSCREEN);
}

void Render3DView()
{
	walkTheHall();
	drawTheHall();
	SwopPages();
	doActions();
	drawGui();
}

void DoPlayerMazeMovement()
{
	byte prevPosition;
	wait();
	if (ASM_DATA.Joystick0)
	{
		if (ASM_DATA.Joystick0 != ASM_DATA.PrevJoystick0)
		{
			ASM_DATA.PrevJoystick0 = ASM_DATA.Joystick0;
			ASM_DATA.JoystickDirection = GUI_ACTION_FACE_FORWARD;

			if (ASM_DATA.Joystick0 & JOYSTICK_FIRE)
			{
				// Draw the map
				mode = SHOW_MAP;
			}
			else
			{
				prevPosition = ASM_DATA.PlayerPosition;
				
				if (ASM_DATA.Joystick0 & JOYSTICK_RIGHT)
				{
					// Turn right
					++ASM_DATA.PlayerDirection;
					ASM_DATA.PlayerDirection &= 3;
					ASM_DATA.JoystickDirection = GUI_ACTION_FACE_RIGHT;
				}
				else if (ASM_DATA.Joystick0 & JOYSTICK_LEFT)
				{
					// Turn left
					--ASM_DATA.PlayerDirection;
					ASM_DATA.PlayerDirection &= 3;
					ASM_DATA.JoystickDirection = GUI_ACTION_FACE_LEFT;
				}

				// C->ASM ASM_DATA.WorkVal = TheMaze[ASM_DATA.PlayerPosition];
				asm("ldy %w", offsetof(_asm_data, PlayerPosition));	
				asm("lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
				asm("sta %w", offsetof(_asm_data, WorkVal));

				if (ASM_DATA.Joystick0 & JOYSTICK_UP)
				{
					// forward
					switch (ASM_DATA.PlayerDirection)
					{
						case DIRECTION_EAST:
						{
							if ((ASM_DATA.PlayerX + 1 < ASM_DATA.LevelWidth) && (ASM_DATA.WorkVal & EAST)) {
								++ASM_DATA.PlayerX;
								++ASM_DATA.PlayerPosition;
							}
							break;
						}
						case DIRECTION_SOUTH:
						{
							if (ASM_DATA.PlayerY + 1 < ASM_DATA.LevelHeight && (ASM_DATA.WorkVal & SOUTH)) {
								++ASM_DATA.PlayerY;
								ASM_DATA.PlayerPosition += ASM_DATA.LevelWidth;
							}
							break;
						}
						case DIRECTION_WEST:
						{
							if (ASM_DATA.PlayerX > 0 && (ASM_DATA.WorkVal & WEST)) {
								--ASM_DATA.PlayerX;
								--ASM_DATA.PlayerPosition;
							}
							break;
						}
						case DIRECTION_NORTH:
						{
							if (ASM_DATA.PlayerY > 0 && (ASM_DATA.WorkVal & NORTH)) {
								--ASM_DATA.PlayerY;
								ASM_DATA.PlayerPosition -= ASM_DATA.LevelWidth;
							}
							break;
						}
					}
				}
				else if (ASM_DATA.Joystick0 & JOYSTICK_DOWN) 
				{
					ASM_DATA.JoystickDirection = GUI_ACTION_FACE_BACK;
					// Backwards
					switch (ASM_DATA.PlayerDirection)
					{
						case DIRECTION_EAST:
						{
							if ((ASM_DATA.PlayerX > 0) && (ASM_DATA.WorkVal & WEST)) {
								--ASM_DATA.PlayerX;
								--ASM_DATA.PlayerPosition;
							}
							break;
						}
						case DIRECTION_SOUTH:
						{
							if (ASM_DATA.PlayerY > 0 && (ASM_DATA.WorkVal & NORTH)) {
								--ASM_DATA.PlayerY;
								ASM_DATA.PlayerPosition -= ASM_DATA.LevelWidth;
							}
							break;
						}
						case DIRECTION_WEST:
						{
							if (ASM_DATA.PlayerX + 1 < ASM_DATA.LevelWidth && (ASM_DATA.WorkVal & EAST)) {
								++ASM_DATA.PlayerX;
								++ASM_DATA.PlayerPosition;
							}
							break;
						}
						case DIRECTION_NORTH:
						{
							if (ASM_DATA.PlayerY + 1 < ASM_DATA.LevelHeight && (ASM_DATA.WorkVal & SOUTH)) {
								++ASM_DATA.PlayerY;
								ASM_DATA.PlayerPosition += ASM_DATA.LevelWidth;
							}
							break;
						}
					}
				}

				if (prevPosition != ASM_DATA.PlayerPosition)
				{
					// Position has changed
					POKE(SFX_CH2, SOUND_JUST_WALK_ONE_STEP);

					// If we are on an object then play the object sound
					if (TheMaze[ASM_DATA.PlayerPosition] & OBJECT)
					{
						if (ASM_DATA.InteractingWithThisObjPtr->Type <= OBJECT_GHOST_PANDA)
						{
							// On a GHOST
							POKE(SFX_CH1, SOUND_MENU_OPEN_ACTIONS);
							POKE(SFX_CH2, SOUND_MENU_OPEN_ACTIONS);
						}
						else
						{
							POKE(SFX_CH1, SOUND_ON_TWO_CHANNELS);
							POKE(SFX_CH2, SOUND_ON_TWO_CHANNELS);
						}
					}
				}
			}
		}
	}
	else {
		ASM_DATA.PrevJoystick0 = ASM_DATA.Joystick0;
		mode = SHOW_3D;
	}
}

void WaitForFireOrEsc()
{
	while (1)
	{
		wait();
		if (PEEK(FIXED_CH) == 28)
		{
			// Yes then clear the key press and exit the game loop
			POKE(FIXED_CH, 255);
			break;
		}
		if (ASM_DATA.Joystick0 & JOYSTICK_FIRE)
		{
			WaitForFireRelease();
			break;
		}
	}
}

// A switch has been hit
// Lets change the walls on this tile
void ActionSwitch()
{
	// C->ASM ASM_DATA.WorkVal = TheMaze[ASM_DATA.PlayerPosition];
	asm("ldy %w", offsetof(_asm_data, PlayerPosition));	
	asm("lda (%w),y", offsetof(_asm_data, MazePtr));
	asm("sta %w", offsetof(_asm_data, WorkVal));

	if (ASM_DATA.PlayerY > 0)
	{
		// Can change the north direction
		ASM_DATA.WorkVal ^= NORTH;
		// Go to the maze position one line north and flip its south direction
		// C->ASM TheMaze[(byte)(ASM_DATA.PlayerPosition - ASM_DATA.LevelWidth)] ^= SOUTH;
		asm("lda %w", offsetof(_asm_data, PlayerPosition));	
		asm("sec");
		asm("sbc %w", offsetof(_asm_data, LevelWidth));
		asm("tay");
		asm("lda (%w),y", offsetof(_asm_data, MazePtr));
		asm("eor #%b", (byte)SOUTH);
		asm("sta (%w),y", offsetof(_asm_data, MazePtr));
	}
	if (ASM_DATA.PlayerX > 0)
	{
		// Can change the west direction
		ASM_DATA.WorkVal ^= WEST;
		// Go to the maze position one pos WEST and flip its EAST direction
		// C->ASM TheMaze[ASM_DATA.PlayerPosition - 1] ^= EAST;
		asm("ldy %w", offsetof(_asm_data, PlayerPosition));	
		asm("dey");
		asm("lda (%w),y", offsetof(_asm_data, MazePtr));
		asm("eor #%b", (byte)EAST);
		asm("sta (%w),y", offsetof(_asm_data, MazePtr));
	}
	if (ASM_DATA.PlayerY < ASM_DATA.LevelHeight-1)
	{
		// Can change the north direction
		ASM_DATA.WorkVal ^= SOUTH;
		// Go to the maze position one line south and flip its north direction
		// C->ASM TheMaze[ASM_DATA.PlayerPosition + ASM_DATA.LevelWidth] ^= NORTH;
		asm("lda %w", offsetof(_asm_data, PlayerPosition));	
		asm("clc");
		asm("adc %w", offsetof(_asm_data, LevelWidth));
		asm("tay");
		asm("lda (%w),y", offsetof(_asm_data, MazePtr));
		asm("eor #%b", (byte)NORTH);
		asm("sta (%w),y", offsetof(_asm_data, MazePtr));
	}
	if (ASM_DATA.PlayerX < ASM_DATA.LevelWidth-1)
	{
		// Can change the west direction
		ASM_DATA.WorkVal ^= EAST;
		// Go to the maze position one pos EAST and flip its WEST direction
		// C->ASM TheMaze[ASM_DATA.PlayerPosition + 1] ^= WEST;
		asm("ldy %w", offsetof(_asm_data, PlayerPosition));	
		asm("iny");
		asm("lda (%w),y", offsetof(_asm_data, MazePtr));
		asm("eor #%b", (byte)WEST);
		asm("sta (%w),y", offsetof(_asm_data, MazePtr));
	}

	ASM_DATA.WorkVal |= OBJECT_DONE;
	TheMaze[ASM_DATA.PlayerPosition] = ASM_DATA.WorkVal;

	// Switch the on to off or vice versa
	if (ASM_DATA.InteractingWithThisObjPtr->Type == OBJECT_SWITCH_OFF)
		ASM_DATA.InteractingWithThisObjPtr->Type = OBJECT_SWITCH_ON;
	else
		ASM_DATA.InteractingWithThisObjPtr->Type = OBJECT_SWITCH_OFF;

	POKE(SFX_CH1, SOUND_HIT_SWITCH);
	POKE(SFX_CH2, SOUND_HIT_SWITCH);
}

void ActionExplosion()
{
	// Check if there is a ghost or chest at this location
	// C->ASM ASM_DATA.WorkVal = ASM_DATA.MazePtr[pos];
	asm("ldy %v", pos);
	asm("lda (%w),y", offsetof(_asm_data, MazePtr));
	asm("sta %w", offsetof(_asm_data, WorkVal));

	if (ASM_DATA.WorkVal & OBJECT)
	{
		// There is something here
		// Find out what it is
		ASM_DATA.WorkVal = 0;		// This is the type of object to draw (Ghost, Chest etc [1-10])
		for (ASM_DATA.LoopJ = 0; ASM_DATA.LoopJ < ASM_DATA.ObjectCount; ++ASM_DATA.LoopJ)
		{
			ASM_DATA.InteractingWithThisObjPtr = &MazeObjects[ASM_DATA.LoopJ];

			if (pos == ASM_DATA.InteractingWithThisObjPtr->Location)
			{
				//ASM_DATA.WorkVal = ASM_DATA.InteractingWithThisObjPtr->Type;
				ASM_DATA.InteractingWithThisObjPtr->Data.Ghost.AttackCost = 0;
				break;
			}
		}
	}
}

void ActionBomb()
{
	if (PlayerInventory.Bombs > 0)
	{
		//byte pos;
		byte x;
		byte y;

		InteractionObject* savedPtr = ASM_DATA.InteractingWithThisObjPtr;

		pos = ASM_DATA.PlayerPosition;
		x = ASM_DATA.PlayerX;
		y = ASM_DATA.PlayerY;

		// Show the 3D view
		POKE(SFX_CH1, SOUND_OPEN_CHEST);
		walkTheHall();
		drawTheHall();
		SwopPages();
		WaitFor30CyclesNoShortcut();

		// Determine how far you are going to blow the wall
		ASM_DATA.LengthL = (rand() & 3) + 1; // 1,2,3,4

		// Remove the bomb from the inventory
		--PlayerInventory.Bombs;

		// Always blow in the facing direction
		// PlayerDirection - 0=East, 1=South, 2=West, 3=North
		// PlayerPosition
		ActionExplosion();

		for (ASM_DATA.LoopI = ASM_DATA.LengthL; ASM_DATA.LoopI; --ASM_DATA.LoopI)
		{
			switch (ASM_DATA.PlayerDirection)
			{
				case DIRECTION_EAST:
				{
					if (x < (byte)(ASM_DATA.LevelWidth-1))
					{
						// Go east, so open east wall
						asm("ldy %v", pos);
						asm("lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
						asm("ora #%b", EAST);
						asm("sta (%w),y", offsetof(_asm_data, MazePtr));
						asm("iny");
						asm("inc %v", pos);
						asm("lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
						asm("ora #%b", WEST);
						asm("sta (%w),y", offsetof(_asm_data, MazePtr));
						asm("inc %v", x);
					}
					break;
				}
				case DIRECTION_WEST:
				{
					if (x > 0)
					{
						// TheMaze[pos] |= WEST;
						// --pos;
						// TheMaze[pos] |= EAST;
						// --x;
						// Converted to ASM
						asm("ldy %v", pos);
						asm("lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
						asm("ora #%b", WEST);
						asm("sta (%w),y", offsetof(_asm_data, MazePtr));
						asm("dey");
						asm("dec %v", pos);
						asm("lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
						asm("ora #%b", EAST);
						asm("sta (%w),y", offsetof(_asm_data, MazePtr));
						asm("dec %v", x);
					}
					break;
				}
				case DIRECTION_NORTH:
				{
					if (y > 0)
					{
						// Go east, so open east wall
						// TheMaze[pos] |= NORTH;
						// pos -= ASM_DATA.LevelWidth;
						// TheMaze[pos] |= SOUTH;
						// --y;

						asm("ldy %v", pos);
						asm("lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
						asm("ora #%b", NORTH);
						asm("sta (%w),y", offsetof(_asm_data, MazePtr));
						// Fixed the current pos
						// Move one line up
						pos -= ASM_DATA.LevelWidth;
						// Fix the line
						asm("ldy %v", pos);
						asm("lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
						asm("ora #%b", SOUTH);
						asm("sta (%w),y", offsetof(_asm_data, MazePtr));
						asm("dec %v", y);
					}
					break;
				}

				case DIRECTION_SOUTH:
				{
					if (y < (byte)(ASM_DATA.LevelHeight-1))
					{
						// Go south
						// TheMaze[pos] |= SOUTH;
						// pos += ASM_DATA.LevelWidth;
						// TheMaze[pos] |= NORTH;
						// ++y;

						asm("ldy %v", pos);
						asm("lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
						asm("ora #%b", SOUTH);
						asm("sta (%w),y", offsetof(_asm_data, MazePtr));
						// Fixed the current pos
						// Move one line up
						pos += ASM_DATA.LevelWidth;
						// Fix the line
						asm("ldy %v", pos);
						asm("lda (%w),y", offsetof(_asm_data, MazePtr));// A = Maze[WorkPosition]
						asm("ora #%b", NORTH);
						asm("sta (%w),y", offsetof(_asm_data, MazePtr));
						asm("dec %v", y);
					}
					break;
				}
			}

			POKE(SFX_CH1, SOUND_ATTACK);
			POKE(SFX_CH2, SOUND_ATTACK);

			walkTheHall();
			drawTheHall();
			SwopPages();
			WaitFor30CyclesNoShortcut();

			ActionExplosion();
		}
		ASM_DATA.InteractingWithThisObjPtr = savedPtr;
		do {
			wait();
			if (ASM_DATA.Joystick0 == 0)
				break;
		} while(1);
	}
}

void ActionVortexAnywhere()
{
	// Note that there was an object interaction at this location
	// C->ASM TheMaze[ASM_DATA.PlayerPosition] |= OBJECT_DONE;
	asm("ldy %w", offsetof(_asm_data, PlayerPosition));			// Y = PlayerPosition
	asm("lda (%w),y", offsetof(_asm_data, MazePtr));			// A = Maze[PlayerPosition]		
	asm("ora #%b", (byte)OBJECT_DONE);
	asm("sta (%w),y", offsetof(_asm_data, MazePtr));

	// Find a new location
	ASM_DATA.PlayerX = rand() % ASM_DATA.LevelWidth;
	ASM_DATA.PlayerY = rand() % ASM_DATA.LevelHeight;
	ASM_DATA.PlayerPosition = MultByWidth[ASM_DATA.PlayerY] + ASM_DATA.PlayerX;

	ASM_DATA.PlayerDirection = rand() & 3;

	POKE(SFX_CH1, SOUND_MENU_VORTEX);
	POKE(SFX_CH2, SOUND_MENU_VORTEX);
}

// Jump to exit tile
// or if at exit jump to the middle of the maze
void ActionVortexToEnd()
{
	// Fire the vortex
	if (PlayerInventory.Vortex > 0)
	{
		byte pos;
		--PlayerInventory.Vortex;

		// If we are on the exit position then warp to the middle of the maze
		if (ASM_DATA.PlayerPosition == ExitPosition)
		{
			ASM_DATA.PlayerX = ASM_DATA.LevelWidth >> 1;
			ASM_DATA.PlayerY = ASM_DATA.LevelHeight >> 1;
			pos = MultByWidth[ASM_DATA.PlayerY] + ASM_DATA.PlayerX;
		}
		else
		{
			// Find a new location
			ASM_DATA.PlayerX = ASM_DATA.LevelWidth - 1;
			ASM_DATA.PlayerY = ASM_DATA.LevelHeight - 1;
			pos = ExitPosition;
		}
		ASM_DATA.PlayerPosition = pos;
		ASM_DATA.PlayerDirection = DIRECTION_EAST;

		POKE(SFX_CH1, SOUND_VORTEX_TO_END);
		POKE(SFX_CH2, SOUND_VORTEX_TO_END);
	}
}

void PlayWalkingTheMaze()
{
	while(1)
	{
		// Check if ESC was pressed
		if (PEEK(FIXED_CH) == 28)
		{
			// Yes then clear the key press and exit the game loop
			POKE(FIXED_CH, 255);
			ASM_DATA.NextActionCommand = AFTER_MENU_COMMANDS_GIVE_UP;
			return;
		}
		
		if (mode == SHOW_3D) {
			Render3DView();
		}
		else {
			DoMapScreen();
			if (ASM_DATA.NextActionCommand)
			{
				switch (ASM_DATA.NextActionCommand) 
				{
					case AFTER_MENU_COMMANDS_EXIT:
					{
						return;
					}
					case AFTER_MENU_COMMANDS_VORTEX:
					{
						ActionVortexAnywhere();
						break;
					}

					case AFTER_MENU_COMMANDS_SWITCH:
					{
						// We hit a switch change the 4 directions on the node
						ActionSwitch();
						break;
					}

					case AFTER_MENU_COMMANDS_PLACE_BOMB:
					{
						// Blow up some walls
						ActionBomb();
						break;
					}

					case AFTER_MENU_COMMANDS_VORTEX_TO_END:
					{
						ActionVortexToEnd();
						break;
					}
				}

				ASM_DATA.NextActionCommand = 0;
			}
		}
		DoPlayerMazeMovement();
	}
}

void main()
{
	ZeroMyMemory();
	// Some basic ptr setup
	pos = 0;
	ASM_DATA.DemoSeed = 0;
	ASM_DATA.Difficulty = 0;			// 0 = Easy, 1 = Medium, 2 = Hard

	ASM_DATA.MazePtr = (byte *)FIXED_THEMAZE;
	ASM_DATA.PlayerInventoryPtr = (char*)&PlayerInventory;

	*((short*)FIXED_SETRANDPTR) = (short*)&rand;
	//memset((char*)ASM_DATA.PlayerInventoryPtr, 0, sizeof(PlayerInventory));

	//RemoveAtariBasic();
	SetupAsm();

	while(1)
	{
		// Game starts here with a full state reset
		ResetGame();

		ShowStartScreen();

		ASM_DATA.PageNr = 0;		// Show page 0, draw to page 1
		SwopPages();

		while(1)
		{
			// NB: w*h < 256
			//ASM_DATA.LevelNr = 1;
			ASM_DATA.LevelWidth = Levels[ASM_DATA.LevelNr].Width;
			ASM_DATA.LevelHeight = Levels[ASM_DATA.LevelNr].Height;
			//srand(Levels[ASM_DATA.LevelNr].Seed);
			srand(ASM_DATA.VBICounter);

			generateMaze();
			PlaceObjects();

			walkTheHall();
			drawTheHall();
			SwopPages();

			DrawLevelNr();
			ResetLevelGUI();
			drawGui();

			asm("jsr %w", FIXED_RENDERLEVELX);

			asm("jsr %w", FIXED_INITMUSIC);
			POKE(SFX_CH1, SOUND_ENTER_LEVEL);
			ShowGameScreen();
			mode = SHOW_3D;

			WaitFor60Cycles();

			ASM_DATA.NextActionCommand = 0;

			PlayWalkingTheMaze();

			// Going on to the next level or exit
			// But show the full map
			RevealFullMap();
			if (ASM_DATA.NextActionCommand == AFTER_MENU_COMMANDS_EXIT)
			{
				if (ASM_DATA.LevelNr > MaxLevelCompleted)
					MaxLevelCompleted = ASM_DATA.LevelNr;		// Remember what the last level was that we completed
				++Levels[ASM_DATA.LevelNr].Seed;
				++ASM_DATA.LevelNr;
				// Check for the end of the game
				if (ASM_DATA.LevelNr > NUM_LEVELS)
				{
					// Draw "The End"
					asm("jsr %w", FIXED_INITMUSIC);
					asm("jsr %w", FIXED_PLAYENDGAMEMUSIC);
					RenderTheEnd();
				}
				else
				{
					RenderLevelDone();
				}
			}
			if (ASM_DATA.NextActionCommand == AFTER_MENU_COMMANDS_GIVE_UP)
			{
				asm("jsr %w", FIXED_INITMUSIC);
				asm("jsr %w", FIXED_PLAYQUITLEVELMUSIC);
			}
			ShowMapScreen();
			WaitForFireOrEsc();
			asm("jsr %w", FIXED_INITMUSIC);

			// What is the last command
			if (ASM_DATA.NextActionCommand == AFTER_MENU_COMMANDS_GIVE_UP)
			{
				break;
			}
			if (ASM_DATA.LevelNr > NUM_LEVELS)
			{
				ASM_DATA.LevelNr = 1;
				break;
			}
		}
	}
}
