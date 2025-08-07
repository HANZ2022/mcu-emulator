/*
 - A1

 - ECED 3403
 - Han Zhang, B00852175
 */

#include"s_record.h"
#include"Memory.h"
#include "CPU.h"

extern void Instruction_Menu();
extern void decoder(unsigned short);
extern void  bus(unsigned short, word_byte*, unsigned char, unsigned char);

int main(int argc, char* argv[])
{
	/*Try to open the file*/
	if (fopen_s(&fptr, argv[1], "r") != 0)
	{
		/*Fail to open the file*/
		printf("Error opening file >%s< - possibly missing\n", argv[1]);
		getchar();
		return -1;
	}
	

	char buffer[Max_L];
	int END = FALSE;

	/*Read the first line*/
	fgets(buffer, Max_L, fptr);

	/*Read a line and load it until all the line are read in*/
	while ( !END )
	{
		/*Load the S-record*/
		typy_dist(buffer);

		if (!fgets(buffer, Max_L, fptr))
		{
			END = TRUE;
		};
	}

	END = FALSE;
	int address;
	int order;
	unsigned int breakpoint;
	unsigned int head;
	unsigned int end;


	Instruction_Menu();
	while (!END)
	{

		if (!scanf("%d", &order))
		{
			printf("Failed to read order\n");
		}

		switch (order)
		{
		case 0:
			Instruction_Menu();
			break;
		case 1:
			/*Continue*/
			bus(PC, &data, read, word);
			decoder(data);
			printf("New PC = %04X\n\n", PC);
			break;
		case 2:
			/*Display PC*/
			printf("PC = %04X\n\n", PC);
			break;
		case 3:
			/*Change PC*/
			printf("Print new PC:\n\n0x:");
			if (scanf("%5x", &address))
			{
				/*Get new PC*/
				PC = address;
			}
			break;
		case 4:

			/*Memory display*/
			printf("Enter the memory range\n");

			/*Read lower limit bound*/
			printf("From 0X:");
			if (!scanf("%X", &head))
			{
				printf("Failed to read order\n");
			}

			/*Read upper limit bound*/
			printf("To 0X:");
			if (!scanf("%X", &end))
			{
				printf("Failed to read order\n");
			}
			for (unsigned int i = head; i <= end; i=i+16)
			{
				printf("Mem[0X%04x]...:  ", i);
				for (unsigned int j = 0; j < 16; j++)
				{
					printf("%02X ", memory.BM[i+j]);
				}
				printf("...\n");
			}
			printf("\n");
			break;
		case 5:
			printf("PSW:\n");
			printf("C:%d  Z:%d  N:%d  slp:%d  V:%d  Current:%d  Faulting:%d  Reserved:%d  Previous:%d \n\n",
				psw.c , psw.z, psw.n, psw.slp, psw.v, psw.current, psw.faulting, psw.reserved, psw.previous);
			break;
		case 6:
			/*Display register*/

			printf("Register:\n");
			for (int j = 0; j < RCsize; j++)
			{
				printf("R%d: 0X%04X   ", j, regfile[register][j].W);
			}
			printf("\n");
			/*Display constant register*/
			printf("Constant Register:\n");

			for (int j = 0; j < RCsize; j++)
			{
				printf("R%d: 0X%04X   ", j, regfile[costant][j].W);
			}
			printf("\n\n");
			break;

		case 7:
			/*Set breakpoint*/
			printf("To PC=0X");
			if (!scanf("%X", &breakpoint))
			{
				printf("Failed to read order\n");
			}
			
			while (PC != breakpoint)
			{
				bus(PC, &data, read, word);
				decoder(data);
			}
			break;
		case 8:
			/*Exit*/
			END = TRUE;
			break;
		}

	}
}