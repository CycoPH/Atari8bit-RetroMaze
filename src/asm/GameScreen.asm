; ---------------------------------------------------------
; Everything here is about the GUI
; Display list
; 3 top rows with the gui
; ---------------------------------------------------------

	.local
; The GUI takes up the top three line in game mode
; Score: 7 digits
; Level: 2 digits
; Power: 32 levels in 8 digits
; Key counter
; Orb counter
; Alien or interaction object
; Direction indicator
; X and Y position indicators
GameGui:	; Length = 3 * 40
	.BYTE 51,35,47,50,37,26,144,144
	.BYTE 144,144,144,144,144,0,0,242
	.BYTE 243,0,0,244,245,0,234,158
	.BYTE 159,102,8,8,103,7,8,8
	.BYTE 8,8,100,8,8,8,8,9
	.BYTE 44,37,54,37,44,26,0,0
	.BYTE 0,0,0,0,0,0,0,240
	.BYTE 241,109,0,118,119,0,235,142
	.BYTE 143,10,0,0,11,10,56,26
	.BYTE 144,144,11,57,26,144,144,11
	.BYTE 48,47,55,37,50,27,1,1
	.BYTE 1,1,1,1,1,1,6,144
	.BYTE 144,144,0,144,144,0,108,28
	.BYTE 29,104,8,8,105,12,8,8
	.BYTE 8,8,101,8,8,8,8,13

; Where are the action points in the GUI
ScoreIndicator = GameGui+6
NEWSIndicator = GameGui+40+26
PlayerDirectionIndicator = GameGui+40+26
PlayerXIndicator = GameGui+40+32
PlayerYIndicator = GameGui+40+37
CurrentObjectIndicator = GameGui+22
LevelNrIndicator = GameGui+40+6
LevelWidthIndicator = GameGui+40+9
LevelHeightIndictor = GameGui+40+12
PowerBarIndicator = GameGui+40+40+6
PowerPillCounter = GameGui+80+15
GhostCounter = GameGui+80+19

ShowMapScreen:
	lda #2
_saveAndReturn:
	sta VBI_WhichScreen
	rts

ShowGameScreen0:
	lda #0
	jmp _saveAndReturn

ShowGameScreen1:
	lda #1
	jmp _saveAndReturn

; ---------------------------------------------------------
; Draw one or two digit level number
DrawLevelNr:
	; Level nr
	ldx zpLevelNr
	cpx #10
	bcc ?DrawSingleDigitLevel
	; 10s
	lda Div10Table,x
	clc
	adc #144
?drawLevelNr:	
	sta LevelNrIndicator
	; 1s
	lda Modulus10Table,x
	adc #144
	sta LevelNrIndicator+1
	rts
?DrawSingleDigitLevel:
	lda Modulus10Table,x
	adc #144
	sta LevelNrIndicator
	lda #0
	sta LevelNrIndicator+1
	rts

; ---------------------------------------------------------
; Draw the player X/Y position, the facing direction
; and the interaction object.
; Variables influencing what the GUI looks like:
; PlayerX, PlayerY
; PlayerDirection
; CurrentObject
; LastJoystickDirection
; PowerPillsLeft 0-255
DrawTopGui:
	; Draw player X position
	ldx zpPlayerX
	cpx zpLastPlayerX				; Check if there was an update
	beq ?noDrawPlayerX
	; Yup draw the new state
	stx zpLastPlayerX
	; 10s
	lda Div10Table,x
	clc
	adc #144
	sta PlayerXIndicator
	; 1s
	lda Modulus10Table,x
	adc #144
	sta PlayerXIndicator+1
?noDrawPlayerX:

	; Draw player Y position
	ldx zpPlayerY
	cpx zpLastPlayerY				; Check if there was an update
	beq ?noDrawPlayerY
	; Yup draw the new state
	stx zpLastPlayerY
	; 10s
	lda Div10Table,x
	clc
	adc #144
	sta PlayerYIndicator
	; 1s
	lda Modulus10Table,x
	adc #144
	sta PlayerYIndicator+1
 ?noDrawPlayerY

	; Draw player facing direction
	lda zpPlayerDirection
	cmp zpLastPlayerDirection		; Check if there was an update
	beq ?noDrawPlayerDirection
	; Yup draw the new state
	sta zpLastPlayerDirection
	asl
	clc
	adc #248		; E->
	sta PlayerDirectionIndicator
	adc #1
	sta PlayerDirectionIndicator+1
?noDrawPlayerDirection:	

	; -----------------------------------------------------
	; Draw player interaction object
	lda #<CurrentObjectIndicator
	sta zpScreenPtr
	lda #>CurrentObjectIndicator
	sta zpScreenPtr+1

	; What object are we interfacing with
	; 0 = nothing -> draw the head with direction indicator
	; 1,2,3,4,5 -> draw ghost
	; 6 => Chest
	; 7 => Vortex
	; 8,9 => Switch
	; 10 => Exit
	ldx zpCurrentObject
	beq ?noInteraction

	; Check if there was an update
	cpx zpLastCurrentObject
	beq ?DoneWithDrawingTheActionObject
	stx zpLastCurrentObject			; Note the last object to be drawn

	; Interacting with something
	; Draw the interaction object
	dex		; To adjust to the GuiInteractionObj array

	lda GuiInteractionObjL,x		; (zpFromPtr) = GuiInteractionObj[CurrentObject-1]
	sta zpFromPtrL
	lda GuiInteractionObjH,x
	sta zpFromPtrH

	jmp ?ActiveGuiDrawEntryPoint

	; -----------------------------------------------------
	; Not interacting with anything
	; Draw the direction the joystick was last pushed into
