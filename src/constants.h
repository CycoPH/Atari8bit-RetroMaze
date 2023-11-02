#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define byte unsigned char
#define uint unsigned int


#define NO_BOOT_LOADER 1

#ifdef DEMO_VERSION
#define MAX_LEVEL_UNLOCKED_AT_START 4
#define NUM_LEVELS 4
#else
#define MAX_LEVEL_UNLOCKED_AT_START 3
#define NUM_LEVELS 15
#endif


#define MAX_OBJECT_COUNT 30


#define MAX_HEIGHT 14

#define EAST 1
#define SOUTH 2
#define WEST 4
#define NORTH 8
#define POWER 16
#define OBJECT 32
#define OBJECT_DONE 64		// Set when the object at this location has finished interacting with you
#define VISITED 128

#define DIRECTIONS	(EAST|SOUTH|WEST|NORTH)		// 15
#define POSSIBLE_ACTIONS (POWER|OBJECT)			// 16+32

#define JOYSTICK_UP	1
#define JOYSTICK_DOWN 2
#define JOYSTICK_LEFT 4
#define JOYSTICK_RIGHT 8
#define JOYSTICK_FIRE 16

// Which direction is the player facing
#define DIRECTION_EAST 0
#define DIRECTION_SOUTH 1
#define DIRECTION_WEST 2
#define DIRECTION_NORTH 3

// What menu icon is to be drawn
#define ICON_BACK		0
#define ICON_GHOST		1
#define ICON_CHEST		2
#define ICON_VORTEX		3
#define ICON_SWITCH		4
#define ICON_EXIT		5
#define ICON_POUCH		6
#define ICON_CHEST_OPEN	7
#define ICON_TRADE		8

// What type of objects can be placed
#define OBJECT_NONE 			0
#define OBJECT_GHOST_BOB		1	// Can be placed
#define OBJECT_GHOST_CYCO		2	// Can be placed
#define OBJECT_GHOST_ALICE		3	// Can be placed
#define OBJECT_GHOST_SPECULA	4	// Can be placed
#define OBJECT_GHOST_PANDA		5	// Can be placed
#define OBJECT_CHEST			6
#define OBJECT_VORTEX			7	// Can be placed Chest convert to Vortex (to 2x likely for vortex)
#define OBJECT_SWITCH_OFF		8	// Can be placed
#define OBJECT_SWITCH_ON		9
#define OBJECT_EXIT				10
#define OBJECT_POUCH			11


// Each of the OBJECT_ items maps to a menu option
#define MENU_ACTION_BACK			0	// Nothing to do
#define MENU_ACTION_ATTACK_GHOST	1	// Attack and kill a ghost
#define MENU_ACTION_TAKE_GHOST		2	// Attack and kill a ghost
#define MENU_ACTION_TRADE_GHOST		3	// Trade with a ghost
#define MENU_ACTION_UNLOCK_CHEST	4	// Unlock a chest
#define MENU_ACTION_TAKE_CHEST		5	// Unlock a chest
#define MENU_ACTION_VORTEX			6	// Jump via a vortex
#define MENU_ACTION_SWITCH			7	// Hit a switch
#define MENU_ACTION_LEVEL_EXIT		8	// Exit the level
#define MENU_ACTION_OPEN_POUCH		9	// Show the pouch contents


// What action indicator to show in the GUI
#define GUI_ACTION_FACE_FORWARD	0
#define GUI_ACTION_FACE_LEFT	1
#define GUI_ACTION_FACE_RIGHT	2
#define GUI_ACTION_FACE_BACK	3

#define SHOW_3D 0
#define SHOW_MAP 1



// Characters that make up parts of the 3D view
// Solid far end of wall
#define SOLID_WALL_TL 41			// 41 9 9 9 9 42
#define SOLID_WALL_TR 42
#define SOLID_WALL_BL 23			// 23 9 9 9 9 24
#define SOLID_WALL_BR 24

#define WALL_CONNECT_H 9			// -
#define WALL_CONNECT_V 10			// |

