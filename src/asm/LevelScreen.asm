.local
; The data to draw the "Level" text onto the screen
; 18x6 (108 bytes)
; All 0 bytes are not drawn
LevelTextData:
	.BYTE 70,0,0,0,0,0,0,0
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 0,70,128,0,0,0,0,70
	.BYTE 128,98,0,70,0,98,0,70
	.BYTE 128,98,0,128,128,0,0,0
	.BYTE 0,128,189,128,0,128,0,128
	.BYTE 0,128,189,128,0,128,128,0
	.BYTE 0,0,0,128,128,59,0,128
	.BYTE 0,128,0,128,128,59,0,128
	.BYTE 128,0,0,0,0,128,0,0
	.BYTE 0,128,0,128,0,128,0,0
	.BYTE 0,128,128,128,128,59,0,58
	.BYTE 128,59,0,201,99,200,0,58
	.BYTE 128,59,0,59

; Render a 6x6 LARGE number on the screen
; zpToPtr (location of the top left corner)
; Source of the font data is Font4 (BackpackFont) $2C00
; 0 digit starts @ $2C00 + $80 (128) 16th char in the font
; Skip the last and the first and last bits.
; Each bit (64,32,16,8,4,2) is rendered as char 0 or 128
; Acc is the digit to draw: 0,1,2...9
RenderLargeNumber:
	asl					; Acc = Acc * 8
	asl
	asl
	clc
	adc #$80			; Acc + 128 (get location of first line of font data to draw)
	tax					; x is the font offset

	lda #7
	sta zpWorkY			; 7 lines of the font data to render

?drawNextLineOfFont:
	lda #64
	sta SMC_WhatBitToTest	; Start with testing the left most bit

	ldy #0
	lda BackpackFont, x		; WorkVal = GuiFont[WorkI]
	sta zpWorkVal

?nextBitOnLine:	
	and #64				; Check the left most bit
SMC_WhatBitToTest = * - 1	
	beq ?noDrawForThisBit

	; Draw the char
	lda #128
	sta (zpToPtr),y		; [zpToPtr] = 128

?noDrawForThisBit:
	lsr SMC_WhatBitToTest		; 32 => 16 => 8 => 4 => 2 => 1
	lda zpWorkVal		; A = the font data
	iny
	cpy #6
	bcc ?nextBitOnLine	; Less then 6 bits/chars done then do the next one

	; Move the draw ptr one line down
	clc
	lda zpToPtrL
	adc #40
	sta zpToPtrl
	bcc ?noCarryInRenderLargeNumber
	inc zpToPtrH
?noCarryInRenderLargeNumber:

	inx
	dec zpWorkY
	bne ?drawNextLineOfFont

	rts

; Draw to zpPagePtr to put something onto the screen
RenderLevelX:
	lda zpPagePtr				; zpToPtr = screen[11,6]
	clc
	adc #<(11 + 6*40)
	sta zpToPtr
	lda zpPagePtr+1
	adc #>(11 + 6*40)
	sta zpToPtr+1

	lda #<LevelTextData			; zpFromPtr = LevelTextData (the bytes to render, skipping 0)
	sta zpFromPtrL
	lda #>LevelTextData
	sta zpFromPtrH

	ldy #0						; # of bytes to render
	; 18x6
	lda #6
	sta zpWorkY					; zpWorkY = 6 vertial lines counter

?nextLineStart:
	ldx #18						; X = counter for the 18 chars on a line

?nextChar:
	lda (zpFromPtr),y			; Get the char to put onto the screen
	beq ?doDrawThisChar			; If it is 0 then skip the draw
	sta (zpToPtr),y				; Put it on the screen
?doDrawThisChar:
	iny							; Next src and dest
	dex							; --in line counter
	bne ?nextChar				; Not zero -> render next char on line

	; Y is the source data ptr but is also used for the target draw
	; normally we would advance by 40 to get to the next line, now
	; only advance by the width of the line data
	lda zpToPtr
	clc
	adc #40-18					; 22 bytes per line
	sta zpToPtr
	bcc ?noCarryThisTime
	inc zpToPtrH
?noCarryThisTime:

	dec zpWorkY
	bne ?nextLineStart

	; Level is done.  Now render the level number as a large digit
	; one or two digits
	ldx zpLevelNr
	cpx #10
	bcs ?LevelIs10OrAbove
	
	; Single digit level number
	; 6 chars wide
	lda zpPagePtr				; zpToPtr = screen[11,7]
	clc
	adc #<(18 + 13*40)
	sta zpToPtr
	lda zpPagePtr+1
	adc #>(18 + 13*40)
	sta zpToPtr+1

	; 1s
	lda Modulus10Table,x
	jmp RenderLargeNumber

?LevelIs10OrAbove:
	; Single digit level number
	; 6 chars wide
	lda zpPagePtr				; zpToPtr = screen[11,7]
	clc
	adc #<(13 + 13*40)
	sta zpToPtr
	lda zpPagePtr+1
	adc #>(13 + 13*40)
	sta zpToPtr+1

	; 10s
	lda Div10Table,x
	jsr RenderLargeNumber

	lda zpPagePtr				; zpToPtr = screen[11,7]
	clc
	adc #<(20 + 13*40)
	sta zpToPtr
	lda zpPagePtr+1
	adc #>(20 + 13*40)
	sta zpToPtr+1

	ldx zpLevelNr
	lda Modulus10Table,x
	jmp RenderLargeNumber


