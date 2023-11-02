; Check in the maze65.map file for "__ZP_LAST__"
; the ASM zero-page variables should start after that

; Some hardcoded values to transfer data between C and ASM
; Match up with _asm_data in the main C file
zpPlayerX 				= $15
zpPlayerY 				= $16
zpLeftView				= $17 	; 18,19,1A
zpRightView				= $1B	; 1C,1D,1E
zpMidView				= $1F	; 20,21,22
zpPlayerDirection 		= $23
zpDifficulty			= $24	; Difficulty level 0=easy, 1=mid, 2=hard
zpJoystickDirection		= $25	; 0=forward, 1=left, 2=right, 3=back
zpScreenPtr				= $26	; ($26,$27)
zpScreenPtrL	= $26
zpScreenPtrH	= $27
zpPagePtr				= $28	; ($28,$29)
zpPagePtrL	= $28
zpPagePtrH	= $29

zpActionPosition		= $2A	; $2A The position where the next action will take place

zpPageNr				= $2C	; Which page are we showing, will draw to the alternative one
zpLevelNr				= $2D	; $2D What level are we playing
zpLevelWidth			= $2E	; $2E Width of level
zpLevelHeight			= $2F	; $2F Height of level
zpPlayerPosition		= $30	; $30 where in the maze the player is located 0-255
zpMazePtr				= $31	; ($31,$32) Where in memory the maze data is located (256 bytes)


zpPowerPillsLeft		= $37	; How many power pill are there left to pickup in this level
zpWaitCounter			= $38	; Only every used to count VBI waits
zpGhostsLeft			= $39	; How many ghosts are left in the level
zpLastGhostsLeft		= $3A	; How many ghosts left on the last render

zpSectionCInverse		= $3B	; When 128 then the currently drawn section C item will be done in inverse
zpMenuTemp0				= $3C	; Temp var used in menu drawing
zpMenuTemp1				= $3D	; Temp var used in menu drawing
; Data about the object being interacted with
zpCurrentObject 				= $3E	; What object are we interactig with
zpInteractingWithThisObjPtr 	= $3F	; ptr to the object currently being interacted with &InteractionObject[]
zpInteractingWithThisObjPtrL 	= $3F
zpInteractingWithThisObjPtH 	= $40

zpWallDistance					= $43	; How far is the wall away from the player 0,1,2,3
zpActionDistance				= $44	; The distance to the next object

zpSectionCBox0Type				= $45	; What to draw into box 0 (0-7)
zpSectionCBox1Type				= $46	; What to draw into box 1 (0-7)
zpSectionCBox2Type				= $47	; What to draw into box 2 (0-7)
zpSectionCBox0Count				= $48	; How many items are there in box 0 (0-7)
zpSectionCBox1Count				= $49	; How many items are there in box 1 (0-7)
zpSectionCBox2Count				= $4A	; How many items are there in box 2 (0-7)

zpWorkX					= $50
zpWorkY					= $51
zpWorkI					= $52
zpWorkVal				= $55

zpPlayerInventoryPtr	= $5E;	($5E, $5F) ptr to player's inventory data (See C Inventory struct to layout)

zpPowerBarMode			= $68	; 0 - Orange, 1 = Red, 2 = Green, 3 = Blue, 4 = Gray
zpVbiMode				= $69	; 0 = Start screen, 1 = game screen

zpLastPlayerX			= $6B	; Last player X position used to draw the GUI
zpLastPlayerY			= $6C	; Last player Y position used to draw the GUI
zpLastPlayerDirection	= $6D	; Last direction the player was facing
zpLastCurrentObject		= $6E	; Last object to be draw in the indicator
zpLastJoystickDirection	= $6F	; Last joystick/alien direction indicator drawn
zpLastScore				= $70	; Score[4] $70-$73
zpLastPowerLevel		= $74	; Last power level drawn
zpLastPowerPillsLeft	= $76	; Last power pill counter drawn
zpVBICounter 			= $77	; Every VBI we count up. Is used to seed the RNG

; General copy pointers and a 16bit length indicator
zpFromPtr				= $57	; ($57,$58)
zpFromPtrL				= $57
zpFromPtrH				= $58

zpToPtr					= $59	; ($59,$5A)
zpToPtrL				= $59
zpToPtrH				= $5A

zpLength				= $5B
zpLengthL		= $5B	; ($5B, $5C) 16-bit length
zpLengthH		= $5C

zpByteParam0			= $65
zpByteParam1			= $66
zpByteParam2			= $67
zpByteParam3			= $7B

zpDemoSeed				= $7D	; What seed is used to generate the demo level. Each time a level is generated the seed is increased
zpPrevJoystick0			= $7E	; Previously processed joystick value
zpJoystick0				= $7F	; The last joystick state. 0 = nothing





; Zero-page for the RasterMusicPlayer
; $CB - $DD (19 bytes)	

z_Regs = $DE
zpScore			= z_Regs 	; $B0,1,2,3,4
zpScoreToAdd	= z_Regs+5	; $B5
zpSfx 			= z_Regs+6
zpSFX_Effect 	= z_Regs+7
zpSFX_Effect2 	= z_Regs+8