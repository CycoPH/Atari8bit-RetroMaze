; General delay functions
WaitFor60Cycles:
	lda #60
	jmp ?startTheWait

WaitFor30Cycles:
	lda #30
?startTheWait:	
	sta zpWaitCounter

?waitLoop:	
	lda	zpWaitCounter
	beq	?endOfWait

	; do the waiting and read the joystick
	jsr WaitForVBI
	jsr ReadJoystick

	lda zpJoystick0
	bne ?endOfWait

	dec	zpWaitCounter
	jmp ?waitLoop


?endOfWait:
	jsr WaitForFireRelease

	lda	#$FF		; Clear keyboard click
	sta	CH			; KBCODE = $FF

	rts

.local
WaitFor30CyclesNoShortcut:
	lda #30
?startTheWait:
	sta zpWaitCounter

?waitLoop:	
	lda	zpWaitCounter
	beq	?endOfWait

	; do the waiting and read the joystick
	jsr WaitForVBI

	dec	zpWaitCounter
	jmp ?waitLoop

?endOfWait:
	lda	#$FF		; Clear keyboard click
	sta	CH			; KBCODE = $FF

	rts