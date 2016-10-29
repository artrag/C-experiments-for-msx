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
	cmdlist.clr = arg;
	cmdlist.cmd = cmd;
	vdpcmd(&cmdlist);
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

// ;    5   |    Z     Y     X     W     V     U     T     S
// ;    6   |   F3    F2    F1   CODE   CAP  GRAPH CTRL  SHIFT
// ;    7   |   RET   SEL   BS   STOP   TAB   ESC   F5    F4
// ;    8   |  RIGHT DOWN   UP   LEFT   DEL   INS  HOME  SPACE
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

