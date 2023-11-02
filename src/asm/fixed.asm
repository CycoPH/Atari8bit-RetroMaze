;
; Memory location	
.include "ANTIC.asm"
.include "GTIA.asm"
.include "OS.asm"
.include "PIA.asm"
.include "POKEY.asm"

.include "_DataStructDefinitions.asm"
.include "Constants.asm"

; =============================================================================
; ATARI Hardware
; =============================================================================
BASICF = $3f8

; =============================================================================
; Memory locations where we want things to go
; =============================================================================
; Zero Page
; $15,16,17,18,19,1a,1b,1c,1d,1e,1f - 11 bytes
; $20-2F - 16 bytes
; $82...
;
; $2000-$23FF	$400		Game font
; $2400-$27FF	$400		Map font
; $2800-$2BFF	$400		GUI font
; $2C00-$2FFF	$400		Backpack/Pouch font
; $2C00-$2C21	$22			Game GUI at top (3x40) mode 4
; $3000						DisplayList

; Atari Basic space
; $A000 - $BFFF
; $A000-$A0FF	$100		The generated maze
; $A100-$A2FF	$200		Maze generator HistoryX					--| Shared space
; $A100-$A217	$118 (280)	MapGuiArea temp when in the map view	--|
; $A300-$A4FF	$200		Maze generator HistoryY	(HistoryX and Y are only used during maze generation)
; $A300-					Maze objects[]
; $A500-$A847	@348		40x21 game screen 0
; $A848-$AB8F	@348		40x21 game screen 1
; $AB90-$AED7	@348		40x21 map screen
; $AED8-$BFFF				Free
	.include "ZeroPage.asm"

TheMaze 			= $A000
HistoryX 			= TheMaze + $100
HistoryY 			= HistoryX + $200
NextFreeSpace 		= HistoryY + $200
; Going to reuse the 1K HistoryX + Y space as the MapGui render and temp area
MapGuiArea			= HistoryX			; 280 bytes - Draw into and then copy to the real display
MapGuiAreaLength	= (40*7)
MapGuiMenuTextBuffer= MapGuiArea + MapGuiAreaLength
MapGuiMenuTextLength= (8*12)			; 8 chars per menu item, 10 lines + 2 for the highlight


GameScreen0		= $A500							; $A500 - 840 / $348
GameScreen1 	= GameScreen0 + (40*21)			; $A848 - 840 / $348
MapScreen 		= GameScreen1 + (40*21)			; $AB90
MapMenuScreen	= MapScreen + (40*14)			; 4ADC0 Where does the menu start on the map screen
MapEnd 			= MapScreen + (40*21)


	; The GameFont is already loaded by the preloader
	* = $B000
GameFont:
	;.include "GameFont.asm"	; $B000 - $B3FF @ $400
	* = $B400
	.include "MapFont.asm"		; $B400 - $B7FF @ $400
	.include "GuiFont.asm"		; $B800 - $BBFF @ $400

	* = $2000
	.include "BackpackFont.asm"	; $2000 - $23FF	@ $400

	.include "AllDLandDLIs.asm"		; All Display List Interrupts in one place
	.include "Delays.asm"
	.include "BootScreen.asm"
	.include "StartScreen.asm"
	.include "GameScreen.asm"
	.include "LevelScreen.asm"
	.include "ScoreManager.asm"
	.include "Colors.asm"
	.include "VBI.asm"
	.include "InteractionGui.asm"
	.include "DrawTheHall.asm"

ZeroMyMemory:
	; We use lots of zpRegisters and expect them to be 0 when we start
	; Clear ranges:
	; $15-$40
	lda #0
	ldx #($40-$15)
?zmmLoop15:
	sta $15,x
	dex
	bpl ?zmmLoop15
	; $43-$5F
	ldx #($5F-$43)
?zmmLoop43:
	sta $43,x
	dex
	bpl ?zmmLoop43

	ldx #($FF-$90)
?zmmLoop90:
	sta $90,x
	dex
	bpl ?zmmLoop90

	rts

SetupAsm:
	lda #1
	sta KBCODE				; Reset POKEY

	jsr ResetAudio

	lda #255
	sta 731				; NOCLIK = set to !0 to turn of the keyboard sound

	; The game is loaded and we need to setup some things
	; Init POKEY
	lda #3				; Enable keyboard debounce/scanning
	sta SKCTL
	lda #0
	sta AUDCTL
	;sta SDMCTL			; disable screen

	; Wait for 1 second more just to show the nice loading screen
	ldx #60
