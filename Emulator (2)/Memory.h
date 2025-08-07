#pragma once


#define read 0
#define write 1
#define byte 1
#define word 0

#define DST(x) (x&0x07)
#define	SRC(x) ((x>>3)&0x07)
#define	W_B(x) ((x>>6)&0x01)
#define	R_C(x) ((x>>7)&0x01)
#define	BIT(x,b) ((x>>b)&0x01)


unsigned short PC;
unsigned short data;

union mem
{
	unsigned short WM[0x10000 >> 1];
	unsigned char BM[0X10000];
};
union mem memory;
