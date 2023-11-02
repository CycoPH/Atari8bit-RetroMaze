; =============================================================================
; BOOT SCREEN
; Shown directly as the game is loaded.
; Title screen on top
; Abbuc message
; Fire to start message
; "Boot_part2.asm" loads the loading screen @ $A800
BootDisplayList:
	;.byte $70,$70,$70       	; 24 blank lines
	;.byte $70,$70,$70,$70,$70,$70,$70,$70,$70
	;.byte DL_LMS|DL_TEXT_7, <BootScreenData, >BootScreenData
	;.byte $70,DL_TEXT_4,DL_TEXT_4,
	;.byte $41,<BootDisplayList,>BootDisplayList ; JVB ends display list

	.byte $70,$70,$70       	; 24 blank lines
	.byte DL_LMS|DL_TEXT_2, 
	;.byte <$A800, >$A800 ; 1x mode 2 line + LMS + address
	.byte <$BC40, >$BC40
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2	; 5 lines
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2 ; 5 lines
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2 ; 5 lines
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2 ; 5 lines
	.byte DL_DLI|$70
	.BYTE DL_LMS|DL_TEXT_4, <BootScreenData, >BootScreenData
	.byte DL_TEXT_4
	.byte $41,<BootDisplayList,>BootDisplayList 	; JVB ends display list	

; ---------------------------------------------------------
; Used during the intro
; Interrupt gets hit near the bottom of the screen
; - Wait for WSYNC
; - change the font
; - change the color
DLIForBootScreen
    pha				; save A & X registers to stack
	txa
	pha

	; Setup the game screen font and color
	lda #>GuiFont
	sta WSYNC
	sta CHBASE
	lda #$1A+16
BootScreenColorFix = *-1
	sta COLPF2
	lda #0
	sta COLBK		; store it in the hardware register
	lda #$7
	sta COLPF1		; Background color
	
	pla				; restore X & A registers from stack
	tax
	pla
	rti	

; =============================================================================

; =============================================================================
; Display lists are placed into the next bytes
GameDisplayList:
	.byte $70,$70,$70       	; 24 blank lines
	.byte DL_LMS|DL_TEXT_4, <GameGui, >GameGui, DL_DLI|DL_TEXT_4
	.byte DL_TEXT_4 ; GUI 3x mode 4 line + LMS + address
	; Game screen section
	; New memory address + DLI
	.byte DL_LMS|DL_TEXT_2 
GameScreenAddr:					; Write the game screen address into this location
	.byte <GameScreen0, >GameScreen0 ; 1x mode 2 line + LMS + address
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2	; 5 lines
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2 ; 5 lines
	.byte DL_TEXT_2, DL_TEXT_2, 
	.byte DL_DLI|DL_TEXT_2
	.byte DL_TEXT_2, DL_TEXT_2 ; 5 lines
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2 ; 5 lines
	.byte $41,<GameDisplayList,>GameDisplayList ; JVB ends display list


ColorBarLuma	.byte 0,254,254,254,254,2,2,2
ColorBarBase	.byte $22, $B6, $84, $04

.local
; =============================================================================
; Display list interrupt
; Switches in the required font
; Red = 34
; Green = 182
; Blue = 132
DLIForGameScreenPart1Of2:
	pha				; save A & X registers to stack
	txa
	pha

	ldx zpPowerBarMode
	bne ?withColor

	ldx #8			; make 8 color changes
?noColorLoop:
	sta WSYNC
	dex
	bne ?noColorLoop
	beq ?setFont

?withColor:
	lda ColorBarBase-1,x
	sta SMCBaseColorOfPowerBar

	; Do a color roll for 8 scan lines
	ldx #7			; make 8 color changes
	lda #4			; initial color
SMCBaseColorOfPowerBar = * - 1

?loop
	sta WSYNC		; first WSYNC gets us to start of scan line we want
	sta COLPF0		; change text color for UPPERCASE characters in gr2
	clc
	adc ColorBarLuma,x	; change color value, making brighter
	dex				; update iteration count
	bne ?loop		; we are still checking result of dex	
	
?setFont:	
	lda #0				; DLI_WhichFont
SMC_WhichFont1: = *-1	
	sta WSYNC       	; any value saved to WSYNC will trigger the pause
	sta CHBASE
	lda #0
	sta COLBK       	; store it in the hardware register
	sta COLPF2			; Background color
	lda #10
GameScreenLumaPtr = *-1
	sta COLPF1			; luma

	lda #<DLIForGameScreenPart2
	sta VDSLST
	lda #>DLIForGameScreenPart2
	sta VDSLST+1

	pla				; restore X & A registers from stack
	tax
	pla
	rti
	
DLIForGameScreenPart2:
	pha					; only using A register, so save old value to the stack
	lda #0				; DLI_WhichFont2
SMC_WhichFont2 = * -1
	sta WSYNC       	; any value saved to WSYNC will trigger the pause
	sta CHBASE
	pla             	; restore the A register
	rti             	; always end DLI with RTI!

; =============================================================================
; =============================================================================

StartDisplayList:
	.byte $70,$70,$70       	; 24 blank lines
	.byte DL_LMS|DL_TEXT_4, <StartGui, >StartGui
	.byte DL_DLI|DL_TEXT_4
	.byte DL_TEXT_4 ; GUI 3x mode 4 line + LMS + address
	; Game screen section
	; New memory address + DLI
	.byte DL_LMS|DL_TEXT_2 
DemoScreenAddr:					; Write the game screen address into this location
	.byte <GameScreen0, >GameScreen0 ; 1x mode 2 line + LMS + address
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2	; 5 lines
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2 ; 5 lines
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2 ; 5 lines
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2 ; 5 lines
	.byte $41,<StartDisplayList,>StartDisplayList ; JVB ends display list



start_color .byte 0
	.local
; ---------------------------------------------------------
; Used during the intro
; Interrupt gets hit on the blank line between the 1st and 2nd lines
; - Wait for WSYNC
; - change the color of the UPPERCASE characters (1 scan line at a time = rainbow)
; - after 8 lines jump into the normal/game DLI to switch 
;	- character font
;	- tile colours
DLIForStartScreen
    pha				; save A & X registers to stack
	txa
	pha

	; Do a color roll for 8 scan lines
	ldx #8			; make 8 color changes
	lda start_color ; initial color
?loop
	sta WSYNC		; first WSYNC gets us to start of scan line we want
	sta COLPF2		; change text color for UPPERCASE characters in gr2
	;sta COLBK
	clc
	adc #1			; change color value, making brighter
	dex				; update iteration count
	bne ?loop		; we are still checking result of dex

	dec start_color	; change starting color for next time

	; Setup the game screen font and color
	lda #>GameFont
	sta WSYNC
	sta CHBASE
	lda #0
	sta COLBK		; store it in the hardware register
	sta COLPF2		; Background color
	lda #10
StartScreenLumaPtr = *-1	
	sta COLPF1		; luma
	
	pla				; restore X & A registers from stack
	tax
	pla
	rti	