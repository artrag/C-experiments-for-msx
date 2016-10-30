typedef unsigned int  	uint;
typedef unsigned long 	ulong;
typedef unsigned short 	ushort;

#define	LINL40	0xF3AE

#define di() asm("di")
#define ei() asm("ei")

#define setvdpreg(r,v) 		{di();outp(0x99,(v));outp(0x99, 0x80|(r));ei();}
#define setvdpwvram(addr) 	{di();outp(0x99,(addr)&255);outp(0x99, 0x40|((addr)>>8));ei();}
#define setvdprvram(addr) 	{di();outp(0x99,(addr)&255);outp(0x99, 0x3F&((addr)>>8));ei();}

extern uchar _vdpReg[];

#define displayOn() 		setvdpreg(1,_vdpReg[1]|64)
#define displayOff() 		setvdpreg(1,_vdpReg[1]&191)

#define JIFFY   (*((uint *) 0xFC9E))

#define HLT					{uint t=JIFFY; while(t==JIFFY);}

void scr(char t);
void TextMode(char t);

struct cmd {
            int    sx,sy;
            int    dx,dy; 
            int    nx,ny;
            uchar   clr;
            uchar   arg;
            uchar   cmd;
        };
		
void 	setpage(char page);
void	vdpcmd(struct cmd  *t);
void 	vdp_cmd(int sx,int sy,int dx,int dy,int nx,int ny,uchar clr, uchar arg,uchar cmd);
void 	restorepalette();

void	line (int sx,int sy,int dx,int dy,uchar clr, uchar lop);

uchar 	checkkbd (char );
uchar 	checkjoy (void);
void 	srand8 (uint);
uchar 	rand8 (void);
#define srand srand8
#define rand  rand8
void 	otir98h(void * p, int n);
void 	otir9Bh(void * p, int n);

#define HMMC 0xF0
#define YMMM 0xE0
#define HMMM 0xD0
#define HMMV 0xC0

#define LMMC 0xB0
#define LMCM 0xA0
#define LMMM 0x90
#define LMMV 0x80

#define LINE 0x70
#define SRCH 0x60
#define PSET 0x50
#define POINT 0x40
#define STOP 0x00

#define IMP 0x00
#define AND 0x01
#define  OR 0x02
#define XOR 0x03
#define NOT 0x04

#define TIMP 0x08
#define TAND 0x09
#define TOR  0x0A
#define TXOR 0x0B
#define TNOT 0x0C

void loadvrampalette(char namefile[]);
void loadvramdata(char namefile[]);
void loadvrambox(char namefile[],int dx,int dy);	// WIP do not use

#define BUFFLEN 128
