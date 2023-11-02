; =============================================================================
; Location of various areas
; =============================================================================

PLAYER 			= $8B00		; Where is the RMT player to be located
MUSIC_DATA 		= $9000		; Where are the music and the sound effects located

STEREOMODE	= 0

; General game constants to be used in assembler go here

POWER_BAR_4_CHAR = 5
POWER_BAR_3_CHAR = 4
POWER_BAR_2_CHAR = 3
POWER_BAR_1_CHAR = 2
POWER_BAR_EMPTY_CHAR = 1

CHEST_ITEM_TYPE_NONE = 0
CHEST_ITEM_TYPE_ORB_RED = 1
CHEST_ITEM_TYPE_ORB_GREEN = 2
CHEST_ITEM_TYPE_ORB_BLUE = 3
CHEST_ITEM_TYPE_KEY = 4
CHEST_ITEM_TYPE_BOMB = 5
CHEST_ITEM_TYPE_MAP = 6
CHEST_ITEM_TYPE_VORTEX = 7

SECTION_C_ITEM_TYPE_NONE			= 0
SECTION_C_ITEM_TYPE_ORB_RED			= 1
SECTION_C_ITEM_TYPE_ORB_GREEN		= 2
SECTION_C_ITEM_TYPE_ORB_BLUE		= 3
SECTION_C_ITEM_TYPE_KEY				= 4
SECTION_C_ITEM_TYPE_BOMB			= 5
SECTION_C_ITEM_TYPE_MAP				= 6
SECTION_C_ITEM_TYPE_VORTEX			= 7
SECTION_C_ITEM_TYPE_POWER_BAR		= 8
SECTION_C_ITEM_TYPE_GIVE_TAKE		= 9