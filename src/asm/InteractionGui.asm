; ---------------------------------------------------------
; Everything to do with the interaction gui at the bottom of the map
; ---------------------------------------------------------

; Menu text BACK
BackText 	.sbyte "BACK    "
BackTextInv .byte 159
			.sbyte +$80,"BACK"
			.byte 128,128,158

; Menu text ghost ATTACK
AttackText 		.sbyte "ATTACK  "
AttackTextInv	.byte 159
				.sbyte +$80,"ATTACK"
				.byte 158

; Menu text ghost TRADE
TradeText		.sbyte "TRADE   "
TradeTextInv	.byte 159
				.sbyte +$80,"TRADE"
				.byte 128,158

; Menu text OPEN
OpenText		.sbyte "OPEN    "
OpenTextInv		.byte 159
				.sbyte +$80,"OPEN"
				.byte 128,128,158

; Menu text POUCH
PouchText		.sbyte "POUCH   "
PouchTextInv	.byte 159
				.sbyte +$80,"POUCH"
				.byte 128,158

; UNLOCK text
UnlockText:		.sbyte "UNLOCK  "
UnlockTextInv:	.byte 159
				.sbyte +$80,"UNLOCK"
				.byte 158

; POWER
PowerText:		.sbyte "POWER   "
PowerTextInv:	.byte 159
				.sbyte +$80,"POWER"
				.byte 128,158

; TAKE
TakeText 	.sbyte "TAKE    "
TakeTextInv .byte 159
			.sbyte +$80,"TAKE"
			.byte 128,128,158

; WARP
WarpText:		.sbyte "WARP    "
WarpTextInv:	.byte 159
				.sbyte +$80,"WARP"
				.byte 128,128,158

; SWITCH
SwitchText 	.sbyte "SWITCH  "
SwitchTextInv .byte 159
			.sbyte +$80,"SWITCH"
			.byte 158

; Menu text EXIT
ExitText		.sbyte "EXIT    "
ExitTextInv		.byte 159
				.sbyte +$80,"EXIT"
				.byte 128,128,158

; This frame is hard-coded!
; 7x7 to be drawn @ 8,17 of the map screen
;WhatInteractionEmptyFrame:
;	.BYTE 68,65,65,65,65,65,69
;	.BYTE 63,0,0,0,0,0,63
;	.BYTE 63,0,0,0,0,0,63
;	.BYTE 63,0,0,0,0,0,63
;	.BYTE 63,0,0,0,0,0,63
;	.BYTE 63,0,0,0,0,0,63
;	.BYTE 70,65,65,65,65,65,71

; 5x5 to be drawn @ 9,18 inside the WhatInteractionEmptyFrame:
BackActionIcon:
	.BYTE 34,33,35,43,0,0,0,0
	.BYTE 0,0,0,26,27,0,0,0
	.BYTE 28,29,0,0,0,0,0,0
	.BYTE 0

; 5x5 @ 9,18
GhostActionIcon:
	.BYTE 39,40,47,51,52,0,0,0
	.BYTE 0,0,0,97,98,99,0,0
	.BYTE 100,101,102,103,0,104,105,106
	.BYTE 0

; 5x5 @ 9,18
ChestActionIcon:
	.BYTE 35,40,37,51,52,72,73,73
	.BYTE 73,74,75,65,77,65,76,63
	.BYTE 0,0,0,63,70,65,65,65
	.BYTE 71

; 5x5 @ 9,18
ChestIsOpenAction:
	.BYTE 59,65,65,65,60,63,0,0
	.BYTE 0,63,75,65,65,65,76,63
	.BYTE 125,126,15,63,70,65,65,65
	.BYTE 71	

; 5x5 @ 9,18
VortexActionIcon:
	.BYTE 55,33,50,48,0,59,65,65
	.BYTE 65,60,63,115,116,119,63,63
	.BYTE 117,118,120,63,61,65,65,65
	.BYTE 62

;5x5 @ 9,18
SwitchActionIcon
	.BYTE 44,37,54,37,50,0,68,65
	.BYTE 69,0,0,63,128,63,0,0
	.BYTE 63,0,63,0,0,70,65,71
	.BYTE 0

; 5x5 @ 9,18
ExitActionIcon:
	.BYTE 37,56,41,52,0,0,68,73
	.BYTE 69,0,0,63,0,76,0,0
	.BYTE 63,0,63,0,0,70,65,71
	.BYTE 0

; 5x5 @ 9,18
PouchActionIcon:
	.BYTE 48,47,53,35,40,0,0,0
	.BYTE 0,0,0,107,108,109,0,0
	.BYTE 110,111,112,0,0,113,105,114
	.BYTE 0

; 5x5 @ 9,18
TradeActionIcon:
	.BYTE 30,30,30,30,30,0,0,0
	.BYTE 0,0,52,50,33,36,37,0
	.BYTE 0,0,0,0,31,31,31,31
	.BYTE 31

InteractionIcons:
	; 0 = Back Icon
	; 1 = Ghost Icon
	; 2 = Chest Icon
	; 3 = Vortex
	; 4 = Switch
	; 5 = Exit
	; 6 = Pouch
	; 7 = Chest is open
	; 8 = Trading
	.WORD BackActionIcon, GhostActionIcon, ChestActionIcon, VortexActionIcon
	.WORD SwitchActionIcon, ExitActionIcon, PouchActionIcon, ChestIsOpenAction
	.WORD TradeActionIcon

