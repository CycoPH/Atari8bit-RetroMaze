; ASM conversion of the original DrawTheHall C function
.local

; Offset values where to draw onto the original 3D view to change it

WALL_0_TL_OFFSET = (3*40 + 6)		; Draw new chars (41, 9,9,9,9, 42) between these two points
WALL_0_TR_OFFSET = (3*40 + 33)
WALL_0_HORIZ_WIDTH = (33 - 6 - 2)

WALL_1_TL_OFFSET = (6*40 + 12)
WALL_1_TR_OFFSET = (6*40 + 27)
WALL_1_HORIZ_WIDTH = (27 - 12 - 2)
WALL_1_CLEAR_LEFT = (7 * 40 + 13)

WALL_2_TL_OFFSET = (8*40 + 16)
WALL_2_TR_OFFSET = (8*40 + 23)
WALL_2_HORIZ_WIDTH = (23 - 16 - 2)
WALL_2_CLEAR_LEFT = (9 * 40 + 17)

LEFT_EXIT_0_TOP_OFFSET = (3*40)
LEFT_EXIT_0_BOTTOM_OFFSET = (17*40)
LEFT_EXIT_1_TOP_OFFSET = (4*40+7)
LEFT_EXIT_1_BOTTOM_OFFSET = (14*40+6)
LEFT_EXIT_2_TOP_OFFSET = (7*40 + 13)

RIGHT_EXIT_0_TOP_OFFSET = (3*40 + 33)
RIGHT_EXIT_0_BOTTOM_OFFSET = (17*40 + 33)
RIGHT_EXIT_1_TOP_OFFSET = (4*40 + 27)
RIGHT_EXIT_2_TOP_OFFSET = (7*40 + 23)

; What characters to use to modify the 3D view
; Solid far end of wall
SOLID_WALL_TL = 41			; 41 9 9 9 9 42
SOLID_WALL_TR = 42
SOLID_WALL_BL = 23			; 23 9 9 9 9 24
SOLID_WALL_BR = 24
EXIT_LEFT_T = 27


WALL_CONNECT_H = 9			; -
WALL_CONNECT_V = 10			; |




; zpFromPtr
; zpToPtr
; zpLengthL
memcpy:
	ldy #0
	ldx zpLengthH
	beq ?memcpyShort ; We need only the short version for <1 pages
?memcpyLoopLong:
	; Copy X pages
	lda (zpFromPtr),y ; Loop unrolling can be done with confidence here
	sta (zpToPtr),y ; any power of 2 will work
	iny
	bne ?memcpyLoopLong
	inc zpFromPtrH ; Go to the next page
	inc zpToPtrH
	dex
	beq ?memcpyShort
	jmp ?memcpyLoopLong
?memcpyShort:
	; Copy remaining bytes
	ldx zpLengthL
	beq ?memcpyEnd
?memcpyLoopShort: ; Copy X bytes
	lda (zpFromPtr),y
	sta (zpToPtr),y
	iny
	dex
	bne ?memcpyLoopShort
?memcpyEnd:
	rts

; zpScreenPtr = top-left corner of area to blank
; X = width
; Y = height
blank:
	stx zpWorkX
	sty zpWorkY

?blankNextLineLoop:
	ldy zpWorkX
	lda #0
?blankNextByte:
	sta (zpScreenPtr),y
	dey
	bpl ?blankNextByte

	; Do the next line	
	dec zpWorkY
	bmi ?blankingIsDone

	; Move one line down
	lda zpScreenPtrL
	clc
	adc #40
	sta zpScreenPtrL
	bcc ?blankNextLineLoop
	inc zpScreenPtrH

	bne ?blankNextLineLoop

?blankingIsDone:	
	rts


; This is an ASM implementation of the C drawTheHall() function
; Should be a bit faster and especially smaller.
; If anything in the 3D view changes still will need to change
DrawTheHall:
	; Copy the original 3D view
	;if (ASM_DATA.PageNr == 0)
	;	ASM_DATA.PagePtr = (unsigned char *)FIXED_GAMESCREEN1;
	;else
	;	ASM_DATA.PagePtr = (unsigned char *)FIXED_GAMESCREEN0;
	;memcpy(ASM_DATA.PagePtr, (char*)FIXED_ORIGINAL3DVIEW, 840);
	lda zpPageNr
	beq ?onPage0
	; On page 1 then draw to screen 0
	lda #<GameScreen0
	ldx #>GameScreen0
	jmp ?gotThePagePointer
	; On page 0, then draw to screen 1
?onPage0;
	lda #<GameScreen1
	ldx #>GameScreen1

?gotThePagePointer
	sta zpPagePtrL
	stx zpPagePtrH		; (zpPagePtr) = GameScreen0 or GameScreen 1
	sta zpToPtrL		; (zpToPtr) = GameScreen0 or GameScreen 1
	stx zpToPtrH

	; copy 840 bytes (21*40) from the Original3DView to the PagePtr
	lda #<Original3DView
	sta zpFromPtrL
	lda #>Original3DView
	sta zpFromPtrH

	lda #<840
	sta zpLengthL
	lda #>840
	sta zpLengthH

	jsr memcpy

	; There are 4 states for the wall infront of us.
	; Determined by the zpWallDistance variable
	; How far down the hall can we see?
	; switch(WallDistance)
	; {
	; 	case 0:		// Very close
	;	case 1:		// One block away
	;	case 2:		// Two blocks away
	;   default:	// Very far away and we dont draw anything
	; }
	lda zpWallDistance
	beq ?atDistance0
	cmp #1
	beq ?atDistance1
	cmp #2
	jeq ?atDistance2
	jmp ?doneWithMiddleWalls

	; -----------------------------------------------------
	; Make the closest wall a full solid one
