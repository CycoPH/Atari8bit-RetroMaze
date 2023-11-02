# Retro Maze

## Basic ideas ##
- Maze with max 256 blocks, 1 byte per location
- Maze dimensions:
  - Width
- each block indicates open direction
  - East = 1
  - South = 2
  - West = 4
  - North = 8
- Power pill at this location = 16
- Object at this location = 32
- [free bit = 64]
- Visibility indicator = 128
- The map layout is based on random numbers
  - srand(xyz) to init the same level every time

- Generate maze with width & height
- Start at top left
- End in bottom right
  - The end needs something to open
- Place an object at each dead-end
  - A place on the map where there is only one exit
  - Figure out what kind of object it is
    - Ghost
      - Bob
      - Cyco
      - Alice
      - Specula
      - Panda
    - Chest
      - Can contain items
        - Things for the backpack
        - Orbs
        - Keys
        - Map reveal
        - Bomb to cut though a wall and open up the maze

## Interface ##

Get dumped into a 3D drawn ASCII inspired maze. 

### Movement ###

- Push joystick up to move in the direction you are facing.
  - If the spot contains a power pill then absorb it
    - Increase power
    - Add score
- Push left to turn left
- Push right to turn right
- Pull back to move backwards (opposite to facing direction)

### Map/Action ###

- Press fire to switch into map mode
  - It will show a top-down view of the map
  - Always show the start and end locations
  - Fog-of-war reveals 3 spots in the view direction.
  - All traveled spots are visible

#### Action Gui ####

- 8 chars for the menu text
- 7 chars for the action graphic (box with 5x5)
- 19 chars for the contents of chest, pouch, ghost
- 6 chars key and orb counters

Show backpack below the map. Use up/down to scroll through action options.
Click to activate the action.
- Back - return to game (exit the map screen)
- Chest

Backpack Options depend on what is in the backpack or on the action tile:

- Back - Return to game
- Action
  - Chest
    - Search to display options
      - Orbs: Any, Red, Green, Blue
      - Keys
      - Maps
      - Bomb
      - Warp
    - Search takes a second or two making some sounds and converting the buttons on the 
      chest to other state.
	- The chest opens showing the "TAKE" option.
	- Click to take each of the items
	- On every click the item on the right moves over until all the items are taken
	- Items are added to the pouch
	- Moving up or down moved the action cursor
  - Ghost
    - Attack to get
      - Orbs: Any, Red, Green, Blue
    - Trade
      - Convert orbs of one color into another

Menu options
Back
Option for the current maze location
  > This can be chest, ghost, exit
Chest Actions
	Search
Ghost Actions
	Attack
	Trade
Exit Actions
	Unlock
	Exit

- Return
- Search
  - Take
    - Click, Click, Click
- Pouch
  - Click to open
  - Show the items in the pouch


# Map/Menu #
Hold Fire to switch into map/menu view.
	- Show the revealed map
	- Show the action item you are on: GHOST, CHEST
	- Show the command options
    	- >BACK< is always the first one.
    	- If you release the FIRE button then it will execute the BACK command and you return to the 3D view
    	- Use up/down to move to the next menu option


List of commands:
BACK		- Always listed
ATTACK		- If on GHOST
TRADE		- If on GHOST and have tradable items then release to open menu of items. Hit fire to action usable item.
UNLOCK		- If on CHEST and CHEST is locked and requires KEY and you have a KEY, take away one lock. max 4 keys
UNLOCK		- If on CHEST and CHEST is locked and requires ORBS and you have an ORB, take away one orb. max 4 orbs
POWER		- If on CHEST and CHEST is locked and requires POWER and you have power, take away one power, max 8 power
TAKE		- If on CHEST and CHEST is unlocked
WARP		- If on VORTEX and you have POWER then random number of power used to warp to unseen location. If all locations seen then warp to END
SWITCH		- If on SWITCH and you have POWER then use 1 POWER to INVERT the 4 direction on the tile you are on.
UNLOCK		- If on EXIT and EXIT is locked and requires KEY and you have a KEY, take away one lock. max 4 keys
UNLOCK		- If on EXIT and EXIT is locked and requires ORBS and you have an ORB, take away one orb. max 4 orbs
POWER		- If on EXIT and EXIT is locked and requires POWER and you have power, take away one power, max 8 power
EXIT		- If on EXIT and EXIT is unlocked then end the level and prepare for next one.
OPEN		- If on POUCH then release to open menu of items. Hit fire to action usable item.


## Menu options ##
No ACTION
BACK
OPEN
	Release on OPEN to open the POUCH.
	Left/Right to select the pouch entry.
		Back, Item 1, Item 2, Item 3
	Click to use the item

On GHOST, Nothing in POUCH
BACK
ATTACK
	Release on ATTACK. Will ATTACK if there are enough Red, Green, Blue, Any orbs in the POUCH OR POWER in the meter.

On GHOST, something in POUCH
BACK
ATTACK
TRADE
	Release on TRADE. Will show options depending on POUCH content and GHOST ability

On CHEST
BACK



=============================================================================
# Item data requirements #
GHOST has
	- location
	- type: Bob, Cyco, Alice, Specula, Panda
	- strength: need X to overcome it
    	- none
    	- power
    	- Any orbs (take from the highest counter)
    	- Red orbs
    	- Green orbs
    	- Blue orbs
        	- Once dead will give
          	- BOB:
            	- RED
              	- MAP
              	- KEY
          	- CYCO
            	- GREEN
            	- BOMB
            	- KEY
          	- ALICE
            	- BLUE
            	- KEY
            	- KEY
          	- SPECULA
            	- KEY
            	- MAP
            	- BOMB
          	- PANDA
            	- BOMB
            	- BOMB
            	- KEY

  	- trade:
    	- BOB: 
        	- 2xRED -> 1xGREEN
        	- 1xBLUE -> 4xRED
        	- 9xRed -> 1xKey
      	- CYCO
        	- 1xGreen -> 3xRed
        	- 4xGreen -> 1xKey
      	- ALICE
        	- 4xRED -> 1xBLUE
        	- 1xBLUE -> 3xGreen
        	- 2xBLUE -> KEY
      	- SPECULA
        	- Key -> 3x Blue
        	- blue -> 3x Green
        	- Green -> Red
        	- ANY -> KEY
      	- PANDA
        	- POWER -> RED
        	- POWER -> GREEN
        	- POWER -> BLUE
        	- ANY -> KEY



How to draw the menu options?
- There are 7 lines of text
- 3 lines for the current options
- 4 lines of extra text




## Memory layout ##
$CB - $DD 		RMT Player
$DE - $E6		Score/SFX

$2000 - $23FF	Backpack Font
$2400 - $4614	Assembler code and data

$4800 - $7AA5	C code

$8700 - $883E	UNZX5, Boot part 2, Boot display list

$8882 - $8F54	RMT Player
$9000 - $991F	Song and instruments

$A000 - $AED8	Maze, Game screen 1+2, Map screen

$B000 - $B3FF 	Game Font		- loaded by pre-loader
$B400 - $B7FF 	Map Font
$B800 - $BBFF	Gui Font
$BC00 - $BF48	Boot loader image



unzx5 $4615 - $4718