; Section D - Player's Inventory closed view
; 7x7 with all the counters at 0
SectionDBlank:
	.BYTE 59,65,65,65,65,65,69
	.BYTE 63,64,0,16,16,16,63
	.BYTE 63,50,0,16,16,16,63
	.BYTE 124,39,0,16,16,16,63
	.BYTE 63,34,0,16,16,16,63
	.BYTE 63,127,0,16,16,16,63
	.BYTE 61,65,65,65,65,65,71

; -----------------------------------------------------------------------------
; SectionC
; Pouch/Chest/Contents indicator
; Blank: 19x7
SectionCBlank:
	.BYTE 59,65,65,65,65,65,66,65
	.BYTE 65,65,65,65,66,65,65,65
	.BYTE 65,65,66,63,0,0,0,0
	.BYTE 0,63,0,0,0,0,0,63
	.BYTE 0,0,0,0,0,63,63,0
	.BYTE 0,0,0,0,63,0,0,0
	.BYTE 0,0,63,0,0,0,0,0
	.BYTE 63,123,0,0,0,0,0,63
	.BYTE 0,0,0,0,0,63,0,0
	.BYTE 0,0,0,63,63,0,0,0
	.BYTE 0,0,63,0,0,0,0,0
	.BYTE 63,0,0,0,0,0,63,63
	.BYTE 0,0,0,0,0,63,0,0
	.BYTE 0,0,0,63,0,0,0,0
	.BYTE 0,63,61,65,65,65,65,65
	.BYTE 67,65,65,65,65,65,67,65
	.BYTE 65,65,65,65,67

; These are the items to be shown in the pouch/chest/section c
; 5x4 data
; last line is always the stock counter 1x or 11x or something like that
; 0
SectionCItemNone:
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 0,0,0

; 1
SectionCItemOrbRed:
	.BYTE 0,50,37,36,0,0,1,2
	.BYTE 3,0,0,4,50,5,0,0
	.BYTE 6,7,8,0

; 2
SectionCItemOrbGreen:
	.BYTE 39,50,37,37,46,0,1,2
	.BYTE 3,0,0,4,39,5,0,0
	.BYTE 6,7,8,0

; 3
SectionCItemOrbBlue:
	.BYTE 34,44,53,37,0,0,1,2
	.BYTE 3,0,0,4,34,5,0,0
	.BYTE 6,7,8,0

; 4
SectionCItemKey:
	.BYTE 0,43,37,57,0,0,0,0
	.BYTE 0,0,0,9,10,11,0,0
	.BYTE 0,0,0,0

; 5
SectionCItemBomb:
	.BYTE 34,47,45,34,0,0,88,89
	.BYTE 0,0,0,86,87,0,0,0
	.BYTE 84,85,0,0

;6
SectionCItemMap:
	.BYTE 0,45,33,48,0,0,0,0
	.BYTE 0,0,0,78,79,80,0,0
	.BYTE 81,82,83,0

; 7
SectionCItemVortex:
	.BYTE 54,47,50,52,56,0,0,0
	.BYTE 0,0,0,115,116,119,0,0
	.BYTE 117,118,120,0

; 8
SectionCItemPowerBarFull:
	.BYTE 48,47,55,37,50,0,0,0
	.BYTE 0,0,38,53,44,44,0,34
	.BYTE 33,50,0,0

; 9
SectionCItemGiveTake:
	.BYTE 39,41,54,37,31,0,0,0
	.BYTE 0,0,65,65,65,65,65,39
	.BYTE 37,52,0,30

; 10
SectionCItemRandom:
	.BYTE 33,46,57,0,0,41,52,37
	.BYTE 45,0,96,0,64,0,127,0
	.BYTE 0,0,0,0

SectionCItemSrcL:
	.byte <SectionCItemNone, <SectionCItemOrbRed, <SectionCItemOrbGreen, <SectionCItemOrbBlue
	.byte <SectionCItemKey, <SectionCItemBomb, <SectionCItemMap, <SectionCItemVortex
	.byte <SectionCItemPowerBarFull, <SectionCItemGiveTake, <SectionCItemRandom
SectionCItemSrcH:
	.byte >SectionCItemNone, >SectionCItemOrbRed, >SectionCItemOrbGreen, >SectionCItemOrbBlue
	.byte >SectionCItemKey, >SectionCItemBomb, >SectionCItemMap, >SectionCItemVortex
	.byte >SectionCItemPowerBarFull, >SectionCItemGiveTake, >SectionCItemRandom

.local

