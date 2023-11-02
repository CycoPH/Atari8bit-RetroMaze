
; Original size: $0920 bytes @ $4000
.local
RMT_SONG_DATA
__start
    .byte "RMT4"
__song_info
    .byte $40            ; Track length = 64
    .byte $06            ; Song speed
    .byte $01            ; Player Frequency
    .byte $01            ; Format version
; ptrs to tables
__ptrInstrumentTbl
    .word __InstrumentsTable       ; start + $0010
__ptrTracksTblLo
    .word __TracksTblLo            ; start + $004e
__ptrTracksTblHi
    .word __TracksTblHi            ; start + $0060
__ptrSong
    .word __SongData               ; start + $08ec

; List of ptrs to instruments
__InstrumentsTable
    .word __Instrument_0		; 0-arp047                        
    .word __Instrument_1		; 1-arp059                        
    .word __Instrument_2		; 2-arp038                        
    .word __Instrument_3		; 3-base-end-of-level             
    .word __Instrument_4		; 4-bass drum                     
    .word __Instrument_5		; 5-bigbass                       
    .word __Instrument_6		; Instrument 04                   
    .word __Instrument_7		; Instrument 05                   
    .word __Instrument_8		; Instrument 0F                   
    .word __Instrument_9		; Instrument 06                   
    .word __Instrument_10		; Instrument 07                   
    .word __Instrument_11		; Instrument 02                   
    .word __Instrument_12		; Instrument 05                   
    .word __Instrument_13		; Instrument 05                   
    .word __Instrument_14		; Instrument 08                   
    .word __Instrument_15		; Instrument 03                   
    .word __Instrument_16		; 16-action                       
    .word __Instrument_17		; 17-warble                       
    .word __Instrument_18		; 18-warble2                      
    .word __Instrument_19		; 19-going up                     
    .word __Instrument_20		; 20-trello                       
    .word __Instrument_21		; 21-ta-short                     
    .word __Instrument_22		; 22-break3                       
    .word __Instrument_23		; 23-tone0                        
    .word __Instrument_24		; 24-tone1                        
    .word __Instrument_25		; 25-end0                         
    .word __Instrument_26		; 26-end1                         
    .word __Instrument_27		; Instrument 03                   
    .word __Instrument_28		; Instrument 07                   
    .word __Instrument_29		; Close to object                 
    .word __Instrument_30		; Enter level                     

__TracksTblLo
    .byte <__Track_00,<__Track_01,<__Track_02,<__Track_03,<__Track_04,<__Track_05,<__Track_06,<__Track_07
    .byte <__Track_08,<__Track_09,$00,$00,$00,$00,$00,$00
    .byte <__Track_10,<__Track_11
__TracksTblHi
    .byte >__Track_00,>__Track_01,>__Track_02,>__Track_03,>__Track_04,>__Track_05,>__Track_06,>__Track_07
    .byte >__Track_08,>__Track_09,$00,$00,$00,$00,$00,$00
    .byte >__Track_10,>__Track_11


; Instrument data

__Instrument_0
    .byte $0e,$0c,$24,$24,$00,$00,$40,$00,$00,$00,$00,$00,$00,$04,$07,$66
    .byte $0a,$00,$55,$0a,$00,$55,$0a,$00,$44,$0a,$00,$33,$0a,$00,$33,$0a
    .byte $00,$33,$0a,$00,$33,$0a,$00
__Instrument_1
    .byte $0e,$0c,$24,$24,$00,$00,$40,$00,$00,$00,$00,$00,$00,$05,$09,$66
    .byte $0a,$00,$55,$0a,$00,$55,$0a,$00,$44,$0a,$00,$33,$0a,$00,$33,$0a
    .byte $00,$33,$0a,$00,$33,$0a,$00
__Instrument_2
    .byte $0e,$0c,$24,$24,$00,$00,$40,$00,$00,$00,$00,$00,$00,$03,$08,$66
    .byte $0a,$00,$55,$0a,$00,$55,$0a,$00,$44,$0a,$00,$33,$0a,$00,$33,$0a
    .byte $00,$33,$0a,$00,$33,$0a,$00