?atDistance0:
	; screenPtr = ASM_DATA.PagePtr + WALL_0_TL_OFFSET;
	lda     zpPagePtr
	ldx     zpPagePtr+1
	clc
	adc     #WALL_0_TL_OFFSET		; 6,3
	bcc     ?L0005
	inx
?L0005:	
	sta     zpScreenPtrL
	stx     zpScreenPtrH

	; Top left corner and the char next to it
	ldy #0
	lda #SOLID_WALL_TL		; $29
	sta (zpScreenPtr),y

	iny
	lda #WALL_CONNECT_H		; $9
	sta (zpScreenPtr),y

	; Top-right corner and the char to the left of it
	ldy #26
	sta (zpScreenPtr),y

	iny
	lda #SOLID_WALL_TR		; $2A
	sta (zpScreenPtr),y

	; Now blank out the (7,4) - (32,17) area
	lda zpPagePtrL
	clc
	adc #<(7 + 4*40)
	sta zpScreenPtrL
	lda zpPagePtrH
	adc #>(7 + 4*40)
	sta zpScreenPtrH

	ldx #32-7
	ldy #17-4
	jsr blank

	; Move one byte left and draw the bottom line
	lda    zpScreenPtr
	sec
	sbc     #$01
	sta    zpScreenPtr
	bcs     L0014
	dec    zpScreenPtr+1
L0014:	
	ldy #0
	lda #SOLID_WALL_BL
	sta (zpScreenPtr),y

	iny
	lda #WALL_CONNECT_H
	ldx #26
?horizLoopDist0:
	sta (zpScreenPtr),y
	iny
	dex
	bne ?horizLoopDist0

	lda #SOLID_WALL_BR
	sta (zpScreenPtr),y
	jmp  ?doneWithMiddleWalls

	; -----------------------------------------------------
	; The solid wall is one block away
?atDistance1:
	; Calc where we need to start drawing
	; screenPtr = ASM_DATA.PagePtr + WALL_1_TL_OFFSET;
	lda     zpPagePtr
	ldx     zpPagePtr+1
	clc
	adc     #WALL_1_TL_OFFSET		; 12,6 (252 bytes down)
	bcc     ?L0006
	inx
?L0006:	
	sta     zpScreenPtrL
	stx     zpScreenPtrH

	; Top left corner and the char next to it
	ldy #0
	lda #SOLID_WALL_TL		; $29
	sta (zpScreenPtr),y		; [12,6] = $29

	iny
	lda #WALL_CONNECT_H		; $9
	sta (zpScreenPtr),y		; [13,6] = $9

	; Top-right corner and the char to the left of it
	ldy #14
	sta (zpScreenPtr),y		; [26,6] = $9

	iny
	lda #SOLID_WALL_TR		; [27,6] = $2A
	sta (zpScreenPtr),y

	; Now blank out the (13,7) - (26,13) area
	lda zpPagePtrL
	clc
	adc #<(13 + 7*40)
	sta zpScreenPtrL
	lda zpPagePtrH
	adc #>(13 + 7*40)
	sta zpScreenPtrH

	ldx #26-13				; blank out (13,7) - (26,13 )
	ldy #14-7
	jsr blank

	; Move one byte left and draw the bottom line
	lda    zpScreenPtr
	sec
	sbc    #$01
	sta    zpScreenPtr
	bcs    L0024
	dec    zpScreenPtr+1
L0024:	
	ldy #0
	lda #SOLID_WALL_BL		; [12,14] = 23
	sta (zpScreenPtr),y

	iny						; [13...26,14] = 9 -
	lda #WALL_CONNECT_H
	ldx #14
?horizLoopDist1:
	sta (zpScreenPtr),y
	iny
	dex
	bne ?horizLoopDist1

	lda #SOLID_WALL_BR		; [27,14] = 24
	sta (zpScreenPtr),y
	bne  ?doneWithMiddleWalls

; -----------------------------------------------------

?atDistance2:
; screenPtr = ASM_DATA.PagePtr + WALL_2_TL_OFFSET;
	lda     zpPagePtr
	clc
	adc     #<WALL_2_TL_OFFSET		; 16,8 
	sta     zpScreenPtrL
	lda     zpPagePtr+1
	adc 	#>WALL_2_TL_OFFSET
	sta		zpScreenPtrH

	; Top left corner and the char next to it
	ldy #0
	lda #SOLID_WALL_TL		; $29
	sta (zpScreenPtr),y		; [16,11] = $29

	iny
	lda #WALL_CONNECT_H		; $9
	sta (zpScreenPtr),y		; [17,11] = $9

	; Top-right corner and the char to the left of it
	ldy #6
	sta (zpScreenPtr),y		; [22,11] = $9

	iny
	lda #SOLID_WALL_TR		; [23,11] = $2A
	sta (zpScreenPtr),y

	; Blank the small area in the middle (only 4 chars here)
	lda #0
	ldy #41
	sta (zpScreenPtr),y		; [17,12] = 0
	ldy #46
	sta (zpScreenPtr),y		; [22,12] = 0
	ldy #121
	sta (zpScreenPtr),y		; [17,14] = 0
	ldy #126
	sta (zpScreenPtr),y		; [22,14] = 0
	
	ldy #160
	lda #SOLID_WALL_BL		; [16,15] = 23
	sta (zpScreenPtr),y

	iny						; [13...26,14] = 9
	lda #WALL_CONNECT_H
	ldx #6