; ---------------------------------------------------------
; Section B
DrawInteractionIcon:
	; zpByteParam0 is the index to the items to draw
	lda zpByteParam0
	asl
	tax
	lda InteractionIcons,X
	sta zpFromPtrL
	lda InteractionIcons+1,X
	sta zpFromPtrH

	lda #<(MapMenuScreen+8+40+1)
	sta zpToPtrL
	lda #>(MapMenuScreen+8+40+1)
	sta zpToPtrH

	; Draw the top line
	lda #68					; Top left corner
	sta MapMenuScreen+8
	lda #65					; Top line
	sta MapMenuScreen+9
	sta MapMenuScreen+10
	sta MapMenuScreen+11
	sta MapMenuScreen+12
	sta MapMenuScreen+13
	lda #69					; Top right corner
	sta MapMenuScreen+14

	; Draw the bottom line
	lda #70					; bottom left corner
	sta MapMenuScreen+8+6*40
	lda #65					; Top line
	sta MapMenuScreen+9+6*40
	sta MapMenuScreen+10+6*40
	sta MapMenuScreen+11+6*40
	sta MapMenuScreen+12+6*40
	sta MapMenuScreen+13+6*40
	lda #71					; bottom right corner
	sta MapMenuScreen+14+6*40

	; Draw the left and right side
	lda #63
	sta MapMenuScreen+8+40
	sta MapMenuScreen+8+40+6
	sta MapMenuScreen+8+40+40
	sta MapMenuScreen+8+40+40+6
	sta MapMenuScreen+8+40+40+40
	sta MapMenuScreen+8+40+40+40+6
	sta MapMenuScreen+8+40+40+40+40
	sta MapMenuScreen+8+40+40+40+40+6
	sta MapMenuScreen+8+40+40+40+40+40
	sta MapMenuScreen+8+40+40+40+40+40+6

	ldy #0			; Index into the source data 5x5 source
	lda #5
	sta zpWorkY		; 5 lines counter
?NextLine:
	ldx #5			; x counter
?NextCharInLine:
	lda (zpFromPtr),y
	sta (zpToPtr),y
	iny
	dex
	bne ?NextCharInLine
	; Move (to) the next line
	clc
	lda zpToPtrL
	adc #35
	sta zpToPtrL
	bcc ?NoRollover
	inc zpToPtrH
?NoRollover:
	dec zpWorkY
	bne ?NextLine

	rts

; ---------------------------------------------------------
; Section D - Player Inventory
; Copy the section D Blank data and then draw the numbers into the space
;
.local
DrawPlayerInventory:
	lda #<SectionDBlank
	sta zpFromPtrL
	lda #>SectionDBlank
	sta zpFromPtrH

	lda #<(MapMenuScreen+33)
	sta zpToPtrL
	lda #>(MapMenuScreen+33)
	sta zpToPtrH

	ldy #0			; Index into the source data 7xx source
	lda #7
	sta zpWorkY		; 5 lines counter
?NextLine:
	ldx #7			; x counter
?NextCharInLine:
	lda (zpFromPtr),y
	sta (zpToPtr),y
	iny
	dex
	bne ?NextCharInLine
	; Move (to) the next line
	clc
	lda zpToPtrL
	adc #33
	sta zpToPtrL
	bcc ?NoRollover
	inc zpToPtrH
?NoRollover:
	dec zpWorkY
	bne ?NextLine

	rts


; =========================================================
; Render the 'Inventory' numbers

; ByteParam0, ByteParam1	L,H byte of number to draw
; (ToPtr) - location to draw at
; ByteParam0 (0-99) 1's and 10's
;			 (100 & 200) 1000's
.local
DrawNumberAtLocation
	ldy #0
	lda zpByteParam0
	cmp #200
	bcs ?over200
	cmp #100
	bcs ?over100

	; Must be below 100
	tax
	lda #16			; Blue 0
	bne ?draw200s

?over200:
	sec
	sbc #200
	tax
	lda #18			; Blue 2
	bne ?draw200s

?over100:
	sec
	sbc #100
	tax
	lda #17			; Blue 1	

?draw200s:
	sta (zpToPtr),y
	iny
	clc
	lda Div10Table,x
	adc #16
	sta (zpToPtr),y
	iny
	lda Modulus10Table,x
	adc #16
	sta (zpToPtr),y

	rts

; Draw2DigitNumberAtLocation(number to draw in A)
; (zpToPtr) is the location where the digits are drawn
; A = number to draw (0-100)
Draw2DigitNumberAtLocation
	tax
	clc
	; 10s
	lda Div10Table,x
	adc #16
	sta (zpToPtr),y
	; 1s
	iny
	lda Modulus10Table,x
	adc #16
	sta (zpToPtr),y

	rts	


; Draw the keys number
DrawNumKeysInPlayerInventory:
	lda #<(MapMenuScreen+36+40)		; 36,1
	sta zpToPtrL
	lda #>(MapMenuScreen+36+40)
	sta zpToPtrH

	ldy #PLAYER_INVENTORY_KEYS		; ByteParam0/1 = PlayerIntenvory->Keys
	lda (zpPlayerInventoryPtr), y
	sta zpByteParam0
	iny
	lda (zpPlayerInventoryPtr), y
	sta zpByteParam1
	jmp DrawNumberAtLocation		; Draw

