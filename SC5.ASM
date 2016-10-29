
	psect		text
	global	_setpage		;Display page E in screen 5
_setpage:
	ld a,e
	add a,a ;x32
	add a,a
	add a,a
	add a,a
	add a,a
	add a,31
	di
	out (0x99),a
	ld a,2+128
	ei
	out (0x99),a
	ret

	global _vdpcmd
_vdpcmd:
	ex      de,hl
	ld      a,32
	di
	out     (0x99),a
	ld      a,17+128
	out     (0x99),a
	ei
	ld      c,0x9B
1:	
	ld      a,2              ; wait VDPready
	di
	out     (0x99),a         ; select s#2
	ld      a,15+128
	out     (0x99),a
	in      a,(0x99)
	rra
	ld      a,0
	out     (0x99),a
	ld      a,15+128
	ei
	out     (0x99),a
	jp      c,1b

	rept 15						; 15x OUTI (faster than OTIR)
		outi
	endm
	ret
