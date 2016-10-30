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

uchar ball[] = {	
				0x07,0x1F,0x3F,0x7F,0x7F,0xFF,0xFF,0xFF,
				0xFF,0xFF,0xFF,0x7F,0x7F,0x3F,0x1F,0x07,
				0xE0,0xF8,0xFC,0xFE,0xFE,0xFF,0xFF,0xFF,
				0xFF,0xFF,0xFF,0xFE,0xFE,0xFC,0xF8,0xE0
			   };

void sprtinit(void) {
	int i,j;
	setvdpreg(14,1);
	setvdpwvram(0x3400);					// eqivalent to 0x7400
	for (i=0;i<31;i++) {
		for (j=1;j<16;j+=2) 
			outp(0x98,j);					// enemy sprite colors
		for (j=15;j>=0;j-=2) 
			outp(0x98,j);					// enemy sprite colors
	}
	
	for (i=0;i<16;i+=2) 
		outp(0x98,i);						// my sprite colors
	for (i=14;i>=0;i-=2) 
		outp(0x98,i);	
	
	setvdpwvram(0x3800);					// eqivalent to 0x7800
	for (i=0;i<32;i++) outp(0x98,ball[i]);	// sprite definition
		
	setvdpreg(14,1);
	for (i=0;i<32;i++) {
		setvdpwvram(0x3600+i*4+2);			// eqivalent to 0x7600+i*4+2
		outp(0x98,0);	// pattern
	}
}
/////////////////////////////////////
// dummy put sprite
void putsprt(uchar n,uchar x,uchar y) {
	setvdpwvram(0x3600+4*n); 	// eqivalent to 0x7600+4*n
	outp(0x98,y);				//	y
	outp(0x98,x);				//	x
}

void screenint(void) {
	uint n;
	scr(5);
	
	loadvrampalette("bg1.pl5");
	setpage(0);

	setvdpreg(14,0);
	setvdpwvram(0);
	for (n=212*128;n>0;n--) outp(0x98,n);
	
	vdp_cmd(0,0,0,0,256,212,0x77, 0,HMMV);

	loadvrambox("test.cpy",64,64);	
	
	for (n=0;n<256;n++) {
		line (127,106,n,  0,n,IMP);
		line (127,106,n,211,n,IMP);
	}
	for (n=0;n<212;n++) {
		line (127,106,  0,n,n,IMP);
		line (127,106,255,n,n,IMP);
	}
	
}
	
/////////////////////////////////////
//
enum state {right,left,up,down,idle};

struct enemy {
	int x;
	int y;
	enum state st;
	int cnt;
};

void fsm(struct enemy *);

/////////////////////////////////////
// actual main
//

#define NumEn	31

struct enemy en[NumEn];

struct enemy myball = {120,98,0,0};	// you can initialize struct at define time

interact(struct enemy * p) {
	uchar io = checkjoy (); 
	if (io & 1) {p->y--;}		// up 
	if (io & 2) {p->y++;}		// down
	if (io & 4) {p->x--;}		// left
	if (io & 8) {p->x++;}		// right
	// if (io & 16) {Trigger A or space or Z}		
	// if (io & 32) {Trigger B or X}		
}

int main(void) {
	
	int n;
	
	screenint();
	
	srand8(JIFFY);

	sprtinit();

	for (n=0;n<NumEn;n++) {
		en[n].x = 120;
		en[n].y = -17;
		en[n].st = idle;
		en[n].cnt = (n+1) * 42;
	}

	while (checkkbd(7)&4) {		// Wait for ESC

		struct enemy *p_en;
	
		// do all the updates in RAM
		// while the raster is plotting the screen
		
		p_en = en;	
		for (n=0;n<NumEn;n++) {
			fsm(p_en);			// same as 	fsm(&en[n]);  but faster
			p_en++;
		}
		
		//	move my ball
		interact(&myball);		
		
		// all the screen I/O has to fit in the VBLANK time
		// unless we do not use double buffering

		HLT;	// wait for VBLANK
		
		setvdpreg(14,1);	// make sure putsprite() aims to the right VRAM address

		// plot my ball
		putsprt(31,myball.x,myball.y);		

		// plot other balls
		p_en = en;
		for (n=0;n<NumEn;n++) {
			putsprt(n,p_en->x,p_en->y);		
			p_en++;				// same as 	putsprt(n,en[n].x,en[n].y); but faster
		}
	}
	
	TextMode(40);
	
	return 0;
}

////////////////////////////////
// simple AI coded as FSM

void fsm(struct enemy *en) {
	switch (en->st) {
		case idle: {
			
			en->cnt --;
			
			if (en->cnt == 0) {
				en->st = down;
				en->cnt = 32;
			}
			break;
		}
		case down: {
			
			en->y++; 
			en->cnt--;
			
			if (en->y>=196) {
				en->st = up;
			}
			else if (en->cnt == 0) {
				if (en->x < 128) {
					en->st = right;
				}
				else {
					en->st = left;
				}
			}
			break;
		}
		case left: {
			
			if (en->x>0) {
				en->x--; 
			}
			else {
				en->st = down;
				en->cnt = 32;
			}
			break;
		}
		case right: {
			
			if (en->x<240) {
				en->x++; 
			}
			else {
				en->st = down;
				en->cnt = 32;
			}
			break;
		}
		case up: {
			
			if (en->y>-17) {
				en->y--; 
			}
			else {
				en->st = idle;
				en->cnt = 42;			
				en->x = 120;			
				en->y = -17;			
			}
			break;
		}
	}
}