// Chars used for the power pills at various distances
#define POWER_LEFT_AT_DIST_3		118
#define POWER_RIGHT_AT_DIST_3		119
#define POWER_LEFT_AT_DIST_2		120
#define POWER_RIGHT_AT_DIST_2		121
#define POWER_LEFT_LN1_AT_DIST_1	81
#define POWER_RIGHT_LN1_AT_DIST_1	82
#define POWER_LEFT_LN2_AT_DIST_1	83
#define POWER_RIGHT_LN2_AT_DIST_1	84

#define WALL_0_TL_OFFSET (3*40 + 6)		// Draw new chars (41, 9,9,9,9, 42) between these two points
#define WALL_0_TR_OFFSET (3*40 + 33)
#define WALL_0_HORIZ_WIDTH (33 - 6 - 2)

#define WALL_1_TL_OFFSET (6*40 + 12)
#define WALL_1_TR_OFFSET (6*40 + 27)
#define WALL_1_HORIZ_WIDTH (27 - 12 - 2)
#define WALL_1_CLEAR_LEFT (7 * 40 + 13)

#define WALL_2_TL_OFFSET (8*40 + 16)
#define WALL_2_TR_OFFSET (8*40 + 23)
#define WALL_2_HORIZ_WIDTH (23 - 16 - 2)
#define WALL_2_CLEAR_LEFT (9 * 40 + 17)

#define LEFT_EXIT_0_TOP_OFFSET (3*40)
#define LEFT_EXIT_0_BOTTOM_OFFSET (17*40)
#define LEFT_EXIT_1_TOP_OFFSET (4*40+7)
#define LEFT_EXIT_1_BOTTOM_OFFSET (14*40+6)
#define LEFT_EXIT_2_TOP_OFFSET (7*40 + 13)

#define RIGHT_EXIT_0_TOP_OFFSET (3*40 + 33)
#define RIGHT_EXIT_0_BOTTOM_OFFSET (17*40 + 33)
#define RIGHT_EXIT_1_TOP_OFFSET (6*40 + 27)
#define RIGHT_EXIT_2_TOP_OFFSET (8*40 + 23)

#define MAP_BLANK 0
#define MAP_WALL 128

#define DEMO_SPEED 20

// What type of item is required to attack a ghost
#define ATTACK_TYPE_NONE		0				// Ghost has no attack type, just take its stuff
#define ATTACK_TYPE_POWER		1				// Need power to attack (8)
#define ATTACK_TYPE_ORBS		2				// Need orbs to attack (4)
#define ATTACK_TYPE_KEYS		3				// Need keys to attack (4)
#define ATTACK_TYPE_RED			4				// Need RED orbs to attack
#define ATTACK_TYPE_GREEN		5				// Need GREEN orbs to attack
#define ATTACK_TYPE_BLUE		6				// Need BLUE orbs to attack


#define DIFFICULTY_LEVEL_EASY 0
#define DIFFICULTY_LEVEL_MEDIUM 1
#define DIFFICULTY_LEVEL_HARD 2


#define SFX_CH1		FIXED_ZPSFX_EFFECT			// Played on channel 3
#define SFX_CH2		FIXED_ZPSFX_EFFECT2			// Played on channel 0

#define SOUND_MAKE_AN_ORB			0
#define SOUND_MENU_OPEN_ACTIONS		3
#define SOUND_NO_ACTION				4
#define SOUND_USE_AN_ORB			5
#define SOUND_JUST_WALK_ONE_STEP	9
#define SOUND_MENU_CHANGE			9
#define SOUND_HIT_SWITCH			11
#define SOUND_MENU_EXIT				12
#define SOUND_PICKUP_AN_POWER_PILL	13
#define SOUND_MENU_REVEAL			14
#define SOUND_ATTACK				15
#define SOUND_LEVEL_SELECTOR		15
#define SOUND_NOT_ENOUGH_POWER		15
#define SOUND_GHOST_KILL			16
#define SOUND_MENU_VORTEX			17
#define SOUND_CHEST_TAKE			18
#define SOUND_VORTEX_TO_END			19