; 16x6
DoneTextData:
	.BYTE 128,128,128,91,0,0,0,0
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 128,0,219,128,0,90,128,91
	.BYTE 0,91,0,128,0,90,128,91
	.BYTE 128,0,0,128,0,128,94,128
	.BYTE 0,128,91,128,0,128,224,128
	.BYTE 128,0,0,128,0,128,0,128
	.BYTE 0,128,128,128,0,128,128,93
	.BYTE 128,0,221,128,0,128,95,128
	.BYTE 0,128,92,128,0,128,0,0
	.BYTE 128,128,128,93,0,92,128,93
	.BYTE 0,128,0,92,0,92,128,93

	.local
; Draw to zpPagePtr to put something onto the screen
RenderLevelDone:
	lda #<(MapMenuScreen+12+40)	; zpToPtr = MapScreen[12,15]
	sta zpToPtr
	lda #>(MapMenuScreen+12+40)
	sta zpToPtr+1

	lda #<DoneTextData			; zpFromPtr = DoneTextData (the bytes to render, skipping 0)
	sta zpFromPtrL
	lda #>DoneTextData
	sta zpFromPtrH

	ldy #0						; # of bytes to render
	; 16x6
	lda #6
	sta zpWorkY					; zpWorkY = 6 vertial lines counter

?nextLineStartDone:
	ldx #16						; X = counter for the 18 chars on a line

?nextCharDone:
	lda (zpFromPtr),y			; Get the char to put onto the screen
	beq ?noDrawThisCharDone		; If it is 0 then skip the draw
	sta (zpToPtr),y				; Put it on the screen
?noDrawThisCharDone:
	iny							; Next src and dest
	dex							; --in line counter
	bne ?nextCharDone			; Not zero -> render next char on line

	; Y is the source data ptr but is also used for the target draw
	; normally we would advance by 40 to get to the next line, now
	; only advance by the width of the line data
	lda zpToPtr
	clc
	adc #40-16					; 24 bytes per line
	sta zpToPtr
	bcc ?noCarryThisTime
	inc zpToPtrH
?noCarryThisTime:

	dec zpWorkY
	bne ?nextLineStartDone
	rts

; =========================================================
; 40x7 to be shown at the bottom of the map screen when all the levels are done
TheEndScreen:
	.BYTE 90,128,128,128,91,0,90,0
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 90,128,128,128,91,0,0,0
	.BYTE 0,0,0,0,0,0,91,0
	.BYTE 0,91,0,50,37,52,50,47
	.BYTE 218,219,128,218,219,0,128,0
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 128,218,0,0,0,0,0,0
	.BYTE 0,0,0,0,0,0,128,0
	.BYTE 0,128,0,45,33,58,37,0
	.BYTE 0,0,128,0,0,0,128,128
	.BYTE 128,91,0,90,128,91,0,0
	.BYTE 128,220,0,0,0,0,90,90
	.BYTE 128,91,0,90,128,128,128,0
	.BYTE 0,128,0,34,57,0,0,0
	.BYTE 0,0,128,0,0,0,128,218
	.BYTE 219,128,0,128,224,128,0,0
	.BYTE 128,128,128,91,0,0,128,218
	.BYTE 219,128,0,128,218,219,128,0
	.BYTE 0,128,0,48,37,52,37,50
	.BYTE 0,0,128,0,0,0,128,0
	.BYTE 0,128,0,128,128,93,0,0
	.BYTE 128,218,0,0,0,0,128,0
	.BYTE 0,128,0,128,0,0,128,0
	.BYTE 0,92,0,40,41,46,58,0
	.BYTE 0,0,128,0,0,0,128,0
	.BYTE 0,128,0,128,0,0,0,0
	.BYTE 128,220,0,0,0,0,128,0
	.BYTE 0,128,0,128,220,221,128,0
	.BYTE 0,121,0,0,0,0,0,0
	.BYTE 0,0,93,0,0,0,128,0
	.BYTE 0,128,0,92,128,93,0,0
	.BYTE 92,128,128,128,91,0,128,0
	.BYTE 0,128,0,92,128,128,93,0
	.BYTE 0,122,0,0,0,0,0,0

.local	
RenderTheEnd:
	lda #<MapMenuScreen	; zpToPtr = MapScreen[0,0]
	sta zpToPtr
	lda #>MapMenuScreen
	sta zpToPtr+1

	lda #<TheEndScreen			; zpFromPtr = DoneTextData (the bytes to render, skipping 0)
	sta zpFromPtrL
	lda #>TheEndScreen
	sta zpFromPtrH

	ldy #0						; # of bytes to render

?nextCharDone:
	lda (zpFromPtr),y			; Get the char to put onto the screen
	sta (zpToPtr),y				; Put it on the screen
	iny							; Next src and dest
	bne ?nextCharDone			; Not zero -> render next char on line

	; now we've done 256 bytes
	; 40*7-256 = 24 bytes to go

	inc zpToPtr+1
	inc zpFromPtrH

	ldx #24
?nextCharDoneMini:
	lda (zpFromPtr),y			; Get the char to put onto the screen
	sta (zpToPtr),y				; Put it on the screen
	iny							; Next src and dest
	dex
	bne ?nextCharDoneMini		; Not zero -> render next char on line

	rts	