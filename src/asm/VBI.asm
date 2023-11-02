; Vertical Blank Interrupt
; Gets executed 50x per second PAL, 60x NTSC
;

VBI_WhichScreen	.byte 0;   0 = GameScreen0 + GameFont, 1 = GameScreen1 + GameFont, 2 = Map Screen + MapFont

VBIFontTable:	.byte >GameFont, >GameFont, >MapFont
VBIFontTable2:	.byte >GameFont, >GameFont, >BackpackFont
VBIScreenTableL	.byte <GameScreen0, <GameScreen1, <MapScreen
VBIScreenTableH	.byte >GameScreen0, >GameScreen1, >MapScreen

DO_VBI_MUSIC = 128

	.local

; =========================================================
; Vertical blank interrupt
; Fired 50/60 times a second

ExitVBIEarly
	JMP	XITVBV

VBINTSCDelay .byte 6

; Vertical Blank Interrupt entry point
VBI
	inc zpVBICounter

	lda zpVbiMode				; 0 = start screen, 1 = game screen, 2 = nothing
	and #3
	beq ?VBIForTheStart
	cmp #2
	beq ?commonVBI		

	; zpVBIMode == 1 (Game mode)
	lda #<GameDisplayList
	sta DLISTL
	lda #>GameDisplayList
	sta DLISTH

	; Reset the DLI to fire the first one
	lda #<DLIForGameScreenPart1Of2
	sta VDSLST
	lda #>DLIForGameScreenPart1Of2
	sta VDSLST+1

	; Set which font and screen are going to the shown
	ldx VBI_WhichScreen
	lda VBIFontTable,x
	sta SMC_WhichFont1
	lda VBIFontTable2,x
	sta SMC_WhichFont2

	; Set which screen will be shown next
	lda VBIScreenTableL,x
	sta GameScreenAddr
	lda VBIScreenTableH,x
	sta GameScreenAddr+1

?commonVBI:
	; Check for PAL/NTSC slowdown
	lda PAL
	cmp #$F
	bne ?doSomeMusicAction	; -> When PAL
	; NTSC
	dec VBINTSCDelay
	bne ?doSomeMusicAction
	; 1 in 6 we skip the Music player
	lda #6
	sta VBINTSCDelay
	;!##TRACE "skip Frame=%d" db($14)
	jmp ?noMusic

?doSomeMusicAction:
	jsr PlayMusic

?noMusic:
	; End the Vertical Blank Interrupt
	JMP	XITVBV

?VBIForTheStart:
	; 
	lda #<StartDisplayList
	sta DLISTL
	lda #>StartDisplayList
	sta DLISTH

	; Reset the DLI to fire the first one
	lda #<DLIForStartScreen
	sta VDSLST
	lda #>DLIForStartScreen
	sta VDSLST+1

	; Set which font and screen are going to the shown
	ldx VBI_WhichScreen
	lda VBIFontTable,x
	sta SMC_WhichFont1
	lda VBIFontTable2,x
	sta SMC_WhichFont2

	; Set which screen will be shown next
	lda VBIScreenTableL,x
	sta DemoScreenAddr
	lda VBIScreenTableH,x
	sta DemoScreenAddr+1

	jmp ?commonVBI