?horizLoopDist2:
	sta (zpScreenPtr),y
	iny
	dex
	bne ?horizLoopDist2

	lda #SOLID_WALL_BR		; [27,14] = 24
	sta (zpScreenPtr),y
?doneWithMiddleWalls:

	; -----------------------------------------------------
	; This is drawing the side exits. From the closest to the farthest.
	; LeftView[0..2] indictes if there is an exit to the left
	; RightView[0..2] indictes if there is an exit to the right
	lda zpLeftView			; LeftView[0]
	beq ?noExitOnTheLeftAtDist0

	; -----------------------------------------------------
	; There is an exit on the left at ditance 0
	; screenPtr = ASM_DATA.PagePtr + LEFT_EXIT_0_TOP_OFFSET;
	lda     zpPagePtr
	ldx     zpPagePtr+1
	clc
	adc     #LEFT_EXIT_0_TOP_OFFSET		; 0,3
	bcc     ?leftExit0NoCarry
	inx
?leftExit0NoCarry:
	sta     zpScreenPtrL
	stx     zpScreenPtrH

	; 27 9 9 9 9 8 (31 | 43)
	ldy #0
	lda #EXIT_LEFT_T			; T flipped left
	sta (zpScreenPtr),y

	ldx #4						; Draw 4x the 9 (-)
	lda #WALL_CONNECT_H
	iny
?leftExit0TopBar:
	sta (zpScreenPtr),y
	iny
	dex
	bne ?leftExit0TopBar

	lda #8						; end of the 4x-
	sta (zpScreenPtr),y			; the connector
	iny

	; Depending on where the wall distance is draw 31 or 43
	lda zpWallDistance
	beq ?leftExitWithWallAt0Top
	lda #31
	bne ?leftExitFinTopBar
?leftExitWithWallAt0Top
	lda #43
?leftExitFinTopBar
	sta (zpScreenPtr),y

	; Draw the bottom bar of the left exit
	; Start of 0,17
	lda     zpPagePtr
	clc
	adc     #<LEFT_EXIT_0_BOTTOM_OFFSET		; 0,17
	sta		zpScreenPtrL
	lda 	zpPagePtrH
	adc 	#>LEFT_EXIT_0_BOTTOM_OFFSET		; 0,17
	sta 	zpScreenPtrH

	ldy #0
	lda #EXIT_LEFT_T
	sta (zpScreenPtr),y

	ldx #5						; Draw 5x the 9 (-)
	lda #WALL_CONNECT_H
	iny
?leftExit0BottomBar:
	sta (zpScreenPtr),y
	iny
	dex
	bne ?leftExit0BottomBar

	; Depending on where the wall distance is draw 25 or 80
	lda zpWallDistance
	beq ?leftExitWithWallAt0Bottom
	lda #25
	bne ?leftExitFinBottomBar
?leftExitWithWallAt0Bottom
	lda #80
?leftExitFinBottomBar
	sta (zpScreenPtr),y		; 

	; Line 18 at 3 bytes in
	ldy #43
	lda #0
	sta (zpScreenPtr),y		; [3, 18] = 0
	iny
	sta (zpScreenPtr),y		; [4, 18] = 0
	iny
	sta (zpScreenPtr),y		; [5, 18] = 0;

	; Line 19
	ldy #81
	sta (zpScreenPtr),y		; [1,19] = 0
	iny
	sta (zpScreenPtr),y		; [2,19] = 0
	iny
	sta (zpScreenPtr),y		; [3,19] = 0

	; Line 20
	ldy #120
	lda #35
	sta (zpScreenPtr),y		; [0,20] = 35
	iny
	lda #WALL_CONNECT_H
	sta (zpScreenPtr),y		; [1,20] = 9
?noExitOnTheLeftAtDist0:	

	; -----------------------------------------------------
	; This is drawing the side exits. From the closest to the farthest.
	; Check if there is an exit to the right at distance 0
	lda zpRightView			; RightView[0]
	beq ?noExitOnTheRightAtDist0

	; -----------------------------------------------------
	; There is an exit on the right at ditance 0
	; screenPtr = ASM_DATA.PagePtr + RIGHT_EXIT_0_TOP_OFFSET;
	lda     zpPagePtr
	ldx     zpPagePtr+1
	clc
	adc     #RIGHT_EXIT_0_TOP_OFFSET		; 33,3
	bcc     ?rightExit0NoCarry
	inx
?rightExit0NoCarry:
	sta     zpScreenPtrL
	stx     zpScreenPtrH

	; Top at 33x3
	; (32 | 44) 7 9 9 9 9 28
	; Depending on where the wall distance is draw 32 or 44
	ldy #0
	lda zpWallDistance
	beq ?rightExitWithWallAt0Top
	lda #32
	bne ?rightExitStartTopBar
?rightExitWithWallAt0Top
	lda #44
?rightExitStartTopBar
	sta (zpScreenPtr),y			; Draw the connection at 33,3

	iny
	lda #7
	sta (zpScreenPtr),y			; Draw 7 at 34,3

	ldx #4						; Draw 4x the 9 (-)
	lda #WALL_CONNECT_H
	iny