#define SOUND_CLOSE_TO_OBJECT		26
#define SOUND_OPEN_CHEST			28
#define SOUND_ON_TWO_CHANNELS		29
#define SOUND_ENTER_LEVEL			30

#define AFTER_MENU_COMMANDS_NONE	0
#define AFTER_MENU_COMMANDS_GIVE_UP	1			// Give up on level and go back to start screen
#define AFTER_MENU_COMMANDS_EXIT	2			// Finish the level
#define AFTER_MENU_COMMANDS_VORTEX	3			// Vortex to a new location
#define AFTER_MENU_COMMANDS_SWITCH	4
#define AFTER_MENU_COMMANDS_MENU_UP	5			// Move the normal menu up
#define AFTER_MENU_COMMANDS_MENU_DOWN 6			// Move the normal menu down
#define AFTER_MENU_COMMANDS_PLACE_BOMB 7		// Place a bomb
#define AFTER_MENU_COMMANDS_SHOW_MAP 8			// Show the whole map
#define AFTER_MENU_COMMANDS_VORTEX_TO_END 9		// Vortex to the end


// Chest unlock type configuration
// BITS: 0,1 = Type of lock
// 		 2,3 - Next lock type
//		 4,5 - Next lock type
//		 6,7 - Next lock type
// 0 = not locked
// 1 = Need Power to unlock
// 2 = Need ORBS to unlock
// 3 = Need KEYS to unlock
#define CHEST_LOCKED_TYPE_NONE		0				// Chest is not locked
#define CHEST_LOCKED_TYPE_POWER		1				// Need power to unlock (8)
#define CHEST_LOCKED_TYPE_ORBS		2				// Need orbs to unlock (4)
#define CHEST_LOCKED_TYPE_KEYS		3				// Need keys to unlock (4)

#define EXIT_LOCKED_TYPE_NONE		0				// EXIT is not locked
#define EXIT_LOCKED_TYPE_POWER		1				// Need power to unlock (8)
#define EXIT_LOCKED_TYPE_ORBS		2				// Need orbs to unlock (4)
#define EXIT_LOCKED_TYPE_KEYS		3				// Need keys to unlock (4)
#define EXIT_LOCKED_TYPE_RED		4				// Need RED orbs to unlock
#define EXIT_LOCKED_TYPE_GREEN		5				// Need GREEN orbs to unlock
#define EXIT_LOCKED_TYPE_BLUE		6				// Need BLUE orbs to unlock

#define MAKE_CHEST_LOCKS(a,b,c,d) (a + (b<<2) + (c<<4) + (d<<6))

// What items can be inside a chest
#define CHEST_ITEM_TYPE_NONE	 	0
#define CHEST_ITEM_TYPE_ORB_RED 	1
#define CHEST_ITEM_TYPE_ORB_GREEN	2
#define CHEST_ITEM_TYPE_ORB_BLUE	3
#define CHEST_ITEM_TYPE_KEY			4
#define CHEST_ITEM_TYPE_BOMB		5
#define CHEST_ITEM_TYPE_MAP			6
#define CHEST_ITEM_TYPE_VORTEX		7
#define NUM_POSSIBLE_CHEST_ITEMS	8

// What items can be used for trading
#define SECTION_C_ITEM_TYPE_NONE			0
#define SECTION_C_ITEM_TYPE_ORB_RED			1
#define SECTION_C_ITEM_TYPE_ORB_GREEN		2
#define SECTION_C_ITEM_TYPE_ORB_BLUE		3
#define SECTION_C_ITEM_TYPE_KEY				4
#define SECTION_C_ITEM_TYPE_BOMB			5
#define SECTION_C_ITEM_TYPE_MAP				6
#define SECTION_C_ITEM_TYPE_VORTEX			7
#define SECTION_C_ITEM_TYPE_POWER_BAR		8
#define SECTION_C_ITEM_TYPE_GIVE_TAKE		9
#define SECTION_C_ITEM_TYPE_RANDOM			10


#endif