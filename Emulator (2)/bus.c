#include <stdio.h>
#include "Memory.h"
#include "CPU.h"

extern void bus(unsigned short mar, word_byte *mdr, unsigned char R_W, unsigned char W_B)
{
	/*
	 * mar: address
	 * mdr: data
	 * R_W: Read or Write
	 * W_B: word or byte
	 */
	if (R_W)
	{
		/*Write to memory*/
		if (W_B)
		{
			memory.BM[mar] = mdr->B[0];
		}
		else
		{
			memory.WM[mar >> 1] = mdr->W;
		}
		
	}
	else
	{
		/*Read from memory*/
		if (W_B)
		{
			//*mdr = memory.BM[mar];
			mdr->B[0] = memory.BM[mar];
		}
		else
		{
			//*mdr = memory.WM[mar >> 1];
			mdr->W = memory.WM[mar >> 1];
		}
		
	}
}