?rightExit0TopBar:
	sta (zpScreenPtr),y
	iny
	dex
	bne ?rightExit0TopBar

	lda #28						; end of the top bar with a right flipped T
	sta (zpScreenPtr),y			; the connector

	; Draw the bottom bar of the right exit
	; Start of 33,17
	lda     zpPagePtr
	clc
	adc     #<RIGHT_EXIT_0_BOTTOM_OFFSET		; 33,17
	sta		zpScreenPtrL
	lda 	zpPagePtrH
	adc 	#>RIGHT_EXIT_0_BOTTOM_OFFSET		; 33,17
	sta 	zpScreenPtrH

	ldy #0
	; Depending on where the wall distance is draw 26 or 80
	lda zpWallDistance
	beq ?rightExitWithWallAt0Bottom
	lda #26
	bne ?rightExitFinBottomBar
?rightExitWithWallAt0Bottom
	lda #80
?rightExitFinBottomBar
	sta (zpScreenPtr),y			; [33,17] = 26 | 80

	ldx #5						; Draw 5x the 9 (-)
	lda #WALL_CONNECT_H
	iny
?rightExit0BottomBar:
	sta (zpScreenPtr),y
	iny
	dex
	bne ?rightExit0BottomBar

	lda #28						; [39,17] = 28
	sta (zpScreenPtr),y
	
	; Line 18 at 3 bytes in
	ldy #41
	lda #0
	sta (zpScreenPtr),y		; [34, 18] = 0
	iny
	sta (zpScreenPtr),y		; [35, 18] = 0
	iny
	sta (zpScreenPtr),y		; [36, 18] = 0;

	; Line 19
	ldy #83
	sta (zpScreenPtr),y		; [36,19] = 0
	iny
	sta (zpScreenPtr),y		; [37,19] = 0
	iny
	sta (zpScreenPtr),y		; [38,19] = 0

	; Line 20
	ldy #125
	lda #WALL_CONNECT_H
	sta (zpScreenPtr),y		; [38,20] = 9
	iny
	lda #36
	sta (zpScreenPtr),y		; [39,20] = 39
?noExitOnTheRightAtDist0:

	; -----------------------------------------------------
	; Left and right exits at distance 0 are done.
	; If there is a wall at distance 0 then it will block any 
	; other exits, so don't draw.
	; if (wallDistance > 0) then there is a chance to draw
	; exits that are further away.
	lda zpWallDistance					; Get wall distance
	bne ?noWallAt0						; if @ 0 then skip the draw section
	jmp ?wallAt0SoSkipGreaterThan0Draw

?noWallAt0:
	; -------------------------------------------
	; No wall at 0 so we could possibly see the exists at distance 1
	; if (ASM_DATA.LeftView[1])
	lda zpLeftView+1
	bne ?haveLeftExitAtDistance1
	jmp ?noLeftExitAtDistance1

?haveLeftExitAtDistance1:	
	; Draw left exit at distance 1
	; Calc were we need to start drawing
	; screenPtr = ASM_DATA.PagePtr + LEFT_EXIT_1_TOP_OFFSET;
	lda     zpPagePtr
	ldx     zpPagePtr+1
	clc
	adc     #LEFT_EXIT_1_TOP_OFFSET		; 7,4
	bcc     ?leftExit1NoCarry
	inx
?leftExit1NoCarry:
	sta     zpScreenPtrL
	stx     zpScreenPtrH

	; Start drawing at 7,4 (top left)
	; Draw the wall edge down the left wall to indicate the exit
	; From top to bottom the bytes are:
	; Line 4: 29 - Angle to vertical connector
	; Line 5: 10 - Vertical bar
	; Line 6: 27 - Vertical to right connection T
	; Line 6: 3x 9 - Hirozontal bar connecting to the right corner
	; Line 6: 8 - Angle connector (horizontal har with the left up edge)
	; Line 6: 43 or 31 - Wall @ 1 then 43 otherwise 31

	ldy #0
	lda #29
	sta (zpScreenPtr),y					; [7,4] = 29

	ldy #40
	lda #WALL_CONNECT_V
	sta (zpScreenPtr),y					; [7,5] = 10 |

	; Horizontal bar T---\.
	ldy #80
	lda #27
	sta (zpScreenPtr),y					; [7,6] = 27

	lda #WALL_CONNECT_H
	iny
	sta (zpScreenPtr),y					; [8,6] = 9 -
	iny
	sta (zpScreenPtr),y					; [9,6] = 9 -
	iny
	sta (zpScreenPtr),y					; [10,6] = 9 -

	iny
	lda #8
	sta (zpScreenPtr),y					; [11,6] = 8 \
	iny

	; Draw the possible wall connection
	; If there is a wall at distance 1 then the connection
	; element looks different
	; wallDistance == 1 ? 43 : 31; 
	lda zpWallDistance
	cmp #1
	bne ?leftExitAtDist1WallNotAt1		; ==> Wall at 2+
	lda #43								; wall at 1
	bne ?drawConnector43
?leftExitAtDist1WallNotAt1:
	lda #31
