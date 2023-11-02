; 32-bit score counter

; --------------------------------------------------------
; DRAW SCORE
; 0 1 2 3 4 5 6
; 16 is for base color
; 144 is for highlighted color
DrawScore	
	ldx zpScore
	cpx zpLastScore
	beq ?noDrawScore0
	; 10s
	stx zpLastScore
	lda Div10Table,x
	clc
	adc #144
	sta ScoreIndicator+5
	; 1s
	lda Modulus10Table,x
	adc #144
	sta ScoreIndicator+6
?noDrawScore0:
	ldx zpScore+1
	cpx zpLastScore+1
	beq ?noDrawScore1
	stx zpLastScore+1
	; 1000s
	lda Div10Table,x
	clc
	adc #144
	sta ScoreIndicator+3
	; 100s
	lda Modulus10Table,x
	adc #144
	sta ScoreIndicator+4
?noDrawScore1
	ldx zpScore+2
	cpx zpLastScore+2
	beq ?noDrawScore2
	stx zpLastScore+2
	; 100000s
	lda Div10Table,x
	clc
	adc #144
	sta ScoreIndicator+1
	; 10000s
	lda Modulus10Table,x
	adc #144
	sta ScoreIndicator+2
?noDrawScore2	
	; Just draw it, not saving much by skipping it
	ldx zpScore+3
	lda Modulus10Table,x
	clc
	adc #144
	sta ScoreIndicator
	rts

; --------------------------------------------------------
Add10ToScore:
	lda #10
	;jmp Add2Score

; --------------------------------------------------------
; ADD SCORE
; Add Acc to the "score"
Add2Score:
	;!##TRACE "add score xy=%d,%d,%d,%d + %d" db(score) db(score+1) db(score+2) db(score+3) @a
	clc					; a = a + score[0] 
	adc zpScore
	cmp #100			; if a >= 100
	bcc ?below100Lo		; set carry
	sbc #100			; a = a - 100
?below100Lo:		
	sta zpScore			; store the remainder in score
	bcc ?endScoreAdd	; if no carry is set then no overflow past 100

	; There was overflow past 100 add 1 to the next byte of the score
	lda zpScore+1		; a = score[1]
	adc #0				; a = a + 1
	cmp #100			; if a >= 100
	bcc ?below100Hi		;
	sbc #100
?below100Hi		
	sta zpScore+1
	bcc ?endScoreAdd	; if no carry is set then no overflow past 100

	; There was overflow past 100 add 1 to the next byte of the score
	lda zpScore+2		; a = score[1]
	adc #0				; a = a + 1
	cmp #100			; if a >= 100
	bcc ?below100_B2	;
	sbc #100
?below100_B2
	sta zpScore+2
	bcc ?endScoreAdd	; if no carry is set then no overflow past 100

	inc zpScore+3
?endScoreAdd
	rts

	; --------------------------------------------------------
ResetScore:
	lda #0
	sta zpScore
	sta zpScore+1
	sta zpScore+2
	sta zpScore+3
	jmp DrawScore