?waitForABit:
	jsr WaitForVBI
	dex
	bpl ?waitForABit

	lda #$00
	sta NMIEN

	; Setup display list in shadow reg, VBI will activate it
	lda #<BootDisplayList
	sta SDLSTL
	lda #>BootDisplayList
	sta SDLSTL+1

	lda #<DLIForBootScreen
	sta VDSLST
	lda #>DLIForBootScreen
	sta VDSLST+1	

	lda #$C0
	sta NMIEN

	; PAL/NTSC boot screen fix
	lda PAL
	cmp #$F		; Check for NTSC
	bne ?isPAL
	; NTSC
	lda #$1A+$10
	sta BootScreenColorFix

?isPAL
	jsr WaitForFireDownAndUp

	jsr WaitForVBI

	; Player has pressed the fire button now and we can start the game
	lda #$00
	sta NMIEN

	; Load deferred vertical blank address
	lda #0
	sta zpVbiMode			; start screen VBI

	ldx #0
?clear:
	sta GameScreen0,x		; $A500
	sta GameScreen0+$100,x	; $A600
	sta GameScreen0+$200,x	; $A700
	sta GameScreen0+$300,x	; $A800
	sta GameScreen0+$400,x	; $A900
	sta GameScreen0+$500,x	; $AA00
	sta GameScreen0+$600,x	; $AB00
	dex
	bne ?clear

	ldx #>VBI
	ldy #<VBI
	lda #7
	jsr SETVBV

	lda #>GuiFont		; This is the gui font for the top of the screen
	sta CHBAS

	lda #<StartDisplayList
	sta SDLSTL
	lda #>StartDisplayList
	sta SDLSTL+1

	lda #<DLIForStartScreen
	sta VDSLST
	lda #>DLIForStartScreen
	sta VDSLST+1

	lda #$C0
	sta NMIEN

	jsr DetectPALorNTSC

	rts

OldJoystickValue: .byte 0

; ---------------------------------------------------------
; Ready the joystick 0 value
; bit 0 = up
; bit 1 = down
; bit 2 = left
; bit 3 = right
; bit 4 = fire
ReadJoystick:
 	lda     STRIG0			; Joystick 0 tigger
	asl     a
	asl     a
	asl     a
	asl     a
	ora     STICK0			; add position information
	eor     #$1F
	cmp     OldJoystickValue
	beq		?NoJoystickChange
	sta		OldJoystickValue
	ldx 	#0
	stx		ATRACT			; Disable "attract mode"
?NoJoystickChange:
	sta		zpJoystick0
	rts 

	; $2c00
; Hang in this loop until one VBI has happend
WaitForVBI:
	lda RTCLOK60
?wait
	cmp RTCLOK60
	beq ?wait
	rts

WaitForFireRelease:
	; DEBUG
	;inc COLBK
	lda STRIG0
	beq WaitForFireRelease
	rts

Original3DView:
	.INCBIN "asm\Default3DView.dat"	; 840 bytes

; Allocate where the game screens go
; There are two screen.
; Draw into one then swop it into view
;GameScreen0 = *		;
;GameScreen1 = * + (40*21)
;MapScreen = * + 840 + 840
;GameScreenEnd = * + 840 + 840 + 840

; =========================================================
Ghost_BobXL:	; 8x7
	.BYTE 64,67,9,9,9,9,66,65
	.BYTE 10,81,82,0,0,81,82,10
	.BYTE 28,66,67,64,65,66,67,27
	.BYTE 10,0,0,3,4,0,0,10
	.BYTE 10,0,64,7,8,65,0,10
	.BYTE 66,65,66,108,109,67,64,67
	.BYTE 0,66,9,9,9,9,67,0
Ghost_BobM:	; 6x5
	.BYTE 64,67,9,9,66,65,28,81
	.BYTE 82,81,82,27,10,0,46,57
	.BYTE 0,10,66,65,46,57,64,67
	.BYTE 0,66,9,9,67,0
Ghost_BobS:	; 4x3
	.BYTE 64,9,9,65,10,81,82,10
	.BYTE 66,9,9,67

; =========================================================
Ghost_CycoXL:	; 8x7
	.BYTE 64,9,79,9,9,79,9,65
	.BYTE 10,81,82,0,0,81,82,10
	.BYTE 27,66,67,0,0,66,67,28
	.BYTE 66,65,50,120,121,53,64,67
	.BYTE 64,67,5,9,9,6,66,65
	.BYTE 66,65,35,5,6,36,64,67
	.BYTE 0,35,5,9,9,6,36,0
Ghost_CycoM:	; 6x5
	.BYTE 64,79,9,9,79,65,27,67
	.BYTE 81,82,66,28,10,0,43,44
	.BYTE 0,10,66,65,64,65,64,67
	.BYTE 0,66,67,66,67,0
Ghost_CycoS:	; 4x3
	.BYTE 64,9,9,65,10,81,82,10
	.BYTE 66,9,9,67