?drawConnector43:
	sta (zpScreenPtr),y					; [12,6] = 43 | 31

	; Continue with the vertical bar at line 7
	; From top to bottom the bytes are:
	; Lines 7...10: 10 - Vertical bar
	lda #WALL_CONNECT_V
	ldy #120
	sta (zpScreenPtr),y					; [7,7] = 10
	ldy #160
	sta (zpScreenPtr),y					; [7,8] = 10
	ldy #200
	sta (zpScreenPtr),y					; [7,9] = 10
	ldy #240
	sta (zpScreenPtr),y					; [7,10] = 10
	; Just run out of Y index and we need to move the zpScreenPtr
	; to position 7x11. Already at 7, so just add 7*40 to move
	; the ptr down 7 lines
	lda zpScreenPtrL					; screenPtr = [7,11]
	clc
	adc #<280
	sta zpScreenPtrL
	lda zpScreenPtrH
	adc #>280
	sta zpScreenPtrH

	; zpScreenPtr -> 7x11 now

	; Continue with the vertical bar at line 11
	; From top to bottom the bytes are:
	; Lines 11...13: 10 - Vertical bar
	; Line 15: 10 - Vertical bar
	; Line 15: 0,0,0 - clear the three byte that were the bottom edge of the left hall
	; Line 16: 10 - Vertical bar
	; Line 16: 0,0 - Clear the next two bytes that were the bottom edge of the left hall
	; Line 14: 27 - Vertical to right connection T
	; Line 14: 4x 9 - Hirozontal bar connecting to the right corner
	; Line 14: 8 - Angle connector (horizontal har with the left up edge)
	lda #WALL_CONNECT_V
	ldy #0
	sta (zpScreenPtr),y					; [7,11] = 10
	ldy #40
	sta (zpScreenPtr),y					; [7,12] = 10
	ldy #80
	sta (zpScreenPtr),y					; [7,13] = 10
	; Skip line 14 will be drawn later
	ldy #160
	sta (zpScreenPtr),y					; [7,15] = 10
	ldy #200
	sta (zpScreenPtr),y					; [7,16] = 10
	
	; Clear postition 8 & 9 on line 16
	lda #0
	iny
	sta (zpScreenPtr),y					; [8,16] = 0
	iny
	sta (zpScreenPtr),y					; [9,16] = 0

	; Clear position 9,10,11 on line 15
	ldy #160+2
	sta (zpScreenPtr),y					; [9,15] = 0
	iny
	sta (zpScreenPtr),y					; [10,15] = 0
	iny
	sta (zpScreenPtr),y					; [11,15] = 0

	; Line 14 - Left horizontal bar connection
	ldy #120
	lda #27
	sta (zpScreenPtr),y					; [7,14] = 10

	; Horizontal bar to draw the bottom of the left exit corridor
	lda #WALL_CONNECT_H
	ldx #4
?horizDrawLoop1
	iny
	sta (zpScreenPtr),y					; [8...11,14] = 9
	dex
	bne ?horizDrawLoop1

	; Finish it off 
	; if wallDistance == 1 ? 80 : 25
	iny
	lda zpWallDistance
	cmp #1
	beq ?leftExit1WallAt1Bottom		; ==> Wall at 2+
	lda #25
	bne ?drawConnectorExitLeft
?leftExit1WallAt1Bottom
	lda #80
?drawConnectorExitLeft:
	sta (zpScreenPtr),y					; [12,14] = 80 | 25

	; -------------------------------------------
	; if (ASM_DATA.RightView[1])
?noLeftExitAtDistance1:
	lda zpRightView+1
	bne ?gotRightExitAtDist1
	jmp ?wallAt0SoSkipGreaterThan0Draw

	; Have a right middle exit
?gotRightExitAtDist1:

	; screenPtr = ASM_DATA.PagePtr + RIGHT_EXIT_1_TOP_OFFSET;
	lda     zpPagePtr
	ldx     zpPagePtr+1
	clc
	adc     #RIGHT_EXIT_1_TOP_OFFSET		; 27,4
	bcc     ?rightExit1NoCarry
	inx
?rightExit1NoCarry:
	sta     zpScreenPtrL
	stx     zpScreenPtrH

	ldy #5
	lda #30
	sta (zpScreenPtr),y					; [32,4] = 30

	ldy #40+5
	lda #WALL_CONNECT_V
	sta (zpScreenPtr),y					; [32,5] = 10 |

	ldy #80	; Line 6
	ldx zpWallDistance
	dex
	beq ?rightWant44
	lda #32
	bne ?drawItRightTop
?rightWant44
	lda #44
?drawItRightTop:
	sta (zpScreenPtr),y					; [27,6] = 32 or 44

	iny
	lda #7
	sta (zpScreenPtr),y					; [28,6] = 7

	lda #WALL_CONNECT_H
	iny
	sta (zpScreenPtr),y					; [29,6] = 9
	iny
	sta (zpScreenPtr),y					; [30,6] = 9
	iny
	sta (zpScreenPtr),y					; [31,6] = 9

	iny
	lda #28
	sta (zpScreenPtr),y					; [32,6] = 28

	; Line 7
	lda #WALL_CONNECT_V
	ldy #125
	sta (zpScreenPtr),y					; [32,7] = 10

	; Line 8
	ldy #165
	sta (zpScreenPtr),y					; [32,8] = 10

	; Line 9
	ldy #205
	sta (zpScreenPtr),y					; [32,9] = 10

	; Line 10
	ldy #245
	sta (zpScreenPtr),y					; [32,10] = 10

	; Set draw ptr to [27,11]
	lda zpScreenPtrL
	clc
	adc #<(7*40)
	sta zpScreenPtrL
	lda zpScreenPtrH
	adc #>(7*40)
	sta zpScreenPtrH

	; Line 11
	lda #WALL_CONNECT_V
	ldy #5
	sta (zpScreenPtr),y					; [32,11] = 10
	ldy #45
	sta (zpScreenPtr),y					; [32,12] = 10
	ldy #85
	sta (zpScreenPtr),y					; [32,13] = 10
	; Skip line 14
	ldy #165
	sta (zpScreenPtr),y					; [32,15] = 10
	ldy #205
	sta (zpScreenPtr),y					; [32,16] = 10

	; Draw line 14
	ldx zpWallDistance
	dex
	beq ?rightExit1WallAt1Bottom
	lda #26
	bne ?drawItRightBottom
