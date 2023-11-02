BootScreenData:
	.sbyte       "       ABBUC SOFTWARE CONTEST 2023      "
	.sbyte +128, "             FIRE TO START              "

	.local
WaitForFireDownAndUp:
?waitForPress:
	lda STRIG0
	bne	?waitForPress

?waitForRelease:
	lda STRIG0
	beq	?waitForRelease

	; Clear the text before we do anything else
	ldx #79
	lda #0
?waitClearLoop:
	sta BootScreenData,X
	dex
	bpl ?waitClearLoop

	rts


