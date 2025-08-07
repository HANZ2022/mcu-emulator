#include "CPU.h"

unsigned carry[2][2][2] = { 0, 0, 1, 0, 1, 0, 1, 1 };
unsigned overflow[2][2][2] = {0, 1, 0, 0, 0, 0, 1, 0};

void update_psw(unsigned short src, unsigned short dst, unsigned short res, 
			unsigned short wb)
{
/* 
 - Update the PSW bits (V, N, Z, C) 
 - Using src, dst, and res values and whether word or byte 
 - ADD, ADDC, SUB, and SUBC
*/
unsigned short mss, msd, msr; /* Most significant src, dst, and res bits */

if (wb == 0)
{
	mss = B15(src);
	msd = B15(dst);
	msr = B15(res);
}
else /* Byte */
{
	mss = B7(src);
	msd = B7(dst);
	msr = B7(res);
	res &= 0x00FF;	/* Mask high byte for 'z' check */
}

/* Carry */
psw.c = carry[mss][msd][msr];
/* Zero */
psw.z = (res == 0);
/* Negative */
psw.n = (msr == 1);
/* oVerflow */
psw.v = overflow[mss][msd][msr];
}