__Instrument_3
    .byte $0c,$0c,$37,$22,$05,$00,$20,$00,$00,$00,$00,$00,$00,$66,$18,$00
    .byte $ff,$06,$00,$ee,$06,$00,$dd,$06,$00,$cc,$06,$00,$bb,$06,$00,$bb
    .byte $06,$00,$55,$06,$00,$55,$06,$00,$44,$06,$00,$44,$06,$00,$44,$06
    .byte $00,$44,$06,$00,$33,$06,$00,$33,$06,$00
__Instrument_4
    .byte $0c,$0c,$25,$25,$00,$00,$00,$00,$00,$00,$00,$00,$00,$77,$18,$08
    .byte $aa,$1a,$c0,$88,$1c,$e0,$55,$1c,$f0,$44,$1a,$f8,$33,$10,$04,$22
    .byte $10,$04,$11,$10,$04,$00,$10,$00
__Instrument_5
    .byte $0c,$0c,$28,$28,$00,$00,$20,$00,$00,$00,$00,$00,$00,$44,$18,$00
    .byte $cc,$0e,$00,$aa,$0e,$00,$77,$0e,$00,$77,$0e,$00,$66,$0e,$00,$66
    .byte $0e,$00,$55,$0e,$00,$55,$0e,$00,$44,$0e,$00
__Instrument_6
    .byte $0c,$0c,$2e,$2e,$00,$00,$00,$00,$00,$00,$00,$00,$00,$ff,$10,$07
    .byte $cc,$1a,$a8,$aa,$1a,$cf,$99,$18,$08,$77,$18,$08,$55,$18,$08,$44
    .byte $18,$08,$33,$18,$08,$22,$18,$08,$22,$18,$08,$11,$18,$08,$00,$10
    .byte $00
__Instrument_7
    .byte $0c,$0c,$28,$28,$00,$00,$00,$00,$00,$00,$00,$00,$00,$ff,$1a,$70
    .byte $bb,$12,$10,$88,$12,$30,$66,$1a,$80,$55,$12,$10,$44,$12,$30,$33
    .byte $12,$10,$22,$12,$30,$11,$12,$10,$00,$12,$30
__Instrument_8
    .byte $0c,$0c,$67,$67,$00,$20,$00,$00,$00,$00,$00,$00,$00,$ff,$02,$00
    .byte $ff,$02,$00,$ee,$02,$00,$ee,$02,$00,$dd,$02,$00,$dd,$02,$00,$cc
    .byte $02,$00,$cc,$02,$00,$bb,$02,$00,$bb,$02,$00,$aa,$02,$00,$aa,$02
    .byte $00,$99,$02,$00,$99,$02,$00,$88,$02,$00,$88,$02,$00,$77,$02,$00
    .byte $77,$02,$00,$66,$02,$00,$66,$02,$00,$55,$02,$00,$55,$02,$00,$44
    .byte $02,$00,$44,$02,$00,$33,$02,$00,$33,$02,$00,$22,$02,$00,$22,$02
    .byte $00,$11,$02,$00,$11,$02,$00,$00,$02,$00
__Instrument_9
    .byte $0c,$0c,$16,$16,$00,$20,$00,$50,$00,$00,$00,$00,$00,$44,$18,$00
    .byte $22,$18,$00,$11,$18,$00,$00,$18,$00
__Instrument_10
    .byte $0e,$0c,$0f,$0f,$01,$00,$40,$00,$00,$00,$00,$00,$03,$07,$00,$55
    .byte $0a,$00
__Instrument_11
    .byte $0e,$0c,$1e,$0f,$05,$00,$22,$00,$00,$02,$00,$00,$f4,$00,$00,$aa
    .byte $0a,$0c,$aa,$0a,$00,$99,$0a,$00,$88,$0a,$00,$66,$0a,$00,$33,$0a
    .byte $00
__Instrument_12
    .byte $0c,$0c,$2b,$2b,$00,$00,$00,$00,$00,$00,$00,$00,$00,$77,$18,$02
    .byte $dd,$18,$00,$99,$18,$01,$66,$10,$00,$55,$10,$00,$44,$10,$00,$33
    .byte $10,$00,$22,$10,$00,$22,$10,$00,$11,$10,$00,$00,$10,$00
