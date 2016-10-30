#include <stdlib.h>
#include <stdio.h>
#include <sys.h>
#include "msxio.h"

unsigned char _vdpReg[8] @ 0xF3DF;

/* set text mode t columns. */
void TextMode(char t)
{
	*((uchar*)LINL40) = t;
    di();
    #asm
        push ix
        push iy

		// ld	a,40
		// ld (LINL40),a	

        xor	a
        ld iy, ((0xfcc0 - 1))
        ld ix, 0x005f
        call 0x001c
		
        pop iy
        pop ix
    #endasm
    ei();
}

/* set screen mode. */
void scr(char mode)
{
	_vdpReg[1] |= 2;
    di();
    #asm
        push ix
        push iy

		ld	a,32				; useful when going to screen 0 and 1
		ld (LINL40),a	

        ld a,e
        ld iy, ((0xfcc0 - 1))
        ld ix, 0x005f
        call 0x001c
		
        pop iy
        pop ix
    #endasm
    ei();
}

/* set vdp command */
void vdp_cmd(int sx,int sy,int dx,int dy,int nx,int ny,uchar clr, uchar arg,uchar cmd) {
	struct cmd cmdlist;
	cmdlist.sx = sx;
	cmdlist.sy = sy;
	cmdlist.dx = dx;
	cmdlist.dy = dy;
	cmdlist.nx = nx;
	cmdlist.ny = ny;
	cmdlist.clr = clr;
	cmdlist.arg = arg;
	cmdlist.cmd = cmd;
	vdpcmd(&cmdlist);
}

void	line (int sx,int sy,int dx,int dy,uchar clr, uchar lop){
	
	int nx = dx-sx;
	int ny = dy-sy;
	uchar arg = 0;
	
	if (nx<0) {
		nx = -nx;
		arg |= 4; 		
	}
	if (ny<0) {
		ny = -ny;
		arg |= 8; 		
	}
	
	if (nx >= ny) {
		vdp_cmd(0,0,sx,sy,nx,ny,clr, arg,LINE+lop);
	} 
	else  {
		vdp_cmd(0,0,sx,sy,ny,nx,clr, arg | 1,LINE+lop);
	} 
}

void restorepalette() {
	uchar i,j;
	setvdpreg( 8,(* ((char*)0xFFE7)) | 32);
	setvdpreg( 7,0);
	setvdpreg(16,0);

	for	(i=0;i<16;i++) 	{
		j = inp (0x98);
		outp(0x9A,j);	//   R,B
		j = inp (0x98);
		outp(0x9A,j);	//	G
	} 
}	


// Line Bit_7 Bit_6 Bit_5 Bit_4 Bit_3 Bit_2 Bit_1 Bit_0
// 0 	   "7"  "6"   "5"   "4"   "3"   "2"   "1"   "0"
// 1 	   ";"  "]"   "["   "\"   "="   "-"   "9"   "8"
// 2 	   "B"  "A"   ???   "/"   "."   ","   "'"   "`"
// 3 	   "J"  "I"   "H"   "G"   "F"   "E"   "D"   "C"
// 4 	   "R"  "Q"   "P"   "O"   "N"   "M"   "L"   "K"
// 5 	   "Z"  "Y"   "X"   "W"   "V"   "U"   "T"   "S"
// 6 	   F3 	 F2    F1  CODE   CAP  GRAPH  CTRL SHIFT
// 7 	   RET  SEL    BS  STOP   TAB   ESC    F5    F4
// 8 	   RIGHT DOWN  UP  LEFT   DEL   INS   HOME SPACE

#asm
	global _checkkbd
_checkkbd:
	in	a,(0aah)
	and 011110000B			; upper 4 bits contain info to preserve
	or	e
	out (0aah),a
	in	a,(0a9h)
	ld	l,a
	ret
	
	
; PSG I/O port A (r#14) – read-only
; Bit	Description	Comment
; 0	Input joystick pin 1	(up)
; 1	Input joystick pin 2	(down)
; 2	Input joystick pin 3	(left)
; 3	Input joystick pin 4	(right)
; 4	Input joystick pin 6	(trigger A)
; 5	Input joystick pin 7	(trigger B)
; 6	Japanese keyboard layout bit	(1=JIS, 0=ANSI)
; 7	Cassette input signal	

	global _checkjoy
_checkjoy:
	ld	a,0x0f
	out	(0xa0),a
	ld	a,0x8F
	out	(0xa1),a		; select port A
	ld	a,0x0e
	out	(0xa0),a
	in	a,(0xa2)

	ld	c,a
	
	ld  e,8
    call    _checkkbd
	bit	0,a				; space
	jr	nz,1f
	res	4,c			; (trigger A)
1:
	bit	7,a				; RIGHT
	jr	nz,1f
	res	3,c			; (right joy)
1:
	bit	6,a				; DOWN
	jr	nz,1f
	res	1,c			; (down joy)
1:
	bit	5,a				; UP
	jr	nz,1f
	res	0,c			; (up joy)
1:
	bit	4,a				; LEFT
	jr	nz,1f
	res	2,c			; (left joy)
1:
	ld  e,5
    call    _checkkbd
	bit	5,a				; X
	jr	nz,1f
	res	5,c			; (trigger B)
1:
	bit	7,a				; Z
	jr	nz,1f
	res	4,c			; (trigger A)
1:
	ld a,c
	cpl
	ld l,a
	ret
#endasm


// ; -------------------------------------------------------------------
// ; rand8
// ; -------------------------------------------------------------------
// ; out:	a = 8 bits random number
// ; -------------------------------------------------------------------
#asm
	global _rand8,_srand8
_srand8:
	ld	(randSeed),de
	ret
_rand8:
    ld      hl,(randSeed)
    add     hl,hl
    sbc     a,a
    and     0x83
    xor     l
    ld      l,a
    rlca
    ld      (randSeed),hl
    ret
	psect	bss,class=DATA
randSeed:	
	ds	2
	psect		text
#endasm

/////////////////////////////////////
// to be moved in vdpio.c
//
// void otir98h((void*) p, int nbyte);

#asm
	global _otir98h
_otir98h:
	; de => void* p
	; bc => int nbyte
	ex de,hl
	ld a,b
	or c
	ret z
	ld d,b
	ld b,c
	inc d
	ld c,098h
_l17:
	outi
	jp	nz,_l17
	dec	d
	jp	nz,_l17
	ret
	
	global _otir9Bh
_otir9Bh:
	; de => void* p
	; bc => int nbyte
	ex de,hl
	ld a,b
	or c
	ret z
	ld d,b
	ld b,c
	inc d
	ld c,09Bh
1:
	outi
	jp	nz,1B
	dec	d
	jp	nz,1B
	ret
	
#endasm