?noInteraction:
	cpx zpLastCurrentObject
	beq ?prevAlsoNothingToDo
	stx zpLastCurrentObject
	ldx zpJoystickDirection
	jmp ?DrawTheHeadDirection

?prevAlsoNothingToDo:
	ldx zpJoystickDirection
	cpx zpLastJoystickDirection
	beq ?DoneWithDrawingTheActionObject
	; Draw the new direction
?DrawTheHeadDirection:
	stx zpLastJoystickDirection

	lda GuiFacingL,x		; (zpFromPtr) = GuiFacing[zpJoystickDirection]
	sta zpFromPtrL
	lda GuiFacingH,x
	sta zpFromPtrH

?ActiveGuiDrawEntryPoint:

	; Draw the first line
	ldy #0
	ldx #3
?DrawFaceLoop:
	lda (zpFromPtr),y
	sta (zpScreenPtr),y
	iny
	lda (zpFromPtr),y
	sta (zpScreenPtr),y
	iny
	lda (zpFromPtr),y
	sta (zpScreenPtr),y
	iny
	dex
	beq ?DoneWithDrawingTheActionObject
	; Move ScreenPtr to next line (minus 3 bytes)
	clc
	lda zpScreenPtr
	adc #37
	sta zpScreenPtr
	bcc ?noHighStep
	inc zpScreenPtr+1
?noHighStep
	jmp ?DrawFaceLoop

?DoneWithDrawingTheActionObject:

	jsr DrawThePowerBar


DrawThePowerPillLeft:
	; Draw the number of power pills left in the level
	; Packed into a single byte
	lda zpPowerPillsLeft
	cmp zpLastPowerPillsLeft
	beq ?skipDrawingOfPowerPillCounts
	sta zpLastPowerPillsLeft
	; Check if its over 200
	cmp #200
	bcs ?over200
	; Check if its over 100
	cmp #100
	bcs ?over100
	; Must be below 100
	tax
	lda #144		; Blue 0
	bne ?draw200s

?over200:
	sec
	sbc #200
	tax
	lda #146		; Blue 2
	bne ?draw200s

?over100:
	sec
	sbc #100
	tax
	lda #145		; Blue 1

?draw200s:
	sta PowerPillCounter		; hundreds counter
	; X is the 0-99 part now
	; 10s
	lda Div10Table,x
	clc
	adc #144
	sta PowerPillCounter+1
	; 1s
	lda Modulus10Table,x
	adc #144
	sta PowerPillCounter+2

?skipDrawingOfPowerPillCounts:

	;rts

DrawTheGhostsLeft:
	; Draw the number of ghosts left in the level
	; Packed into a single byte
	lda zpGhostsLeft
	cmp zpLastGhostsLeft
	beq ?skipDrawingOfGhostCounts
	sta zpLastGhostsLeft
	tax
	; X is the 0-99 part now
	; 10s
	lda Div10Table,x
	clc
	adc #144
	sta GhostCounter
	; 1s
	lda Modulus10Table,x
	adc #144
	sta GhostCounter+1

?skipDrawingOfGhostCounts:

	rts


WhichBoostChar	.byte 1,2,3,4


DrawThePowerBar:
	; Draw the power bar
	; If current <> zpLastPowerLevel then render
	; CurrentPowerLevel = (zpPlayerInventoryPtr), PLAYER_INVENTORY_POWER_LEVEL
	ldy #PLAYER_INVENTORY_POWER_LEVEL
	lda (zpPlayerInventoryPtr),y			; A = PlayerInventory.PowerLevel

	cmp zpLastPowerLevel
	beq ?noPowerBarChange
	sta zpLastPowerLevel

	tay				; Y = Power level
	ldx #0			; start drawing with the first char
?nextPowerCharLevel	
	cpy #4
	bcc ?endPhase
	; > 4 so draw full bar
	lda #POWER_BAR_4_CHAR				; full bar segment (4 slots)
?drawBoostChar
	sta PowerBarIndicator,x		; draw the char
	inx						; move to next spot
	; take 4 off the boost level
	dey
	dey
	dey
	dey
	bpl ?noReset
	ldy #0
?noReset
	; Are there more chars to draw?
	cpx #8
	bcc ?nextPowerCharLevel
	bcs ?noPowerBarChange

?endPhase
	; Not a full bar, 0,1,2,3
	lda WhichBoostChar,y		; This is the char to draw
	jsr ?drawBoostChar

?noPowerBarChange:
	rts

	.local
FadeOutGameScreen:
	ldx #10
?nextFadeOutStep:
	ldy #1
?miniLoop:	
	jsr WaitForVBI		; Clobbers Acc
	dey
	bpl ?miniLoop
	stx GameScreenLumaPtr
	dex
	bpl ?nextFadeOutStep
	rts

FadeInGameScreen:
	ldx #0
?nextFadeInStep:
	ldy #1
?miniInWaitLoop:	
	jsr WaitForVBI		; Clobbers Acc
	dey
	bpl ?miniInWaitLoop
	stx GameScreenLumaPtr
	inx
	cpx #11
	bcc ?nextFadeInStep

	rts


ShowTheGameScreen:
	jsr WaitForVBI

	;lda #0
	;sta NMIEN
	lda #1
	sta zpVbiMode

	; Setup display list in shadow reg, VBI will activate it
	;lda #<GameDisplayList
	;sta SDLSTL
	;lda #>GameDisplayList
	;sta SDLSTL+1

	;lda #$C0
	;sta NMIEN

	jsr WaitForVBI

	;jsr FadeInGameScreen

	rts