__Instrument_13
    .byte $0c,$0c,$1c,$1c,$00,$00,$00,$00,$00,$00,$00,$00,$00,$ff,$0e,$00
    .byte $cc,$0e,$00,$99,$0e,$00,$66,$0e,$00,$33,$0e,$00,$00,$0e,$00
__Instrument_14
    .byte $0c,$0c,$40,$0d,$01,$00,$10,$00,$00,$02,$00,$00,$f4,$ee,$0c,$00
    .byte $aa,$0c,$00,$66,$0c,$00,$66,$0c,$00,$44,$0c,$00,$22,$0c,$00,$ee
    .byte $0a,$0c,$aa,$0a,$00,$66,$0a,$0c,$66,$0a,$00,$44,$0a,$0c,$22,$0a
    .byte $00,$ee,$0a,$18,$aa,$0a,$0c,$66,$0a,$18,$66,$0a,$0c,$44,$0a,$18
    .byte $22,$0a,$0c
__Instrument_15
    .byte $0c,$0c,$19,$19,$00,$00,$60,$00,$00,$00,$00,$00,$00,$ff,$10,$03
    .byte $dd,$1a,$f0,$99,$10,$44,$66,$1e,$86,$44,$1e,$ff
__Instrument_16
    .byte $0c,$0c,$2e,$0d,$00,$00,$80,$00,$00,$00,$00,$00,$00,$ff,$0a,$00
    .byte $ff,$8a,$00,$ff,$8a,$00,$ff,$0a,$03,$ff,$8a,$03,$ff,$8a,$03,$ff
    .byte $0a,$07,$ff,$8a,$07,$ff,$8a,$07,$ff,$0a,$03,$ff,$8a,$03,$ff,$8a
    .byte $03
__Instrument_17
    .byte $13,$0c,$14,$14,$80,$00,$20,$00,$00,$00,$00,$00,$f1,$e2,$d3,$c4
    .byte $b5,$a6,$97,$88,$ff,$0a,$00
__Instrument_18
    .byte $0c,$0c,$2e,$0d,$00,$00,$50,$00,$00,$00,$00,$00,$00,$ff,$0a,$0c
    .byte $ff,$0a,$00,$ff,$0a,$00,$ff,$0a,$10,$ff,$0a,$04,$ff,$0a,$04,$ff
    .byte $0a,$11,$ff,$0a,$05,$ff,$0a,$05,$ff,$0a,$15,$ff,$0a,$09,$ff,$0a
    .byte $09
__Instrument_19
    .byte $0e,$0c,$18,$0f,$4f,$00,$10,$00,$00,$00,$00,$00,$00,$00,$00,$ee
    .byte $3a,$01,$cc,$0a,$07,$aa,$0a,$0c,$66,$0a,$13
__Instrument_20
    .byte $0f,$0c,$55,$55,$00,$00,$80,$00,$05,$00,$00,$00,$00,$09,$05,$0c
    .byte $55,$0a,$00,$55,$0a,$00,$77,$0a,$00,$77,$0a,$00,$88,$0a,$00,$88
    .byte $0a,$00,$88,$0a,$00,$88,$0a,$00,$88,$0a,$00,$88,$0a,$00,$88,$0a
    .byte $00,$88,$0a,$00,$77,$0a,$00,$77,$0a,$00,$66,$0a,$00,$66,$0a,$00
    .byte $66,$0a,$00,$55,$0a,$00,$55,$0a,$00,$44,$0a,$00,$44,$0a,$00,$33
    .byte $0a,$00,$22,$0a,$00,$22,$0a,$00
__Instrument_21
    .byte $0c,$0c,$31,$31,$00,$00,$60,$00,$08,$02,$00,$00,$00,$77,$0a,$0c
    .byte $55,$0a,$00,$44,$0a,$00,$33,$0a,$00,$22,$0a,$00,$22,$0a,$00,$22
    .byte $0a,$00,$22,$0a,$00,$22,$0a,$00,$22,$0a,$00,$11,$0a,$00,$11,$0a
    .byte $00,$11,$0a,$00