; Draw the red orbs number
DrawNumRedOrbsInPlayerInventory:
	; # of red orbs
	lda #<(MapMenuScreen+36+2*40)	; 36,2
	sta zpToPtrL
	lda #>(MapMenuScreen+36+2*40)
	sta zpToPtrH

	ldy #PLAYER_INVENTORY_RED		; ByteParam0/1 = PlayerIntenvory->Red
	lda (zpPlayerInventoryPtr),y
	sta zpByteParam0
	iny
	lda (zpPlayerInventoryPtr),y
	sta zpByteParam1
	jmp DrawNumberAtLocation

; Draw the green orbs number
DrawNumGreenOrbsInPlayerInventory:
	; # of green orbs
	lda #<(MapMenuScreen+36+3*40)	; 36,3
	sta zpToPtrL
	lda #>(MapMenuScreen+36+3*40)
	sta zpToPtrH

	ldy #PLAYER_INVENTORY_GREEN		; ByteParam0/1 = PlayerIntenvory->Green
	lda (zpPlayerInventoryPtr),y
	sta zpByteParam0
	iny
	lda (zpPlayerInventoryPtr),y
	sta zpByteParam1
	jmp DrawNumberAtLocation

; Draw the blue orbs number
DrawNumBlueOrbsInPlayerInventory:
	; # of blue orbs
	lda #<(MapMenuScreen+36+4*40)	; 36,4
	sta zpToPtrL
	lda #>(MapMenuScreen+36+4*40)
	sta zpToPtrH

	ldy #PLAYER_INVENTORY_BLUE		; ByteParam0/1 = PlayerIntenvory->Blue
	lda (zpPlayerInventoryPtr),y
	sta zpByteParam0
	iny
	lda (zpPlayerInventoryPtr),y
	sta zpByteParam1
	jmp DrawNumberAtLocation

; Draw the power level number
DrawNumPowerOrbsInPlayerInventory:
	; # of power orbs
	lda #<(MapMenuScreen+36+5*40)
	sta zpToPtrL
	lda #>(MapMenuScreen+36+5*40)
	sta zpToPtrH

	ldy #PLAYER_INVENTORY_POWER_LEVEL	; ByteParam0/1 = PlayerIntenvory->PowerLevel
	lda (zpPlayerInventoryPtr),y
	sta zpByteParam0
	lda #0
	sta zpByteParam1
	jmp DrawNumberAtLocation

; For each of the 5 counters draw the 3 digit numbers
DrawPlayerInventoryNumbers:
	jsr DrawNumKeysInPlayerInventory
	jsr DrawNumRedOrbsInPlayerInventory
	jsr DrawNumGreenOrbsInPlayerInventory
	jsr DrawNumBlueOrbsInPlayerInventory
	jmp DrawNumPowerOrbsInPlayerInventory	; This will RTS at the end

; =========================================================
; Add to the various inventory counters
; void AddToOrbsRed(ByteParam0 = Num to add)
;
AddToKeys:
	ldy #PLAYER_INVENTORY_KEYS
	; NOTE: will fall through to the actual add routine
; Y = index into the inventory structure where the number is being added
AddToInventoryCounterInt:	
	lda zpByteParam0
	clc					; a = a + #
	adc (zpPlayerInventoryPtr),y
	cmp #100
	bcc ?below100Int
	sbc #100
?below100Int:		
	sta (zpPlayerInventoryPtr),y		; store the remainder in score
	bcc ?endOfIntAdd					; if no carry is set then no overflow past 100	

	; Add one to the high red orb counter
	iny
	lda (zpPlayerInventoryPtr),y
	adc #0
	sta (zpPlayerInventoryPtr),y

?endOfIntAdd
	rts

AddToOrbsRed:
	ldy #PLAYER_INVENTORY_RED			; Orbs @ index 2 in the inventory (2 bytes)		
	bne AddToInventoryCounterInt

AddToOrbsGreen:
	ldy #PLAYER_INVENTORY_GREEN
	bne AddToInventoryCounterInt

AddToOrbsBlue:
	ldy #PLAYER_INVENTORY_BLUE
	bne AddToInventoryCounterInt

; =========================================================
; Add something to a byte counter

AddToBombs:
	ldy #PLAYER_INVENTORY_NUM_BOMBS
; Y = index into the inventory structure where the number is being added
AddToInventoryCounterByte:	
	lda zpByteParam0
	clc					; a = a + # 
	adc (zpPlayerInventoryPtr),y
	cmp #100
	bcc ?byteBelow100
	lda #99								; max is 99
?byteBelow100:
	sta (zpPlayerInventoryPtr),y		; store the remainder in score

	rts

AddToMaps;
	ldy #PLAYER_INVENTORY_NUM_MAPS
	bne AddToInventoryCounterByte

AddToVortex;
	ldy #PLAYER_INVENTORY_NUM_VORTEX
	bne AddToInventoryCounterByte

AddFunctionVector:
	.word 0, AddToOrbsRed, AddToOrbsGreen, AddToOrbsBlue
	.word AddToKeys, AddToBombs, AddToMaps, AddToVortex

; ParamByte0 is the amount to add
; ParamByte1 is the type of addition to perform
DispatchAddToInventory:
	lda zpByteParam1
	asl
	tay
	lda AddFunctionVector,y
	sta SMC_AddDispatch
	lda AddFunctionVector+1,y
	sta SMC_AddDispatch+1
	; Jump to the function
	jmp $ffff