?rightExit1WallAt1Bottom:
	lda #80
?drawItRightBottom:
	ldy #120							
	sta (zpScreenPtr),y					; [27,14] = 26 or 80

	lda #WALL_CONNECT_H
	iny
	sta (zpScreenPtr),y					; [28,14] = 9
	iny
	sta (zpScreenPtr),y					; [29,14] = 9
	iny
	sta (zpScreenPtr),y					; [30,14] = 9
	iny
	sta (zpScreenPtr),y					; [31,14] = 9

	lda #28
	iny
	sta (zpScreenPtr),y					; [32,14] = 28

	ldy #161
	lda #0
	sta (zpScreenPtr),y					; [28,15] = 0
	iny
	sta (zpScreenPtr),y					; [29,15] = 0
	iny
	sta (zpScreenPtr),y					; [30,15] = 0

	ldy #200+3
	sta (zpScreenPtr),y					; [30,16] = 0
	iny
	sta (zpScreenPtr),y					; [31,16] = 0

?wallAt0SoSkipGreaterThan0Draw:

	; if(wallDistance > 1) then draw the far away left and right exists
	lda zpWallDistance
	cmp #2
	bcs ?isMoreThan1
	jmp ?wallIsCloserThan2
?isMoreThan1:

	;--------------------------------------------
	; Left far exit
	; if (ASM_DATA.LeftView[2])
	lda zpLeftView+2
	beq ?noLeftExitAt2

	; Draw the left exit
	; screenPtr = ASM_DATA.PagePtr + LEFT_EXIT_2_TOP_OFFSET;
	lda zpPagePtrL
	clc
	adc #<LEFT_EXIT_2_TOP_OFFSET		; 13,7
	sta zpScreenPtrL
	lda zpPagePtrH
	adc #>LEFT_EXIT_2_TOP_OFFSET
	sta zpScreenPtrH

	; Look for the right exit
	ldy #0
	lda #29
	sta (zpScreenPtr),y					; [13,7] = 29
	; Line 8
	ldy #40
	lda #27
	sta (zpScreenPtr),y					; [13,8] = 27

	lda #WALL_CONNECT_H
	iny
	sta (zpScreenPtr),y					; [14,8] = 9 --

	lda #8
	iny
	sta (zpScreenPtr),y					; [15,8] = 8

	iny

	lda zpWallDistance
	cmp #2
	beq ?leftExit2HasWallAt2
	lda #31
	bne ?skipOne
?leftExit2HasWallAt2
	lda #43
?skipOne:
	sta (zpScreenPtr),y					; [16,8] = 31 or 43

	ldy #80
	lda #WALL_CONNECT_V
	sta (zpScreenPtr),y					; [13,9] = 10 |

	ldy #120
	lda #WALL_CONNECT_V
	sta (zpScreenPtr),y					; [13,10] = 10 |

	ldy #160
	lda #WALL_CONNECT_V
	sta (zpScreenPtr),y					; [13,11] = 10 |

	ldy #200
	lda #27
	sta (zpScreenPtr),y					; [13,12] = 27

	lda #9
	iny
	sta (zpScreenPtr),y					; [14,12] = 9 --
	iny
	sta (zpScreenPtr),y					; [15,12] = 9 --

	lda zpWallDistance
	cmp #2
	beq ?leftExit2HasWallAt2Bottom
	lda #25
	bne ?skipSome
?leftExit2HasWallAt2Bottom
	lda #80
?skipSome:	
	iny
	sta (zpScreenPtr),y					; [16,12] = 25 OR 80

	ldy #240
	lda #WALL_CONNECT_V
	sta (zpScreenPtr),y					; [13,12] = 9 |

	lda #0
	iny
	sta (zpScreenPtr),y					; [14,13] = 0
	iny
	sta (zpScreenPtr),y					; [15,13] = 0
?noLeftExitAt2:
	; -------------------------------------------
	; Look at the right exit
	lda zpRightView+2
	beq ?noRightViewAtDist2

	; Draw the exit right back
	; screenPtr = ASM_DATA.PagePtr + RIGHT_EXIT_2_TOP_OFFSET;
	lda zpPagePtrL
	clc
	adc #<RIGHT_EXIT_2_TOP_OFFSET		; 23,7
	sta zpScreenPtrL
	lda zpPagePtrH
	adc #>RIGHT_EXIT_2_TOP_OFFSET
	sta zpScreenPtrH
	; Line 7
	ldy #3
	lda #30
	sta (zpScreenPtr),y					; [26,7] = 30
	; Line 8
	lda zpWallDistance
	cmp #2
	bne ?backRightExitWallAt2
	lda #44
	bne ?skipBackRight
?backRightExitWallAt2:
	lda #32
