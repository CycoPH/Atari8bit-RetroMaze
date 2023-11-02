; Make sure that the offset values for the C data structures are correctly set
;typedef struct Inventory {
;	uint Keys;				// (2) How many keys 
;	uint Red;				// (2) How many red orbs
;	uint Green;				// (2) How many green orbs
;	uint Blue;				// (2) How many blue orbs
;	byte PowerLevel;		// (1) How much power (0-31), 32 => becomes an orb and level goes to 0
;	byte Bombs;				// (1) How many bombs in stock
;	byte Maps;				// (1) How many maps in stock
;	byte Vortex;			// (1) How many vortex jumps in stock
;	byte CurrentColor		// (1) 0 = Red, 1 = Green, 2 = Blue
;	byte dummy[3];			// (4) Empty
;} Inventory;
; zpPlayerInventoryPtr	= ($5E, $5F) ptr to player's inventory data (See C Inventory struct to layout)

PLAYER_INVENTORY_KEYS = 0
PLAYER_INVENTORY_RED = 2
PLAYER_INVENTORY_GREEN = 4
PLAYER_INVENTORY_BLUE = 6
PLAYER_INVENTORY_POWER_LEVEL = 8
PLAYER_INVENTORY_NUM_BOMBS = 9
PLAYER_INVENTORY_NUM_MAPS = 10
PLAYER_INVENTORY_NUM_VORTEX = 11
PLAYER_INVENTORY_CURRENT_COLOR = 12