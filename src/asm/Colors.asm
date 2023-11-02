DetectPALorNTSC
	lda PAL
	cmp #$F		; Check for NTSC
	bne ?isPAL
	; NTSC

	lda #$00
	sta COLOR4			; Background color
	lda #$1A+$10
	sta COLOR0
	lda #$08
	sta COLOR1
	lda #$0C
	sta COLOR2
	lda #$86+$10
	sta COLOR3

	lda #$22+$20
	sta ColorBarBase

	lda #$B6+$20
	sta ColorBarBase+1

	lda #$84+$20
	sta ColorBarBase+2
	rts
	
	; PAL
?isPAL
	lda #$00
	sta COLOR4			; Background color
	lda #$1A			; 
	sta COLOR0
	lda #$08
	sta COLOR1
	lda #$0C
	sta COLOR2
	lda #$86
	sta COLOR3

	rts