SMC_AddDispatch = * - 2;

; =========================================================
LockChars:
	.byte 0
	.byte 127		; Power = 1
	.byte 96		; Orbs = 2
	.byte 64		; Keys = 3
	.byte 50		; Red Orbs = 4
	.byte 39		; Green Orbs = 5
	.byte 34		; Blue Orbs = 6

; =========================================================
; Draw Chest locks @ 10,4 in the MapMenuScreen
; (zpInteractingWithThisObjPtr),y  where 
DrawLockedChestCost:
	lda #<(MapMenuScreen+10+4*40)
	sta zpScreenPtrL
	lda #>(MapMenuScreen+10+4*40)
	sta zpScreenPtrH

?quickEntryToDrawLockCost:
	ldy #3				; ->UnlockCost<- is at byte 3 of InteractionObject
	lda (zpInteractingWithThisObjPtr),y
	beq ?noCostToOpenChest

	tax					; X = Cost
	ldy #2				; ->UnlockType<- is at byte #2 of InteractionObject
	lda (zpInteractingWithThisObjPtr),y	
	tay
	lda LockChars,y		; Which char for the lock type
	ldy #2
	sta (zpScreenPtr),y	; draw the lock type

	dey
	lda #12
	sta (zpScreenPtr),y	; draw the x

	dey
	lda Modulus10Table,x; Get the unlock number
	clc
	adc #16
	sta (zpScreenPtr),y	; draw the x

?noCostToOpenChest:
?dlecNothingToDo:
	rts

; =========================================================
; Draw Chest locks @ 10,2 in the MapMenuScreen
; (zpInteractingWithThisObjPtr),y  where 
DrawAttackCostForGhost:
	lda #<(MapMenuScreen+10+2*40)
	sta zpScreenPtrL
	lda #>(MapMenuScreen+10+2*40)
	sta zpScreenPtrH
	jmp ?quickEntryToDrawLockCost

; =========================================================
; Draw EXIT locks @ 9,3 and at 13x3 in the MapMenuScreen
; (zpInteractingWithThisObjPtr),y  where 
DrawLockedExitCost:
	lda #<(MapMenuScreen+9+3*40)
	sta zpScreenPtrL
	lda #>(MapMenuScreen+9+3*40)
	sta zpScreenPtrH

	; (ScreenPtr) is the new draw location

	; Figure out what to draw
	; Get the lock types and costs
	; if (lockType0 + lockType1 == 0) drawOpenDoor()
	ldy #2			; Loc@0, Type@1, UnlockType0@2, UnlockCost0@3, UnlockType0@4, UnlockCost0@5
	lda (zpInteractingWithThisObjPtr),y	; Get the UnlockType0
	sta zpByteParam0
	; Get the 2nd lock type
	ldy #4			; Loc@0, Type@1, UnlockType0@2, UnlockCost0@3, UnlockType0@4, UnlockCost0@5
	lda (zpInteractingWithThisObjPtr),y	; Get the UnlockType1
	sta zpByteParam1			; Param1 = type
	clc
	adc zpByteParam0			; A = Type0 + Type1
	; if Type0 + Type1 is 0 then draw the open door
	beq ?dlecNothingToDo		; Type0 + Type1 == 0 -> end early
	
	; Something but lock type NONE has been set
	; Check the total unlock cost
	; if (lockCost0 + lockCost1 == 0) -> draw nothing
	ldy #3			; Loc@0, Type@1, UnlockType0@2, UnlockCost0@3, UnlockType0@4, UnlockCost0@5
	lda (zpInteractingWithThisObjPtr),y	; Get the UnlockCost0
	sta zpByteParam2

	ldy #5			; Loc@0, Type@1, UnlockType0@2, UnlockCost0@3, UnlockType0@4, UnlockCost0@5
	lda (zpInteractingWithThisObjPtr),y	; Get the UnlockCost1
	sta zpByteParam3
	clc
	adc zpByteParam2
	beq ?dlecNothingToDo			; Not locked anymore. Draw the open door

	; Draw the lock 0 cost and type
	; num x type (vertically) starting at 9,3  9,4  and 9,5
	ldx zpByteParam0		; Type0
	beq ?noLockType0
	lda LockChars,x			; A is the char to draw
	ldx zpByteParam2		; Cost0
	beq ?noLockType0
	cpx #9
	bcc ?lockType0IsOk
	ldx #8					; Limit to max 8 counter
?lockType0IsOk:
	; draw the number
	ldy #80
	sta (zpScreenPtr),y		; Draw the lock type char
	ldy #40
	lda #12					; x character
	sta (zpScreenPtr),y		; Draw the x char
	lda Modulus10Table,x	; Get the unlock number
	clc
	adc #16					; Make the 0-9 digit
	ldy #0
	sta (zpScreenPtr),y		; Draw the cost

	; Draw lock 1 cost and type
	; num x type (vertically) starting at 13,3  13,4  and 13,5
?noLockType0:

	ldx zpByteParam1		; Type1
	beq ?noLockType1
	lda LockChars,x			; A is the char to draw
	ldx zpByteParam3		; Cost1
	beq ?noLockType1
	cpx #9
	bcc ?lockType1IsOk
	ldx #8					; Limit to max 8 counter
