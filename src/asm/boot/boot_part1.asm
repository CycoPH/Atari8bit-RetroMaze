; Loader to display a quick message ASAP and turn off BASIC
; NOTE: The DL and text message a relocated to $8000
; Create a one line display list
; Put up a loading message
; Turn off BASIC
; If that fails then change the text and cycle border colours for ever
; The loader will be overridden by the normal game once it has run
.include "OS.asm"
.include "PIA.asm"
.include "GTIA.asm"
BASICF = $3f8

	.bank 0,0
	*=$2000 "Boot, remove BASIC"
	.local
BOOT_LOADER
	lda #0
	sta COLOR0
	sta COLOR1
	sta COLOR2
	sta COLOR3
	jsr WaitForVBI

	; Disable basic
	lda #$C0
	cmp RAMTOP
	beq ?RamOk
	sta RAMTOP
	sta RAMSIZ

	; turn off basic
	lda PORTB
	ora #$02
	sta PORTB

	; Check if BASIC ROM area is now writable
	lda $A000
	inc $A000
	cmp $A000
	beq ?RamNotOk		; No change so BASIC ROM is still there

	lda #1				; Set BASICF to non-zero, so BASIC remains OFF after RESET
	sta BASICF
?RamOk
	rts					; return to LOADER to continue the load process

?RamNotOk
	jsr WaitForVBI
	inc COLOR1			; Change the background color to indicate that something has gone wrong
	jmp ?RamNotOk

; Hang in this loop until one VBI has happend
WaitForVBI
	lda RTCLOK60
?wait
	cmp RTCLOK60
	beq ?wait
	rts

; Setup the INITAD vector to call the code ASAP
	.bank 1
	* = $2e2 "Init Vector"
	.word BOOT_LOADER
