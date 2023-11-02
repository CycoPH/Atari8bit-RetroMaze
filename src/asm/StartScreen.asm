

StartGui:
	.BYTE 234,158,159,0,73,202,203,0
	.BYTE 0,0,0,0,85,86,0,50
	.BYTE 37,52,50,47,0,45,33,58
	.BYTE 37,0,213,214,0,0,0,0
	.BYTE 0,73,74,75,0,106,30,31
	.BYTE 235,142,143,0,76,205,206,0
	.BYTE 0,238,239,176,165,180,165,178
	.BYTE 0,168,169,174,186,0,97,0
	.BYTE 163,165,178,165,162,181,179,0
	.BYTE 0,76,77,78,0,107,14,15
	.BYTE 108,28,29,0,79,80,81,172
	.BYTE 165,182,165,172,154,0,17,0
	.BYTE 0,164,169,166,166,169,163,181
	.BYTE 172,180,185,154,37,33,58,57
	.BYTE 0,79,80,81,0,108,28,29

StartGuiLevelIndicator = StartGui + 13 + 2*40
StartGuiDifficulty = StartGui + 28 + 2*40

	.local
ShowTheStartScreen:
 	jsr WaitForVBI
	lda #0
	sta zpVbiMode
	jsr WaitForVBI
		
	lda #1
	sta zpLevelNr

	; Can fall through to the render routine
	;jsr DrawSelectedLevelNr
	;rts


DrawSelectedLevelNr:
	; Level nr
	lda #0
	sta StartGuiLevelIndicator
	clc

	ldx zpLevelNr
	; 10s
	lda Div10Table,x
	beq ?no10s
	adc #16
	sta StartGuiLevelIndicator
?no10s:
	; 1s
	lda Modulus10Table,x
	adc #16
	sta StartGuiLevelIndicator+1
	rts

DifficultyText4:
	.byte 	57,58,33,37			;Easy in reverse
	.byte	0,36,41,45			;Mid
	.byte	36,50,33,40			;Hard
	
DrawSelectedDifficultyLevel:
	lda zpDifficulty
	asl	; *2
	asl	; *4
	tax
	ldy #3
?DifficultyDrawLoop:
	lda DifficultyText4,x
	sta StartGuiDifficulty,y
	inx
	dey
	bpl ?DifficultyDrawLoop

	rts





FadeOutStartScreen:
	ldx #10
?nextFadeOutStep:
	ldy #1
?miniOutWaitLoop:	
	jsr WaitForVBI		; Clobbers Acc
	dey
	bpl ?miniOutWaitLoop
	stx StartScreenLumaPtr
	dex
	bpl ?nextFadeOutStep

	rts


FadeInStart:
	ldx #0
?nextFadeInStep:
	ldy #1
?miniInWaitLoop:	
	jsr WaitForVBI		; Clobbers Acc
	dey
	bpl ?miniInWaitLoop
	stx StartScreenLumaPtr
	inx
	cpx #11
	bcc ?nextFadeInStep

	rts

; Press Fire in a 36x6 bitmap (216 bytes)
; All 0 bytes are not drawn
PressFireData:
	.BYTE 70,128,128,98,0,0,0,0
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 0,0,0,0,0,0,70,128
	.BYTE 128,59,0,0,0,0,0,0
	.BYTE 0,0,0,0,128,0,0,128
	.BYTE 0,70,128,98,0,70,128,98
	.BYTE 0,70,128,98,0,70,128,98
	.BYTE 0,0,128,0,0,0,0,61
	.BYTE 0,70,128,98,0,70,128,98
	.BYTE 128,0,0,128,0,128,97,99
	.BYTE 0,128,189,128,0,128,0,0
	.BYTE 0,128,0,0,0,0,128,128
	.BYTE 59,0,0,225,0,128,97,99
	.BYTE 0,128,189,128,128,128,128,59
	.BYTE 0,128,0,0,0,128,128,59
	.BYTE 0,58,128,98,0,58,128,98
	.BYTE 0,0,128,0,0,0,0,128
	.BYTE 0,128,0,0,0,128,128,59
	.BYTE 128,0,0,0,0,128,0,0
	.BYTE 0,128,0,0,0,0,0,128
	.BYTE 0,0,0,128,0,0,128,0
	.BYTE 0,0,0,128,0,128,0,0
	.BYTE 0,128,0,0,59,0,0,0
	.BYTE 0,59,0,0,0,58,128,59
	.BYTE 0,58,128,59,0,58,128,59
	.BYTE 0,0,59,0,0,0,0,59
	.BYTE 0,59,0,0,0,58,128,59

; Draw to zpPagePtr to put something onto the screen
RenderPressFire:
	lda zpDemoSeed
	and #1
	beq ?RenderAtTop
	; Draw the Press Fire text at the bottom
	lda #<(2 + 14*40)
	sta SMC_WhereToRenderL
	lda #>(2 + 14*40)
	sta SMC_WhereToRenderH
	jmp ?goRender

?RenderAtTop	
	lda #<(2 + 1*40)
	sta SMC_WhereToRenderL
	lda #>(2 + 1*40)
	sta SMC_WhereToRenderH

?goRender:
	lda zpPagePtr
	clc
	adc #<(2 + 14*40)						;42
SMC_WhereToRenderL = *-1
	sta zpToPtr
	lda zpPagePtr+1
	adc #>(2 + 14*40)
SMC_WhereToRenderH = *-1
	sta zpToPtr+1

	lda #<PressFireData
	sta zpFromPtrL
	lda #>PressFireData
	sta zpFromPtrH

	ldy #0		; # of bytes to render
	; 36x6
	lda #6
	sta zpWorkY		; 6 vertial lines

?nextLineStart:
	ldx #36

?nextChar:
	lda (zpFromPtr),y			; Get the char to put onto the screen
	beq ?doDrawThisChar			; If its 0 then skip the draw
	sta (zpToPtr),y				; Put it on the screen
?doDrawThisChar:
	iny							; Next src and dest
	dex							; --in line counter
	bne ?nextChar				; Not zero -> render next char on line

	lda zpToPtr
	clc
	adc #4
	sta zpToPtr
	bcc ?noCarryThisTime
	inc zpToPtr
?noCarryThisTime:

	dec zpWorkY
	bne ?nextLineStart

	rts	