?lockType1IsOk:
	; draw the number
	ldy #84
	sta (zpScreenPtr),y		; Draw the lock type char
	ldy #44
	lda #12					; x character
	sta (zpScreenPtr),y		; Draw the x char
	lda Modulus10Table,x	; Get the unlock number
	clc
	adc #16					; Make the 0-9 digit
	ldy #4
	sta (zpScreenPtr),y		; Draw the cost

?noLockType1:
	rts	

; =========================================================
; Draw a blank section C
DrawSectionC:
	lda #<SectionCBlank
	sta zpFromPtrL
	lda #>SectionCBlank
	sta zpFromPtrH

	lda #<(MapMenuScreen+15)
	sta zpToPtrL
	lda #>(MapMenuScreen+15)
	sta zpToPtrH

	ldy #0			; Index into the source data 7xx source
	lda #7
	sta zpWorkY		; 5 lines counter
?NextLineDSC:
	ldx #19			; x counter
?NextCharInLineDSC:
	lda (zpFromPtr),y
	sta (zpToPtr),y
	iny
	dex
	bne ?NextCharInLineDSC
	; Move (to) the next line
	clc
	lda zpToPtrL
	adc #21
	sta zpToPtrL
	bcc ?NoRolloverDSC
	inc zpToPtrH
?NoRolloverDSC:
	dec zpWorkY
	bne ?NextLineDSC
	rts

; =========================================================
; Draw the contents of the pouch/chest/section c items
DrawSectionCItems:
	lda zpSectionCBox0Count
	ldx #0
	ldy zpSectionCBox0Type
	jsr DrawSectionCItem

	lda zpSectionCBox1Count
	ldx #1
	ldy zpSectionCBox1Type
	jsr DrawSectionCItem

	lda zpSectionCBox2Count
	ldx #2
	ldy zpSectionCBox2Type
	jmp DrawSectionCItem
	
; =========================================================
; Clear Section C
; Blank the section and fix the left side of the inventory
; Clobber: zpToPtr, zpWorkY
RestoreInventoryLeft:
	.byte 61,63,63,124,63,63,59		; Reverse order with 1 extra byte

ClearSectionC:
	lda #<(MapMenuScreen+15)
	sta zpToPtrL
	lda #>(MapMenuScreen+15)
	sta zpToPtrH

	lda #7
	sta zpWorkY		; 5 lines counter

	ldy #0
?NextLineCSC:
	ldx #18			; x counter
	lda #0
?NextCharInLineCSC:
	sta (zpToPtr),y
	iny
	dex
	bne ?NextCharInLineCSC
	; Fix the inventory left side
	ldx zpWorkY
	lda RestoreInventoryLeft-1,x
	sta (zpToPtr),y
	iny
	; Move (to) the next line
	clc
	lda zpToPtrL
	adc #21
	sta zpToPtrL
	bcc ?NoRolloverCSC
	inc zpToPtrH
?NoRolloverCSC:
	dec zpWorkY
	bne ?NextLineCSC

	rts	

; =========================================================
; Call the C random function
GetRandomNumber:
	jmp GetRandomNumber
SetRandPtr = * -2

; =========================================================
; Draw random elements into section c
DrawRandomSectionC:
	lda #20		; loop counter
	sta zpWorkY

DRSC_OuterLoop:
	lda #2
	sta zpWorkX	; slot counter
	
DRSC_InnerLoop:
	; Queue up parameters
	jsr GetRandomNumber
	and #7
	tay
	lda #0
	ldx zpWorkX
	jsr DrawSectionCItem

	dec zpWorkX
	bpl DRSC_InnerLoop

	jsr WaitForVBI
	jsr WaitForVBI
	jsr WaitForVBI

	dec zpWorkY
	bpl DRSC_OuterLoop

	rts

; =========================================================
ItemLocationL:	
	.byte <(MapMenuScreen+40+16)
	.byte <(MapMenuScreen+40+22)
	.byte <(MapMenuScreen+40+28)
ItemLocationH:	
	.byte >(MapMenuScreen+40+16)
	.byte >(MapMenuScreen+40+22)
	.byte >(MapMenuScreen+40+28)

; =========================================================
; X = 0,1,2 = slot position
; Y = Item nr: 0 = none, 1 = red, 2 = green, .. 7 = vortex
; A = number of items
; zpSectionCInverse = 0 | 128 draw the item inverted
; Clobber: zpByteParam1 & 2
; Clobber: zpToPtr
; Clobber: zpMenuTemp0
DrawSectionCItem:
	sta zpMenuTemp0		; The item counter (5x)
	; Where are we drawing to
	; X (0,1,2) is used to index the ItemLocation array
	lda ItemLocationL,x
	sta zpToPtrL
	lda ItemLocationH,x
	sta zpToPtrH				; ToPtr = ItemLocation[X]

	; Where is the 5x4 data coming from
	lda SectionCItemSrcL,y
	sta SMC_ItemPtr
	lda SectionCItemSrcH,y
	sta SMC_ItemPtr+1

	ldx #0		; src runner
	ldy #0		; dest runner
	lda #4
	sta zpByteParam1		; Y counter