?skipBackRight
	ldy #40
	sta (zpScreenPtr),y					; [23,8] = 32 OR 44
	iny
	lda #7
	sta (zpScreenPtr),y					; [24,8] = 7 /
	iny
	lda #WALL_CONNECT_H
	sta (zpScreenPtr),y					; [25,8] = 9 -
	iny
	lda #28
	sta (zpScreenPtr),y					; [26,8] = 28

	lda #WALL_CONNECT_V
	ldy #83
	sta (zpScreenPtr),y					; [26,9] = 10 |
	ldy #123
	sta (zpScreenPtr),y					; [26,10] = 10 |
	ldy #163
	sta (zpScreenPtr),y					; [26,11] = 10 |

	; Line 12
	ldy #200
	lda zpWallDistance
	cmp #2
	bne ?backRightExitWallAt2Bottom
	lda #80
	bne ?skipBackRightBottom
?backRightExitWallAt2Bottom:
	lda #26
?skipBackRightBottom
	sta (zpScreenPtr),y					; [23,12] = 26 OR 80

	lda #WALL_CONNECT_H
	iny
	sta (zpScreenPtr),y					; [24,12] = 9 -
	iny
	sta (zpScreenPtr),y					; [25,12] = 9 -
	iny
	lda #28
	sta (zpScreenPtr),y					; [26,12] = 28

	; Line 13
	ldy #241
	lda #0
	sta (zpScreenPtr),y					; [24,13] = 28
	iny
	sta (zpScreenPtr),y					; [25,13] = 0
	iny
	lda #WALL_CONNECT_V
	sta (zpScreenPtr),y					; [26,13] = 0

?noRightViewAtDist2:
?wallIsCloserThan2:
	rts


; =========================================================
LockTypeChar: 
	.byte 77		; = empty bar 	CHEST_LOCKED_TYPE_NONE = 0
	.byte 101		; z power 		CHEST_LOCKED_TYPE_POWER = 1
	.byte 89		; o orb			CHEST_LOCKED_TYPE_ORBS = 2
	.byte 117		; key			CHEST_LOCKED_TYPE_KEYS = 3
	.byte 89		; Red orb		EXIT_LOCKED_TYPE_RED = 4
	.byte 89		; Green orb		EXIT_LOCKED_TYPE_GREEN = 5
	.byte 89		; Blue orb		EXIT_LOCKED_TYPE_BLUE = 6

; =========================================================
; Draw chest power bar
; 1,4 offset inside the chest
; |====|
.local
DrawChestLock:
	lda     zpToPtrL
	ldx     zpToPtrH
	clc
	adc     #(4*40+1)				; 1,4
	bcc     ?dclNoCarry
	inx
?dclNoCarry:
	sta     zpScreenPtrL
	stx     zpScreenPtrH

	; zpScreenPtr is the location to draw to
	
	; What type of lock do we have?
	ldy #2			; Loc@0, Type@1, -->UnlockType@2<--, UnlockCost@3
	lda (zpInteractingWithThisObjPtr),y
	sta zpByteParam0			; Param0 = lock type
	tax
	lda LockTypeChar,x			; Get the char to draw
	sta zpByteParam2			; Param2 = the lock character

	iny
	lda (zpInteractingWithThisObjPtr),y	; Get the UnlockCost
	cmp #5						; If < 5 then we can draw, otherwise limit to 4 chars
	bcc ?dclBelow5
	lda #4
?dclBelow5
	sta zpByteParam1			; Param1 = cost

	; Draw something
	ldy #0
	lda #74			; Left side of lock bar |
	sta (zpScreenPtr),y
	iny

	; Draw the lock characters
	ldx zpByteParam1
	beq ?unlocked

	lda zpByteParam2		; The lock character
?drawNextLockChar:	
	sta (zpScreenPtr),y
	iny
	dex
	bne ?drawNextLockChar

	; How many blanks to draw?
	lda #4
	sec
	sbc zpByteParam1
	beq ?doneWithUnlockBar	; Nothing left to draw

	tax
	lda #77
?drawNextUnlockedChar:	
	sta (zpScreenPtr),y
	iny
	dex
	bne ?drawNextUnlockedChar

	jmp ?doneWithUnlockBar
	; ---------------------------------
	; |open|
?unlocked:
	lda #77		; blank bar
	sta (zpScreenPtr),y
	iny
	lda #111				; OP
	sta (zpScreenPtr),y
	iny
	lda #112				; EN
	sta (zpScreenPtr),y
	iny
	lda #77
	sta (zpScreenPtr),y
	iny
?doneWithUnlockBar:	
	lda #78		; Closing |
	sta (zpScreenPtr),y

	rts

; =========================================================
; Draw vortex power bar ALWAYS POWER LOCKED
; 1,6 offset inside the chest
; ======
; 2,4 or 6
.local
DrawVortexLock:
	lda     zpToPtrL
	ldx     zpToPtrH
	clc
	adc     #(4*40-1)				; 1,6
	bcc     ?dvlNoCarry
	inx
?dvlNoCarry:
	sta     zpScreenPtrL
	stx     zpScreenPtrH

	; zpScreenPtr is the location to draw to