; =========================================================
Ghost_AliceXL:	; 8x7
	.BYTE 41,65,64,65,64,65,64,42
	.BYTE 64,28,83,84,83,84,27,65
	.BYTE 86,10,0,65,64,0,10,61
	.BYTE 0,10,0,66,67,0,10,0
	.BYTE 0,27,65,0,0,64,28,0
	.BYTE 0,27,66,9,9,67,28,0
	.BYTE 0,66,9,9,9,9,67,0
Ghost_AliceM:	;6x5
	.BYTE 65,64,65,64,65,64,28,83
	.BYTE 84,83,84,27,10,0,66,67
	.BYTE 0,10,27,9,5,6,9,28
	.BYTE 66,9,9,9,9,67
Ghost_AliceS:	;4x3
	.BYTE 10,103,104,10,27,5,6,28
	.BYTE 66,9,9,67

; =========================================================
Ghost_SpeculaXL:	;8x7
	.BYTE 64,65,0,0,0,0,64,65
	.BYTE 10,209,137,133,134,137,210,10
	.BYTE 10,138,214,138,138,189,138,10
	.BYTE 66,147,137,212,211,137,148,67
	.BYTE 0,128,128,192,193,128,128,0
	.BYTE 0,128,128,183,176,128,128,0
	.BYTE 0,58,128,139,140,128,59,0
Ghost_SpeculaM:	;6x5
	.BYTE 64,65,0,0,64,65,10,209
	.BYTE 210,209,210,10,66,147,212,211
	.BYTE 148,67,0,128,174,185,128,0
	.BYTE 0,58,139,140,59,0
Ghost_SpeculaS:	;4x3
	.BYTE 64,65,64,65,27,86,86,28
	.BYTE 66,9,9,67

; =========================================================
Ghost_PandaXL:	;8x7
	.BYTE 70,128,128,128,128,128,128,98
	.BYTE 128,209,210,128,128,209,210,128
	.BYTE 128,211,212,128,128,211,212,128
	.BYTE 128,128,128,174,185,128,128,128
	.BYTE 128,128,128,128,128,128,128,128
	.BYTE 128,128,139,236,237,140,128,128
	.BYTE 58,128,128,128,128,128,128,59
Ghost_PandaM:	; 6x5
	.BYTE 70,128,128,128,128,98,128,209
	.BYTE 210,209,210,128,128,211,212,211
	.BYTE 212,128,128,128,174,185,128,128
	.BYTE 58,128,139,140,128,59
Ghost_PandaS:	; 4x3
	.BYTE 70,128,128,98,128,241,241,128
	.BYTE 58,139,140,59

; =========================================================
; Chest
ChestXL:		; 8x6
	.BYTE 105,106,106,106,106,106,106,107
	.BYTE 10,0,0,0,0,0,0,10
	.BYTE 27,9,9,65,64,9,9,28
	.BYTE 10,0,0,66,67,0,0,10
	.BYTE 10,74,77,77,77,77,78,10
	.BYTE 35,9,9,9,9,9,9,36

ChestM:			; 6x4
	.BYTE 105,106,106,106,106,107,27,9
	.BYTE 108,109,9,28,10,0,0,0
	.BYTE 0,10,35,9,9,9,9,36

ChestS:			; 4x3
	.BYTE 105,106,106,107,27,108,109,28
	.BYTE 35,9,9,36

; =========================================================
VortexXL	; 8x7
	;.BYTE 17,5,9,9,9,9,6,18
	;.BYTE 10,11,15,0,0,16,12,10
	;.BYTE 10,0,0,69,33,0,0,10
	;.BYTE 10,0,37,124,125,62,0,10
	;.BYTE 10,0,110,126,127,63,0,10
	;.BYTE 10,13,0,34,68,0,14,10
	;.BYTE 21,12,0,0,0,0,11,22
	; 4x4
	.BYTE 0,69,33,0,37,124,125,62
	.BYTE 110,126,127,63,0,34,68,0

VortexM:	; 6x5
	; 2x2
	.BYTE 124,125,126,127

	;.BYTE 17,5,9,9,6,18,10,11
	;.BYTE 0,0,12,10,10,0,124,125
	;.BYTE 0,10,10,0,126,127,0,10
	;.BYTE 21,12,0,0,11,22

VortexS:	;4x3
	; 2x1
	.BYTE 87,88
	;.BYTE 17,9,9,18,10,87,88,10
	;.BYTE 21,0,0,22

; =========================================================
SwitchOffXL:	; 8x7
	.BYTE 41,9,9,9,9,9,9,42
	.BYTE 10,0,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,0,0,10
	.BYTE 10,74,77,77,220,221,114,10
	.BYTE 10,0,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,0,0,10
	.BYTE 23,9,9,9,9,9,9,24
