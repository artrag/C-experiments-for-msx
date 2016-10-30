#include <stdlib.h>
#include <stdio.h>
#include <sys.h>
#include "msxio.h"

/////////////////////////////////////
// to be moved in dskio.c
//
void loadvramdata(char namefile[]) {
    FILE * pFile;
	static uchar buffer[BUFFLEN];	

	
	pFile = fopen (namefile,"rb");
	if (pFile==NULL) 	{
		scr(0);
		printf ("Error opening file %s",namefile);
		exit(1);
	}
	else 	{
		long 	m;		// needed to load up to 64Kbyte
	
		// read msx header
		fread(buffer,1,7,pFile);
		
		// compute the number of bytes to be loaded
		m = (buffer[3]+buffer[4]*256)-(buffer[1]+buffer[2]*256)+1;
		
// Byte: #FE (Type of file)
// Word: Begin address of file
// Word: End address of file
// Word: Start address of file (Only important when ",R" parameter is defined with BLOAD-command)

		// load and move to vram
		do {  
			uchar n,lenread = min(BUFFLEN,m);
			n = fread(buffer,1,lenread,pFile);
			otir98h(buffer,n);
			if (n<lenread)	
				break;		// end if I read less than I expected
			m -= n; 
		} 
		while (m > 0);		// end if I read the last bulk of data 
		
		fclose (pFile);
	}
	return;
}

/////////////////////////////////////
// to be moved in dskio.c
//
// *SCREEN 5 (256*212 Graphic mode, 16 colours):
// Matrix                          0000-69FF
// Sprite colours                  7400-75FF
// Sprite attribute table          7600-767F
// Palette                         7680-769F
// Sprite character patterns       7800-7FFF

void loadvrampalette(char namefile[]) {
	
    FILE * pFile;

	pFile = fopen (namefile,"rb");
	if (pFile==NULL) 	{
		scr(0);
		printf ("Error opening file %s",namefile);
		exit(1);
	}
	else 	{
		uchar i,j;

		setvdpreg( 8,(* ((char*)0xFFE7)) | 32);
		setvdpreg( 7,0);
		setvdpreg(16,0);
		
		// skip msx header
		fseek (pFile , 7 , SEEK_SET);
		
		for	(i=0;i<16;i++) 	{
			j = fgetc (pFile);
			outp(0x9A,j);	//   R,B
			j = fgetc (pFile);
			outp(0x9A,j);	//	G
		} 
	
		fclose (pFile);
	}
}
/////////////////////////////////////
// do not use for odd nx
// buggy: WIP
//


void loadvrambox(char namefile[],int dx,int dy) {
    FILE * pFile;
	
	pFile = fopen (namefile,"rb");
	if (pFile==NULL) 	{
		scr(0);
		printf ("Error opening file %s",namefile);
		exit(1);
	}
	else 	{
		int 	nx,ny;
		uint 	m;		// needed to load up to 64Kbyte
		uchar	buffer[5];	
	
		// read  header
		fread(buffer,1,5,pFile);

		// Word: nx,ny

		// compute the number of bytes to be loaded
		nx = (buffer[0]+buffer[1]*256);
		ny = (buffer[2]+buffer[3]*256); 

		m  = nx / 2 * ny ;

		vdp_cmd(0,0,dx,dy,nx,ny,buffer[4], 0,HMMC);

		m--;

		setvdpreg(17,44+128);
		
		// load and move to vram
		while (m > 0) {				// end if I read the last byte
			outp(0x9B,fgetc(pFile));
			m--;
		} 
		
		fclose (pFile);
	}
	return;
}

