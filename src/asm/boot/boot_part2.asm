.include "OS.asm"
.include "PIA.asm"
.include "GTIA.asm"
.include "ANTIC.asm"

	.bank 0,0
	* = $B000
	.include "../fonts/GameFont.asm"

	*=$2000 "Decompress, DL"

		.local
ZP = $F0
ZX5_OUTPUT = ZP + 0
ZX5_INPUT = ZP + 2
zp_zx5_copysrc = ZP + 4
zp_zx5_offset = ZP + 6
zp_zx5_offset2 = ZP + 8
zp_zx5_offset3 = ZP + $A
zp_zx5_len = ZP + $C
zp_zx5_pnb = ZP + $E

* = $8700
unZX5
	lda #$ff
	sta zp_zx5_offset
	sta zp_zx5_offset+1
	ldy #$00
	sty zp_zx5_len
	sty zp_zx5_len+1
	lda #$80

?dzx5s_literals
	jsr ?dzx5s_elias
	pha
?cop0
	jsr zx5_get_byte
	ldy #$00
	sta (ZX5_OUTPUT),y
	; inw ZX5_OUTPUT
	inc ZX5_OUTPUT
	bne ?skipIncOutput
	inc ZX5_OUTPUT+1
?skipIncOutput
	lda zp_zx5_len
	bne ?skipLen1
	dec zp_zx5_len+1
?skipLen1
  	dec zp_zx5_len
	bne ?cop0
	lda zp_zx5_len+1
	bne ?cop0
	pla
	asl
	bcs ?dzx5s_other_offset

?dzx5s_last_offset
	jsr ?dzx5s_elias
?dzx5s_copy  
	pha
	lda ZX5_OUTPUT
	clc
	adc zp_zx5_offset
	sta zp_zx5_copysrc
	lda ZX5_OUTPUT+1
	adc zp_zx5_offset+1
	sta zp_zx5_copysrc+1
	ldy #$00
	ldx zp_zx5_len+1
	beq ?Remainder
?Page  
	lda (zp_zx5_copysrc),y
	sta (ZX5_OUTPUT),y
	iny
	bne ?Page
	inc zp_zx5_copysrc+1
	inc ZX5_OUTPUT+1
	dex
	bne ?Page
?Remainder   
	ldx zp_zx5_len
	beq ?copyDone
?copyByte
	lda (zp_zx5_copysrc),y
	sta (ZX5_OUTPUT),y
	iny
	dex
	bne ?copyByte
	tya
	clc
	adc ZX5_OUTPUT
	sta ZX5_OUTPUT
	bcc ?copyDone
	inc ZX5_OUTPUT+1
?copyDone 
	stx zp_zx5_len+1
	stx zp_zx5_len
	pla
	asl
	bcc ?dzx5s_literals

?dzx5s_other_offset
	asl
	bne ?dzx5s_other_offset_skip
	jsr zx5_get_byte
	;sec ; can be removed if decompress from memory rather than file
	rol
?dzx5s_other_offset_skip
	bcc ?dzx5s_prev_offset

?dzx5s_new_offset
	sta zp_zx5_pnb
	asl
	ldx zp_zx5_offset2
	stx zp_zx5_offset3
	ldx zp_zx5_offset2+1
	stx zp_zx5_offset3+1
	ldx zp_zx5_offset
	stx zp_zx5_offset2
	ldx zp_zx5_offset+1
	stx zp_zx5_offset2+1
	ldx #$fe
	stx zp_zx5_len
	jsr ?dzx5s_elias_loop
	pha
	ldx zp_zx5_len
	inx
	stx zp_zx5_offset+1
	bne ?nextByte
	pla
	rts   ; The end
?nextByte
	jsr zx5_get_byte
	sta zp_zx5_offset
	ldx #$00
	stx zp_zx5_len+1
	inx
	stx zp_zx5_len
	pla
	dec zp_zx5_pnb
	bmi ?skipBacktrack
	jsr ?dzx5s_elias_backtrack
?skipBacktrack
	; inw zp_zx5_len
	inc zp_zx5_len
	bne ?skipLenIncHigh
	inc zp_zx5_len+1