BSCI_DrawLoop:	
	lda #4
	sta zpByteParam2		; X counter
	
BSCI_DrawLoopInner:	
	lda $2000,x	; This will get fixed by the self modifying code
SMC_ItemPtr = * - 2
	eor zpSectionCInverse
	sta (zpToPtr),y
	inx
	iny
	dec zpByteParam2		; X counter
	bpl BSCI_DrawLoopInner

	clc
	tya
	adc #35
	tay

	dec zpByteParam1		; Y counter
	bne BSCI_DrawLoop

	; On last line
	; Draw the counter at the bottom
	lda #5
	sta zpMenuTemp1		; Chars drawn counter, count down until we hit 0
	
	lda zpMenuTemp0		; This is the item counter
	beq BSCI_ClearLastLine
	; Draw the counter
	; If its > 10 then draw 2 digits and the x
	cmp #10
	bcc ?SingleDigitDraw
	; 10s
	tax
	lda Div10Table,x
	clc
	adc #16
	eor zpSectionCInverse
	sta (zpToPtr),y	; draw the digit
	iny
	dec zpMenuTemp1	; We have drawn 1 char

	lda zpMenuTemp0

?SingleDigitDraw:
	tax
	lda Modulus10Table,x
	clc
	adc #16
	eor zpSectionCInverse
	sta (zpToPtr),y	; draw the digit
	iny
	dec zpMenuTemp1	; We have drawn 1 char

	lda #12		;x
	eor zpSectionCInverse
	sta (zpToPtr),y	; draw the digit
	iny

	lda #0
	sta (zpToPtr),y	; draw the digit
	ldx zpMenuTemp1
	dex

	bne BSCI_DrawLastChars

	; Clear the counter
BSCI_ClearLastLine:
	ldx #5
BSCI_DrawLastChars:
	lda #0
	eor zpSectionCInverse
BSCI_DrawLoopEnd:
	sta (zpToPtr),y
	iny
	dex
	bne BSCI_DrawLoopEnd

	rts


; =========================================================
; Pouch interactions
PouchMenuType:	.byte CHEST_ITEM_TYPE_BOMB, CHEST_ITEM_TYPE_MAP, CHEST_ITEM_TYPE_VORTEX
PouchMenuCount:	.byte 0,0,0
PouchMenuActive: .byte 0

DrawPouchMenuFromState:
	; Draw the first box
	; Check if 0 is active if(PouchMenuActive == 0) then inverse
	lda #0
	sta zpSectionCInverse		; Nothing is inverse
	
	lda PouchMenuActive			; Which menu item is active
	bne ?drawPouchMenuSlot0

	lda #128					; Make this entry inverse
	sta zpSectionCInverse

?drawPouchMenuSlot0
	lda PouchMenuCount
	ldx #0
	ldy PouchMenuType
	jsr DrawSectionCItem		; X=0,1,2  A=0-99 number of items   Y=4,5,6 Key,Bomb,Map

	; Slot 1
	; Check if 0 is active if(PouchMenuActive == 0) then inverse
	lda #0
	sta zpSectionCInverse		; Nothing is inverse
	
	lda PouchMenuActive			; Which menu item is active
	cmp #1
	bne ?drawPouchMenuSlot1

	lda #128					; Make this entry inverse
	sta zpSectionCInverse

?drawPouchMenuSlot1
	lda PouchMenuCount+1
	ldx #1
	ldy PouchMenuType+1
	jsr DrawSectionCItem		; X=0,1,2  A=0-99 number of items   Y=4,5,6 Key,Bomb,Map

	; Slot 2
	; Check if 0 is active if(PouchMenuActive == 0) then inverse
	lda #0
	sta zpSectionCInverse		; Nothing is inverse
	
	lda PouchMenuActive			; Which menu item is active
	cmp #2
	bne ?drawPouchMenuSlot2

	lda #128					; Make this entry inverse
	sta zpSectionCInverse

?drawPouchMenuSlot2
	lda PouchMenuCount+2
	ldx #2
	ldy PouchMenuType+2
	jmp DrawSectionCItem		; X=0,1,2  A=0-99 number of items   Y=4,5,6 Key,Bomb,Map


; =========================================================
; Pouch interactions
; Items that can be traded are:
; Sell:
;	SECTION_C_ITEM_TYPE_ORB_RED = 1
;	SECTION_C_ITEM_TYPE_ORB_GREEN = 2
;	SECTION_C_ITEM_TYPE_ORB_BLUE = 3
;	SECTION_C_ITEM_TYPE_KEY = 4
;	SECTION_C_ITEM_TYPE_BOMB = 5
;	SECTION_C_ITEM_TYPE_MAP = 6
;	SECTION_C_ITEM_TYPE_VORTEX = 7
; Get:
;	
	.local
TradeMenuType:	.byte SECTION_C_ITEM_TYPE_ORB_RED, SECTION_C_ITEM_TYPE_ORB_GREEN
TradeMenuCount:	.byte 2,1