SideEntryForPowerBarDraw:
	; Always draw the power lock
	; UNLESS the unlock power is 0 then we exit early and draw NOTHING
	ldy #3			; Loc@0, Type@1, UnlockType@2, -->UnlockCost@3<--
	lda (zpInteractingWithThisObjPtr),y	; Get the UnlockCost
	sta zpByteParam1			; Param1 = cost
	beq ?dvlNoRight

	ldy #0
	lda #6						; How many blank spots are there
	sec
	sbc zpByteParam1			; (6 - cost) / 2
	lsr 						; divide by 2
	sta zpByteParam2
	tax
	beq ?dvl_DrawLockChar		; Check if there are blanks to draw

	; Draw blanks
	ldy #0
	lda #77			; Left side of lock bar =
?dvlLeftBlank:
	sta (zpScreenPtr),y
	iny
	dex
	bne ?dvlLeftBlank

?dvl_DrawLockChar:
	; Draw the lock characters
	ldx zpByteParam1
	lda #101		; The lock character
?drawNextLockChar:	
	sta (zpScreenPtr),y
	iny
	dex
	bne ?drawNextLockChar

	; Draw blanks on the right
	ldx zpByteParam2
	beq ?dvlNoRight
	lda #77			; Left side of lock bar =
?dvlRightBlank:
	sta (zpScreenPtr),y
	iny
	dex
	bne ?dvlRightBlank
?dvlNoRight:
	rts

; =========================================================
; Draw switch power bar ALWAYS POWER LOCKED
; 1,5 offset inside the chest
; ======
; 2,4 or 6
DrawSwitchLock:
	lda     zpToPtrL
	ldx     zpToPtrH
	clc
	adc     #(5*40+1)				; 1,6
	bcc     ?dvlNoCarry
	inx
?dvlNoCarry:
	sta     zpScreenPtrL
	stx     zpScreenPtrH

	jmp SideEntryForPowerBarDraw

; =========================================================
.local
DrawExitLock:
	lda     zpToPtrL
	ldx     zpToPtrH
	clc
	adc     #(4*40)				; 0,4
	bcc     ?delNoCarry
	inx
?delNoCarry:
	sta     zpScreenPtrL
	stx     zpScreenPtrH

	; (ScreenPtr) = location where to start drawing

	lda #0
	sta zpWorkI			; Counter of how many lock items we have drawn

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
	beq ?delDrawOpenDoor

	; Something but lock type NONE has been set
	; Check the total unlock cost
	; if (lockCost0 + lockCost1 == 0) drawOpenDoor()
	ldy #3			; Loc@0, Type@1, UnlockType0@2, UnlockCost0@3, UnlockType0@4, UnlockCost0@5
	lda (zpInteractingWithThisObjPtr),y	; Get the UnlockCost0
	sta zpByteParam2

	ldy #5			; Loc@0, Type@1, UnlockType0@2, UnlockCost0@3, UnlockType0@4, UnlockCost0@5
	lda (zpInteractingWithThisObjPtr),y	; Get the UnlockCost1
	sta zpByteParam3
	clc
	adc zpByteParam2
	beq ?delDrawOpenDoor			; Not locked anymore. Draw the open door

	; There are some locks in play
	; Draw from right to the left
	ldy #(160+7)					; This is where we start drawing, going left
	ldx zpByteParam0				; char2Draw = LockTypeChar[ByteParam0]
	lda LockTypeChar,x				; Which lock character to draw
	ldx zpByteParam2				; How many lock chars to draw
	beq ?noType0Draw				; if UnlockCost0 == 0 then skip this drawing
	; Make sure the lock cost is below 8
	cpx #9
	bcc ?delCostBelow8
	ldx #8
?delCostBelow8:
	sta (zpScreenPtr),y
	dey
	inc zpWorkI						; ++ number of chars drawn
	dex
	bne ?delCostBelow8

?noType0Draw:
	; Draw lock type 1
	lda zpWorkI						; if WorkI == 8 then stop drawing the bar
	cmp #8
	bcs ?delStopDrawingTheBar

	ldx zpByteParam1				; char2Draw = LockTypeChar[ByteParam2]
	lda LockTypeChar,x				; Which lock character to draw
	ldx zpByteParam3				; How many lock chars to draw
	beq ?noType1Draw				; if UnlockCost1 == 0 then skip this drawing
	; Make sure the lock cost is below 8
	cpx #9
	bcc ?delCost1Below8
	ldx #8
?delCost1Below8:
	; Now make sure that the bar does not grow past 8 positions
	sta zpWorkVal					; char to draw temp storage
	
?delDrawType1BarLoop:
	lda zpWorkI
	cmp #9
	bcs ?delStopDrawingTheBar

	lda zpWorkVal
	sta (zpScreenPtr),y
	dey
	inc zpWorkI						; ++ number of chars drawn
	dex
	bne ?delDrawType1BarLoop

?delStopDrawingTheBar:

?noType1Draw:
	; Fill the lock indicator to at least level 6
	lda #77				; empty bar
	ldx zpWorkI
?delFillLoop:	
	cpx #6
	bcs ?overTheLimitOf6
	sta (zpScreenPtr),y
	dey
	inx
	bne ?delFillLoop

?overTheLimitOf6:	
	rts

?delDrawOpenDoor	
	ldy #4
	lda #70		; Door Top left
	sta (zpScreenPtr),y

	ldy #(4+40)
	lda #111	; OP of open
	sta (zpScreenPtr),y
	iny
	lda #112	; EN of open
	sta (zpScreenPtr),y

	ldy #(4+40+40)
	lda #128		; Door left bottom
	sta (zpScreenPtr),y

	rts