?skipLenIncHigh	
	jmp ?dzx5s_copy

?dzx5s_prev_offset
	asl
	bcc ?dzx5s_second_offset
	ldy zp_zx5_offset2
	ldx zp_zx5_offset3
	sty zp_zx5_offset3
	stx zp_zx5_offset2
	ldy zp_zx5_offset2+1
	ldx zp_zx5_offset3+1
	sty zp_zx5_offset3+1
	stx zp_zx5_offset2+1

?dzx5s_second_offset
	ldy zp_zx5_offset2
	ldx zp_zx5_offset
	sty zp_zx5_offset
	stx zp_zx5_offset2
	ldy zp_zx5_offset2+1
	ldx zp_zx5_offset+1
	sty zp_zx5_offset+1
	stx zp_zx5_offset2+1
	jmp ?dzx5s_last_offset

?dzx5s_elias 
	inc zp_zx5_len
?dzx5s_elias_loop
	asl
	bne ?dzx5s_elias_skip
	jsr zx5_get_byte
	;sec ; can be removed if decompress from memory rather than file
	rol
?dzx5s_elias_skip
	bcc ?dzx5s_elias_backtrack
	rts
?dzx5s_elias_backtrack
	asl
	rol zp_zx5_len
	rol zp_zx5_len+1
	jmp ?dzx5s_elias_loop

zx5_get_byte
	ldy #0
	lda (ZX5_INPUT),y
	inc ZX5_INPUT
	bne ?skipIncInput
	inc ZX5_INPUT+1
?skipIncInput	
	rts

;zx5_get_byte
;	lda  $ffff
;ZX5_INPUT = *-2
;	;inw  ZX5_INPUT
;	inc ZX5_INPUT
;	bne ?skipIncInput
;	inc ZX5_INPUT+1
;?skipIncInput	
;	rts
ZX5_END

	.local

BOOT_PART2:
	lda #>GameFont		; This is the gui font for the top of the screen
	sta CHBAS

	lda #10
	sta COLOR1
	lda #$86
	sta COLOR3

	; Setup display list
	lda #<LoaderDisplayList
	sta SDLSTL
	lda #>LoaderDisplayList
	sta SDLSTL+1

	lda #$00
	sta COLOR2			; Background color to black

	rts

	;*=$A000 "Boot Display List"
LoaderDisplayList
	.byte $70,$70,$70       	; 24 blank lines
	.byte DL_LMS|DL_TEXT_2, 
	.byte <GameScreen0, >GameScreen0 ; 1x mode 2 line + LMS + address
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2	; 5 lines
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2 ; 5 lines
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2 ; 5 lines
	.byte DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2, DL_TEXT_2 ; 5 lines
	.byte $41,<LoaderDisplayList,>LoaderDisplayList 	; JVB ends display list

	;*=$A800 "Boot screen"
	* = $BC40	; Load into high mem and out of the way