SwitchOnXL:		; 8x7
	.BYTE 41,9,9,9,9,9,9,42
	.BYTE 10,0,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,0,0,10
	.BYTE 10,74,218,219,77,77,78,10
	.BYTE 10,0,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,0,0,10
	.BYTE 23,9,9,9,9,9,9,24
SwitchM:		; 6x5
	.BYTE 41,9,9,9,9,42,10,0
	.BYTE 0,0,0,10,10,0,94,95
	.BYTE 0,10,10,0,0,0,0,10
	.BYTE 23,9,9,9,9,24
SwitchS:		; 4x3
	.BYTE 41,9,9,42,10,94,95,10
	.BYTE 23,9,9,24

; =========================================================
ExitXL:	; 8x8
	.BYTE 0,0,72,73,118,119,0,0
	.BYTE 0,72,166,167,168,188,73,0
	.BYTE 72,128,128,200,118,119,201,73
	.BYTE 10,0,10,0,0,0,0,10
	.BYTE 10,104,10,0,100,98,0,10
	.BYTE 10,104,10,0,199,128,0,10
	.BYTE 76,14,10,0,199,128,0,10
	.BYTE 0,11,75,9,9,9,9,36
ExitM: ; 8x2
	.BYTE 0,48,9,9,9,9,55,0
	.BYTE 46,47,9,9,9,9,56,57
ExitS: ; 4x2
	.BYTE 0,115,116,0,46,47,56,57

; =========================================================
; GUI facing direction indicators @ 26x1 2x1
GuiEast:	.byte 248,249
GuiSouth:	.byte 250,251
GuiWest: 	.BYTE 252,253
GuiNorth:	.BYTE 254,255

; =========================================================
; GUI action indicators @ 21x0 3x3
; If no action then the last movement direction is given by the head
; Forward, left, right, back
; Otherwise the primary action is shown:
; - Ghost: attack to get items, or trade
; - Chest: search to find items, might have to unlock first
; - Vortex: to warp somewhere else in the map
; - Switch: to invert the open walls on the tile
; - Exit: end the map, might have to power up or unlock
GuiObjectHeadForward:	.BYTE 234,158,159,235,142,143,108,28,29		; 0 = Direction
GuiObjectHeadLeft:		.BYTE 234,187,159,235,142,143,108,28,29		; 1
GuiObjectHeadRight:		.BYTE 234,188,159,235,142,143,108,28,29		; 2
GuiObjectHeadBack:		.BYTE 234,189,159,235,142,143,108,28,29		; 3
GuiObjectGhost:			.BYTE 73,202,203,76,205,206,79,80,81
GuiObjectChest:			.BYTE 64,65,66,67,68,69,12,8,13
GuiObjectVortex:		.BYTE 91,92,93,94,223,96,97,98,99
GuiObjectSwitchOff:		.BYTE 82,83,84,70,32,72,88,89,90
GuiObjectSwitchOn:		.BYTE 82,83,84,70,71,72,88,89,90
GuiObjectExit:			.BYTE 82,83,84,10,0,87,88,89,90

GuiFacingL:				.byte <GuiObjectHeadForward, <GuiObjectHeadLeft, <GuiObjectHeadRight, <GuiObjectHeadBack
GuiFacingH:				.byte >GuiObjectHeadForward, >GuiObjectHeadLeft, >GuiObjectHeadRight, >GuiObjectHeadBack
GuiInteractionObjL		.byte <GuiObjectGhost,<GuiObjectGhost,<GuiObjectGhost,<GuiObjectGhost,<GuiObjectGhost
						.byte <GuiObjectChest,<GuiObjectVortex,<GuiObjectSwitchOff,<GuiObjectSwitchOn,<GuiObjectExit
GuiInteractionObjH		.byte >GuiObjectGhost,>GuiObjectGhost,>GuiObjectGhost,>GuiObjectGhost,>GuiObjectGhost
						.byte >GuiObjectChest,>GuiObjectVortex,>GuiObjectSwitchOff,>GuiObjectSwitchOn,>GuiObjectExit

; =============================================================================
; Binary to decimal conversion
; =============================================================================

; 100 entries that map an index 0-99 to 10x0, 10x1, 10x2, ..., 10x9
; Divide by 10 table
; 100 bytes
Div10Table .rept 100
	.byte [*-Div10Table]/10
	.endr

; 100 entries that map an index to the 1s counter
; entry = i % 10
; ATASM does not have a modulus operator
; 100 bytes
Modulus10Table .rept 100
	.byte [*-Div10Table]%%10
	.endr




	.include "music/music.asm"


ZZZZZ_ThisIsTheFixedEND
