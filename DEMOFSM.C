#include <stdlib.h>
#include <stdio.h>
#include <sys.h>
#include "msxio.h"

/////////////////////////////////////
// dummy sprite
//
// *SCREEN 5 (256*212 Graphic mode, 16 colours):
// Matrix                          0000-69FF
// Sprite colours                  7400-75FF
// Sprite attribute table          7600-767F
// Palette                         7680-769F
// Sprite character patterns       7800-7FFF

void sprtinit(void) {
	uchar i;
	setvdpreg(14,1);
	setvdpwvram(0x3400);				// eqivalent to 0x7400
	for (i=16;i>0;i--) outp(0x98,15);	// sprite colors
	setvdpwvram(0x3800);				// eqivalent to 0x7800
	for (i=32;i>0;i--) outp(0x98,255);	// sprite definition
	setvdpwvram(0x3600);				// eqivalent to 0x7600
	for (i=32;i>0;i--) {
		outp(0x98,212);	// y
		outp(0x98,255);	// x
		outp(0x98,0);	// pattern
		outp(0x98,-1);	// dummy
	}
}
/////////////////////////////////////
// dummy put sprite
void putsprt(uchar n,uchar x,uchar y) {
	n *= 4;					// Trick: the sat is max 128 bytes
	setvdpwvram(0x3600+n); 	// eqivalent to 0x7600+4*n
	outp(0x98,y);			//	y
	outp(0x98,x);			//	x
}

void screenint(void) {
	uint i;
	scr(5);
	
	loadvrampalette("bg1.pl5");
	setpage(0);
	setvdpreg(14,0);
	setvdpwvram(0);
	for (i=212*128;i>0;i--) outp(0x98,i);
}
	
/////////////////////////////////////
//
enum state {right,left,uplf,uprg,downlf,downrg};

struct enemy {
	uchar x;
	uchar y;
	enum state st;
	uchar cnt;
};

void fsm(struct enemy *);

/////////////////////////////////////
// actual main
//

#define NumEn	32

struct enemy en[NumEn];

int main(void) {
	
	struct enemy *p_en = en;
	
	uchar n;
	
	screenint();
	
	sprtinit();
	
	srand8(JIFFY);

	p_en = en;
		
	for (n=0;n<NumEn;n++) {
		p_en->x = rand8() % 240;
		p_en->y = rand8() % 160;
		p_en->st = rand8() % 2;	// states are also integers (right and left are 0 and 1)
		p_en++;
	}
	
	while (checkkbd(7)&4) {		// Wait for ESC

		// do all the updates in RAM
		// while the raster is plotting the screen
		
		p_en = en;
		
		for (n=NumEn;n>0;n--) {
			fsm(p_en);
			p_en++;
		}
		
		// all the screen I/O has to fit in the VBLANK time
		// unless we do not use double buffering

		HLT;	// wait for VBLANK

		p_en = en;
		
		for (n=0;n<NumEn;n++) {
			putsprt(n,p_en->x,p_en->y);
			p_en++;
		}
	}
	
	TextMode(40);
	
	return 0;
}

////////////////////////////////
// simple AI coded as FSM

void fsm(struct enemy *en) {
	switch (en->st) {
		case right: {
			if (en->x<240)
				en->x++; 
			else {
				en->st = downlf;
				en->cnt = 16 + (rand8()&31);
			}
			break;
		}
		case downlf: {
			if ((en->y<196) && (en->cnt>0)) {
				en->y++; 
				en->cnt--;
			}
			else if ((en->y)>=196)
				en->st = uplf;
			else 
				en->st = left;
			break;
		}
		case downrg: {
			if (((en->y)<196) && (en->cnt>0)) {
				en->y++; 
				en->cnt--;
			}
			else if (en->y>=196)
				en->st = uprg;
			else 
				en->st = right;
			break;
		}
		case left: {
			if (en->x>0)
				en->x--; 
			else {
				en->st = downrg;
				en->cnt = 16 + (rand8()&31);
			}
			break;
		}
		case uplf: {
			if (en->y>0)
				en->y--; 
			else 
				en->st = left;
			break;
		}
		case uprg: {
			if (en->y>0)
				en->y--; 
			else 
				en->st = right;
			break;
		}
	}
}