__Instrument_22
    .byte $0c,$0c,$25,$25,$00,$00,$00,$00,$00,$00,$00,$00,$00,$ff,$10,$03
    .byte $ff,$1a,$d0,$ff,$1a,$e0,$bb,$1a,$f0,$77,$1a,$f8,$44,$10,$04,$33
    .byte $10,$04,$33,$10,$04,$00,$10,$00
__Instrument_23
    .byte $0c,$0c,$0d,$0d,$00,$00,$b0,$30,$00,$00,$00,$00,$00,$bb,$06,$00
__Instrument_24
    .byte $0c,$0c,$0d,$0d,$00,$00,$10,$60,$00,$00,$00,$00,$00,$ff,$0c,$00
__Instrument_25
    .byte $0c,$0c,$28,$28,$00,$00,$10,$00,$0a,$02,$00,$00,$00,$99,$8a,$00
    .byte $55,$8a,$00,$33,$8a,$00,$33,$8a,$00,$33,$8a,$00,$33,$8a,$00,$22
    .byte $8a,$00,$22,$8a,$00,$22,$8a,$00,$22,$8a,$00
__Instrument_26
    .byte $0c,$0c,$28,$28,$00,$00,$10,$00,$0a,$02,$00,$00,$00,$88,$8a,$00
    .byte $66,$8a,$00,$55,$8a,$00,$55,$8a,$00,$44,$8a,$00,$33,$8a,$00,$33
    .byte $8a,$00,$22,$8a,$00,$22,$8a,$00,$22,$8a,$00
__Instrument_27
    .byte $0e,$0c,$6c,$54,$00,$00,$20,$00,$00,$00,$00,$00,$00,$03,$07,$aa
    .byte $0a,$00,$aa,$0a,$00,$aa,$0a,$00,$aa,$0a,$00,$99,$0a,$00,$99,$0a
    .byte $00,$99,$0a,$00,$99,$0a,$00,$88,$0a,$00,$88,$0a,$00,$77,$0a,$00
    .byte $77,$0a,$00,$77,$0a,$00,$77,$0a,$00,$77,$0a,$00,$77,$0a,$00,$77
    .byte $0a,$00,$77,$0a,$00,$77,$0a,$00,$77,$0a,$00,$77,$0a,$00,$77,$0a
    .byte $00,$77,$0a,$00,$77,$0a,$00,$77,$0a,$00,$77,$0a,$00,$77,$0a,$00
    .byte $77,$0a,$00,$77,$0a,$00,$77,$0a,$00,$77,$0a,$00,$77,$0a,$00
__Instrument_28
    .byte $0c,$0c,$52,$52,$00,$00,$00,$00,$00,$00,$00,$00,$00,$33,$18,$20
    .byte $22,$18,$20,$11,$18,$20,$11,$18,$20,$66,$18,$20,$11,$18,$20,$aa
    .byte $18,$14,$44,$18,$14,$11,$18,$14,$11,$18,$18,$bb,$18,$18,$33,$18
    .byte $18,$22,$18,$18,$11,$18,$10,$11,$18,$10,$33,$18,$10,$11,$18,$10
    .byte $33,$10,$20,$aa,$18,$1c,$dd,$18,$10,$44,$18,$12,$22,$18,$14,$11
    .byte $18,$18,$00,$18,$1c
__Instrument_29
    .byte $0c,$0c,$28,$1f,$00,$00,$20,$00,$20,$03,$00,$00,$00,$ee,$da,$00
    .byte $dd,$8a,$00,$cc,$ea,$ff,$bb,$8a,$00,$bb,$8a,$00,$aa,$ea,$01,$99
    .byte $ea,$ff,$aa,$8a,$00,$bb,$8a,$00,$aa,$ea,$01
__Instrument_30
    .byte $0c,$0c,$0d,$0d,$00,$00,$50,$00,$02,$00,$fe,$00,$00,$ff,$4a,$81