DrawTradeMenuFromState:
	; Draw the first box
	; Check if 0 is active if(PouchMenuActive == 0) then inverse
	lda #0
	sta zpSectionCInverse		; Nothing is inverse

	lda #0
	tax
	ldy #SECTION_C_ITEM_TYPE_GIVE_TAKE
	jsr DrawTradeMenuItem		; X=0,1,2  A=0-99 number of items   Y=4,5,6 Key,Bomb,Map

	; Slot 2
	; Check if 0 is active if(PouchMenuActive == 0) then inverse
	lda TradeMenuCount+1
	ldx #2
	ldy TradeMenuType+1
	jsr DrawTradeMenuItem		; X=0,1,2  A=0-99 number of items   Y=4,5,6 Key,Bomb,Map

	lda #128
	sta zpSectionCInverse		; Nothing is inverse
	; Slot 1
	lda TradeMenuCount			; Counter
	ldx #1
	ldy TradeMenuType			; What item to draw
	jmp DrawTradeMenuItem		; X=0,1,2  A=0-99 number of items   Y=4,5,6 Key,Bomb,Map

; =========================================================
; X = 0,1,2 = slot position
; Y = Item nr: 0 = none, 1 = red, 2 = green, .. 7 = vortex, 8, 9 = give take
; A = number of items
; Clobber: zpByteParam1 & 2
; Clobber: zpToPtr	- Top left corner of the slot
; Clobber: zpMenuTemp0
; Slot 0 does not draw the bottom counter
DrawTradeMenuItem:
	sta zpMenuTemp0				; The item counter (5x)
	stx zpMenuTemp1				; The slot position
	; Where are we drawing to
	; X (0,1,2) is used to index the ItemLocation array
	lda ItemLocationL,x
	sta zpToPtrL
	lda ItemLocationH,x
	sta zpToPtrH				; ToPtr = ItemLocation[X]

	; Where is the 5x4 data coming from
	; Self modifying code change the mem location of the lda ABCD,x command
	lda SectionCItemSrcL,y
	sta SMC_TMItemPtr
	lda SectionCItemSrcH,y
	sta SMC_TMItemPtr+1

	ldx #0					; X = src runner
	ldy #0					; Y = dest runner
	lda #4
	sta zpByteParam1		; Line counter

?BSCI_DrawLoop:
	lda #4
	sta zpByteParam2		; X counter
	
?BSCI_DrawLoopInner:	
	lda $2000,x				; This will get fixed by the self modifying code
SMC_TMItemPtr = * - 2
	eor zpSectionCInverse
	sta (zpToPtr),y			; ToPtr[Y] = SectionCItemSrc[X]
	inx
	iny
	dec zpByteParam2		; X counter
	bpl ?BSCI_DrawLoopInner

	; Move the target Y to the next line
	clc
	tya
	adc #35
	tay

	dec zpByteParam1		; Line counter
	bne ?BSCI_DrawLoop

	; Y = offset pointing to the start of the last line
	; (zpToPtr),y = location to draw to
	lda zpMenuTemp1			; Slot indicator
	beq ?drawSlot0			; Slot 0 then skip all other draws

	cmp #1
	beq ?drawSlot1

	; Draw slot 2
	; <-  1x
	lda #$1E				; <- left arrow
	sta (zpToPtr),y
	iny
	lda #0					; Draw two blanks
	sta (zpToPtr),y
	iny
	sta (zpToPtr),y
	iny

	ldx zpMenuTemp0			; X = how much counter 0-9
	lda Modulus10Table,x
	clc
	adc #16
	sta (zpToPtr),y			; draw the digit
	sta zpMenuTemp0			; zpMenuTemp0 = the counter char to draw

	iny
	lda #$0C				; x
	sta (zpToPtr),y			; draw the digit

	; Now draw the counter into slot 0
	tya
	sec
	sbc #13					; 13 chars left
	tay

	lda zpMenuTemp0			; zpMenuTemp0 = the counter char to draw
	sta (zpToPtr),y			; draw the digit
	iny
	lda #$0C				; x
	sta (zpToPtr),y			; draw the digit

	rts

?drawSlot1:	

	; Draw slot 1
	; 2x  >
	ldx zpMenuTemp0			; X = what counter value (0-9) to draw
	lda Modulus10Table,x
	clc
	adc #16+128
	sta (zpToPtr),y			; draw the digit
	sta zpMenuTemp0			; zpMenuTemp0 = the counter char to draw

	iny
	lda #$0C+128			; x
	sta (zpToPtr),y			; draw the digit
	iny
	lda #0+128				; Draw two blanks
	sta (zpToPtr),y
	iny
	sta (zpToPtr),y
	iny

	lda #$1F+128			; -> right arrow
	sta (zpToPtr),y

	; Now draw the counter into slot 0
	tya
	sec
	sbc #(7+3*40)			; 7 chars left, 3 lines up
	tay

	lda zpMenuTemp0			; zpMenuTemp0 = the counter char to draw
	and #127
	sta (zpToPtr),y			; draw the digit
	iny
	lda #$0C				; x
	sta (zpToPtr),y			; draw the digit

	rts

?drawSlot0:
	; move back 81
	tya
	sec
	sbc #(1+2*40)
	tay

	lda #$4B				; |-
	sta (zpToPtr),y 
	tya
	clc
	adc #6
	tay
	lda #$4C				; -|
	sta (zpToPtr),y
	rts