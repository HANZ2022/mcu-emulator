#include"s_record.h"
#include"Memory.h"

int type_s0(char srecord[], int re_length)
{
	/*
	* Load S0. First two is type, first byte is length
	* second and third byte is 0x0000.
	* check the S-record by comparing with checksum and display the file's name
	*/
	int position = 8;
	int length = re_length * 2 + 4;
	int data;
	int sum = 0;
	int checksum;

	/*Add the record length*/
	sum = sum + re_length;
	printf("file name: ");

	/*Add the other bytes and make them into character for file's name*/
	while (position != (length - 2))
	{
		if (sscanf(&srecord[position], "%2x", &data))
		{
			sum = sum + data;
			printf("%c", data);
		};
		position = position + 2;
	}
	
	printf("\n");

	/*Compared with the checksum and return the results*/
	if (sscanf(&srecord[position], "%2x", &checksum))
	{
		return(checksum == (0XFF - (sum % 0X100)));
	}
	else
		return FALSE;

}

int type_s1(char srecord[], int re_length)
{
	/*
	* Load S1. First two is type, first byte is length
	* second and third byte are address.
	* check the S-record by comparing with checksum and load data into memory
	*/
	int position = 4;
	int length = re_length * 2 + 4;
	int checksum;
	int address_H;
	int address_L;
	int address;
	int data;
	int sum = 0;

	/*Get the address*/
	if (!sscanf(&srecord[position], "%2x%2x", &address_H, &address_L))
	{
		return -1;
	}

	/*Add record length, first and second byte of address*/
	sum = re_length + address_H + address_L;
	position = position + 4;
	address = (address_H << 8 | address_L);

	/*Add the other bytes to sum and store the data into memory*/
	while (position != (length - 2))
	{
		if (sscanf(&srecord[position], "%2x", &data))
		{
			memory.BM[address] = data;
			sum = sum + data;
		};
		address = address + 1;
		position = position + 2;
	}

	/*Compare the sum with the checksum and return the results*/
	if (sscanf(&srecord[position], "%2x", &checksum))
	{
		return(checksum == (0XFF - sum%0X100));
	}
	else
		return FALSE;
}

int type_s9(char srecord[], int re_length)
{
	/*
	* Load S9. First two is type, first byte is length
	* second and third byte are starting address.
	* check the S-record by comparing with checksum and display the starting address
	*/
	int address_H;
	int address_L;
	int address;
	int sum = 0;
	int checksum;

	/*Get the starting address and calculate the checksum. And display the starting address*/
	if (sscanf(&srecord[2], "%2x%2x%2x%2x", &re_length, &address_H, &address_L, &checksum))
	{
		sum = re_length + address_H + address_L;
		address = (address_H << 8 | address_L);

		//printf("Starting Address: %4x\n", address);
		PC = address;
		/*Compare the sum with the checksum and return the results*/
		return((checksum == (0XFF - sum%0X100)));
	}
	else
		return -1;

}


void typy_dist(char srecord[])
{
	/*
	* Distinguish the type of a S-record and do the loading.
	*/
	int type;
	char Sre;
	int re_length;
	int check_sum = TRUE;
	if ((sscanf(&srecord[0], "%c%1d%2x", &Sre, &type, &re_length)))
	{
		/*If this is not S-record, then print. If it is, do swich case*/
		if (Sre != 'S')
		{
			printf("This is not S-record: %s", srecord);
		}
		else
		{
			switch (type)
			{
			case 0:
				/*S0 record*/
				check_sum = type_s0(srecord, re_length);
				break;

			case 1:
				/*S1 record*/
				check_sum = type_s1(srecord, re_length);
				break;

			case 9:
				/*S9 record*/
				check_sum = type_s9(srecord, re_length);
				break;

			default:
				/*An invalid S-record*/
				printf("Invalid S-record, Not S0, S1, S9: %s\n", srecord);
				break;
			}
		}

		/*If the S-record doesn't match its checksum, print it*/
		if (!check_sum)
		{
			printf("The s-record: %s checksum doesn't match\n", srecord);
		}
	}
}