; Track data
__Track_00
    .byte $3f,$06,$c0,$13,$7e,$cb,$03,$7e,$c0,$13,$7e,$cb,$03,$7e,$c0,$13
    .byte $7e,$cb,$03,$7e,$c0,$13,$7e,$cb,$03,$7e,$c0,$13,$7e,$cb,$07,$7e
    .byte $c0,$13,$7e,$cb,$07,$7e,$c0,$13,$7e,$cd,$07,$7e,$c0,$13,$7e,$cd
    .byte $07,$7e,$c0,$13,$7e,$cb,$03,$7e,$c0,$13,$7e,$cb,$03,$7e,$c0,$13
    .byte $7e,$cb,$03,$7e,$c0,$13,$7e,$cb,$03,$7e,$c0,$13,$7e,$cb,$07,$7e
    .byte $c0,$13,$7e,$cb,$07,$7e,$c0,$13,$7e,$ca,$0b,$7e,$c0,$13,$7e,$ca
    .byte $0b,$7e
__Track_01
    .byte $e3,$0f,$7e,$e3,$0f,$3e,$1a,$de,$0f,$e0,$0f,$de,$0f,$e3,$0f,$7e
    .byte $e3,$0f,$3e,$0d,$e8,$0f,$fe,$ea,$0f,$7e,$e5,$0f,$3e,$06,$de,$0f
    .byte $e0,$0f,$de,$0f
__Track_02
    .byte $e3,$0f,$7e,$e3,$0f,$3e,$0d,$dc,$0f,$3e,$05,$de,$0f,$3e,$06,$de
    .byte $0f,$e0,$0f,$de,$0f,$e3,$0f,$7e,$e3,$0f,$3e,$0d,$db,$0f,$7e,$d9
    .byte $0f,$7e,$d7,$0f,$7e,$dc,$0f,$7e,$de,$0f,$3e,$05,$e0,$0f,$7e
__Track_03
    .byte $d7,$0f,$e3,$0f,$d7,$0f,$fe,$d7,$0f,$7e,$db,$0f,$be,$e7,$0f,$be
    .byte $db,$0f,$db,$0f,$dc,$0f,$e8,$0f,$dc,$0f,$fe,$dc,$0f,$7e,$de,$0f
    .byte $be,$ea,$0f,$7e,$dc,$0f,$d9,$0f,$d7,$0f,$d7,$0f,$e3,$0f,$d7,$0f
    .byte $fe,$d7,$0f,$7e,$db,$0f,$be,$e7,$0f,$be,$db,$0f,$db,$0f,$dc,$0f
    .byte $e8,$0f,$dc,$0f,$fe,$dc,$0f,$7e,$d9,$0f,$e5,$0f,$7e,$d9,$0f,$7e
    .byte $d9,$0f,$db,$0f,$d9,$0f
__Track_04
    .byte $d7,$0f,$e3,$0f,$d7,$0f,$fe,$d7,$0f,$7e,$db,$0f,$be,$e7,$0f,$be
    .byte $db,$0f,$db,$0f,$dc,$0f,$e8,$0f,$dc,$0f,$fe,$dc,$0f,$7e,$dd,$0f
    .byte $e9,$0f,$7e,$dd,$0f,$7e,$dd,$0f,$e9,$0f,$dd,$0f,$de,$0f,$3e,$07
    .byte $e0,$0f,$3e,$07,$dc,$0f,$fe,$de,$0f,$7e,$d7,$0f,$7e,$bd,$00,$7d
    .byte $00,$bd,$00,$3d,$00,$3e,$04
__Track_05
    .byte $3f,$06,$c0,$13,$7e,$cb,$03,$7e,$c0,$13,$7e,$cb,$03,$7e,$c0,$13
    .byte $7e,$cb,$03,$7e,$c0,$13,$7e,$cb,$03,$7e,$c0,$13,$7e,$cb,$07,$7e
    .byte $c0,$13,$7e,$cb,$07,$7e,$c0,$13,$7e,$cd,$03,$7e,$c0,$13,$7e,$cd
    .byte $03,$7e,$c0,$13,$3e,$0f,$d0,$17,$fe,$d2,$17,$7e,$cb,$17,$7e,$bd
    .byte $00,$7d,$00,$bd,$00,$3d,$00,$3e,$04
__Track_06
    .byte $3e,$30,$c8,$0b,$fe,$ca,$0b,$7e,$c3,$0b,$7e,$bd,$00,$7d,$00,$bd
    .byte $00,$3d,$00,$3e,$04