GameScreen0:
	.BYTE 1,9,9,9,9,9,9,9
	.BYTE 9,9,9,9,9,9,9,9
	.BYTE 9,9,9,9,9,9,9,9
	.BYTE 9,9,9,9,9,9,9,9
	.BYTE 9,9,9,9,9,9,9,2
	.BYTE 10,70,128,98,0,70,128,98
	.BYTE 0,70,128,98,0,70,128,98
	.BYTE 0,70,128,98,0,0,70,0
	.BYTE 0,0,98,0,70,128,98,0
	.BYTE 70,128,128,0,70,128,98,10
	.BYTE 10,128,189,128,0,128,0,0
	.BYTE 0,0,128,0,0,128,189,128
	.BYTE 0,128,0,128,0,0,128,73
	.BYTE 0,72,128,0,128,189,128,0
	.BYTE 0,72,128,0,128,0,0,10
	.BYTE 10,128,128,59,0,128,128,0
	.BYTE 0,0,128,0,0,128,128,59
	.BYTE 0,128,0,128,0,0,128,201
	.BYTE 128,200,128,0,128,128,128,0
	.BYTE 72,128,200,0,128,128,0,10
	.BYTE 10,128,73,0,0,128,0,0
	.BYTE 0,0,128,0,0,128,73,0
	.BYTE 0,128,0,128,0,0,128,0
	.BYTE 0,0,128,0,128,0,128,0
	.BYTE 128,200,0,0,128,0,0,10
	.BYTE 10,58,201,73,0,58,128,59
	.BYTE 0,0,128,0,0,58,201,73
	.BYTE 0,58,128,59,0,0,58,0
	.BYTE 0,0,59,0,58,0,59,0
	.BYTE 128,128,59,0,58,128,59,10
	.BYTE 27,9,9,9,9,9,9,9
	.BYTE 9,9,9,9,9,5,9,9
	.BYTE 9,9,9,9,9,9,9,9
	.BYTE 9,9,6,9,9,9,9,9
	.BYTE 9,9,9,9,9,9,9,28
	.BYTE 10,0,0,0,0,0,10,0
	.BYTE 0,0,0,0,10,11,15,14
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 13,16,12,10,0,0,0,0
	.BYTE 0,10,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,0,10,0
	.BYTE 0,0,0,0,10,0,0,11
	.BYTE 17,5,9,9,9,9,6,18
	.BYTE 12,0,0,10,0,0,0,0
	.BYTE 0,10,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,0,10,0
	.BYTE 0,0,0,0,10,0,0,0
	.BYTE 10,11,0,0,0,0,12,10
	.BYTE 0,0,0,10,0,0,0,0
	.BYTE 0,10,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,0,10,0
	.BYTE 0,0,0,0,10,0,0,0
	.BYTE 10,0,0,0,0,0,0,10
	.BYTE 0,0,0,10,0,0,0,0
	.BYTE 0,10,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,0,10,0
	.BYTE 0,0,0,0,10,0,0,0
	.BYTE 10,13,0,0,0,0,14,10
	.BYTE 0,0,0,10,0,0,0,0
	.BYTE 0,10,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,0,10,0
	.BYTE 0,0,0,0,10,0,0,13
	.BYTE 21,12,0,0,0,0,11,22
	.BYTE 14,0,0,10,0,0,0,0
	.BYTE 0,10,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,0,10,0
	.BYTE 0,0,0,0,10,13,16,12
	.BYTE 64,65,0,0,0,0,64,65
	.BYTE 11,15,14,10,0,0,0,0
	.BYTE 0,10,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,0,10,0
	.BYTE 0,0,0,13,21,12,0,0
	.BYTE 10,209,137,133,134,137,210,10
	.BYTE 0,0,11,22,14,0,0,0
	.BYTE 0,10,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,0,10,0
	.BYTE 0,13,16,12,0,0,0,0
	.BYTE 10,138,214,138,138,189,138,10
	.BYTE 0,0,0,0,11,15,14,0
	.BYTE 0,10,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,0,10,13
	.BYTE 16,12,0,0,0,0,0,0
	.BYTE 66,147,137,212,211,137,148,67
	.BYTE 0,0,0,0,0,0,11,15
	.BYTE 14,10,0,0,0,0,0,10
	.BYTE 10,0,0,0,0,13,21,12
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 0,128,128,192,193,128,128,0
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 11,22,14,0,0,0,0,10
	.BYTE 10,0,0,13,16,12,0,0
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 0,128,128,183,176,128,128,0
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 0,0,11,15,14,0,0,10
	.BYTE 10,13,16,12,0,0,0,0
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 0,58,128,139,140,128,59,0
	.BYTE 0,0,0,0,0,0,0,0
	.BYTE 0,0,0,0,11,15,14,10
	.BYTE 3,7,9,9,9,9,9,9
	.BYTE 9,9,9,9,9,9,9,9
	.BYTE 9,9,9,9,9,9,9,9
	.BYTE 9,9,9,9,9,9,9,9
	.BYTE 9,9,9,9,9,9,8,4

; Setup the INITAD vector to call the code ASAP
	.bank 1
	* = $2e2 "Init Vector"
	.word BOOT_PART2