__Track_07
    .byte $d7,$57,$7e,$d9,$57,$7e,$d7,$57,$7e,$d9,$57,$db,$57,$de,$57,$7e
    .byte $d9,$57,$db,$57,$de,$57,$7e,$db,$57,$7e,$d9,$57,$7e,$d7,$57,$7e
    .byte $d4,$57,$7e,$d1,$57,$7e,$d4,$57,$7e,$d7,$57,$7e,$d7,$57,$3e,$06
    .byte $ff
__Track_08
    .byte $d3,$63,$3e,$0f,$d0,$63,$3e,$0f,$d2,$63,$3e,$0f,$cf,$63,$3e,$0f
__Track_09
    .byte $3f,$06,$df,$5f,$7e,$e6,$5f,$7e,$eb,$5f,$7e,$e6,$5f,$7e,$df,$5f
    .byte $7e,$e6,$5f,$7e,$eb,$5f,$7e,$e6,$5f,$7e,$dc,$5f,$7e,$e3,$5f,$7e
    .byte $e8,$5f,$7e,$e3,$5f,$7e,$dc,$5f,$7e,$e3,$5f,$7e,$e8,$5f,$7e,$e3
    .byte $5f,$7e,$de,$5f,$7e,$e5,$5f,$7e,$ea,$5f,$7e,$e5,$5f,$7e,$de,$5f
    .byte $7e,$e5,$5f,$7e,$ea,$5f,$7e,$e5,$5f,$7e,$db,$5f,$7e,$e2,$5f,$7e
    .byte $e7,$5f,$7e,$e2,$5f,$bf,$49
__Track_10
    .byte $cc,$67,$cd,$67,$d0,$6b,$7e,$d0,$6b,$7e,$cd,$67,$d0,$67,$d2,$67
    .byte $d0,$67,$cc,$6b,$7e,$cb,$67,$c9,$6b,$be,$cc,$67,$cd,$67,$d0,$6b
    .byte $7e,$d2,$6b,$7e,$cd,$67,$d0,$6b,$3e,$05,$d0,$67,$d2,$67,$d3,$67
    .byte $d4,$67,$d4,$67,$d3,$67,$d2,$6b,$d0,$6b,$7e,$cd,$67,$d0,$6b,$7e
    .byte $d0,$67,$cf,$67,$ce,$67,$cc,$6b,$7e,$cb,$67,$c9,$6b,$3e,$10
__Track_11
    .byte $d5,$17,$7e,$d5,$17,$7e,$d8,$17,$d9,$17,$de,$17,$dc,$17,$d5,$17
    .byte $7e,$d5,$17,$7e,$d2,$17,$d3,$17,$d4,$17,$7e,$d5,$17,$7e,$d5,$17
    .byte $7e,$d8,$17,$d9,$17,$de,$17,$dc,$17,$d5,$17,$7e,$d5,$17,$7e,$de
    .byte $17,$dd,$17,$dc,$17,$7e,$d0,$17,$7e,$d0,$17,$7e,$d3,$17,$d4,$17
    .byte $d9,$17,$d7,$17,$d0,$17,$7e,$d0,$17,$7e,$d9,$17,$d8,$17,$d7,$17
    .byte $7e,$d5,$17,$7e,$d5,$17,$7e,$d8,$17,$d9,$17,$de,$17,$dc,$17,$d5
    .byte $17,$3e,$04,$3d,$00,$be

; Song data
__SongData
__Line_00  .byte $ff,$ff,$ff,$ff
__Line_01  .byte $fe,$00,<__line_00,>__line_00
__Line_02  .byte $ff,$00,$ff,$01
__Line_03  .byte $ff,$00,$ff,$02
__Line_04  .byte $ff,$00,$ff,$03
__Line_05  .byte $06,$05,$ff,$04
__Line_06  .byte $fe,$02,<__line_02,>__line_02
__Line_07  .byte $07,$ff,$ff,$ff
__Line_08  .byte $fe,$00,<__line_00,>__line_00
__Line_09  .byte $ff,$09,$ff,$08
__Line_0a  .byte $fe,$09,<__line_09,>__line_09
__Line_0b  .byte $10,$11,$ff,$ff
__Line_0c  .byte $fe,$0b,<__line_0b,>__line_0b
