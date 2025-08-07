/*
 - Lab 4
 - All the cache lines' operation 
 - Including searching the address in cache lines
  and updating and display.
 - ECED 3403
 - Han Zhang, B00852175
 */

#include "CPU.h"
#include <stdio.h>
#include "Memory.h"
#define TRUE 1
#define FALSE 0

void display_cache_lines()
{
	/*Display the cache lines*/
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		printf("Cache line:%d  ", i);
		printf("Address:0X%04X  ", cache_mem[i].address);
		printf("Content:0X%04X ", cache_mem[i].content.W);
#ifdef ASS
		printf("Age:%d\n", cache_mem[i].age);
#endif
#ifdef WBack
		printf("DityBit:%d\n", cache_mem[i].dirty);
#endif
#if ((!defined ASS)&&(!defined WBack))
		printf("\n");
#endif
	}
}


void update_cache(int update_id, unsigned short address)
{
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		/*Go Throuhg the cache lines*/
		if (cache_mem[i].age > cache_mem[update_id].age)
		{
			/*Decrease the usage of the cache lines, 
			whose usage is greater than choosen cache line*/
			cache_mem[i].age--;
		}
	}
	/*Update the cache line content*/
	cache_mem[update_id].age = CACHE_SIZE - 1;
	cache_mem[update_id].address = address;
}



void Hit(int i, word_byte* mdr, unsigned char R_W, unsigned char W_B)
{
	if (R_W)
	{
		/*Write to cache*/
		if (W_B)
		{
			cache_mem[i].content.B[0] = mdr->B[0];
		}
		else
		{
			cache_mem[i].content.W = mdr->W;
		}
		cache_mem->dirty = TRUE;
	}
	else
	{
		/*Read from cache*/
		if (W_B)
		{
			mdr->B[0] = cache_mem[i].content.B[0];
		}
		else
		{
			mdr->W = cache_mem[i].content.W;
		}

	}
}





int cache(unsigned short mar, word_byte* mdr, unsigned char R_W, unsigned char W_B)
{
	int miss = TRUE;
	int cache_No = 0;

#ifdef ASS
	/*Go Throuhg the cache lines*/
	for (int i = 0; i < CACHE_SIZE; i++)
#else 
	/*Go to the cache line*/
	int i = 0;
	i = mar/2 % CACHE_SIZE;
#endif
	{
		/*If the current cache line hitted*/
		if (cache_mem[i].address == mar)
		{
			/*Checking whether the address we want is in cache, 
			 If YES, return the cache*/
			printf("Hit\n");
			cache_No = i;
			miss = FALSE;
#ifndef DIR
			break;
#endif
		}
	}


	if (miss)
	{
#ifdef ASS
		/*Find the RLU*/
		for (int i = 0; i < CACHE_SIZE; i++)
			if (cache_mem[i].age < cache_mem[cache_No].age)
			{
				/*Record the cache with smallest usage*/
				cache_No = i;
			}
#else
		/*Current cache line*/
		cache_No = i;
#endif
			printf("Miss\n");
	}






	if (R_W == read)
	{
		if (miss)
		{
			/*Miss in cache*/
#ifdef WBack
			/*If it is Write back, chech whether the cache was written to*/
			if (cache_mem[cache_No].dirty == TRUE)
			{
				/*If written to, upload it to memory*/
				bus(cache_mem[cache_No].address, &cache_mem[cache_No].content, write, word);
				cache_mem[cache_No].dirty = FALSE;
			}
#endif
			/*Read from memory to cache*/
			bus(mar, &cache_mem[cache_No].content, read, W_B);
		}

		/*Read from cache*/
		Hit(cache_No, mdr, read, W_B);
	}

	else
	{
#ifdef WBack
		if (miss)
		{
			/*If write to cache*/

			if (cache_mem[cache_No].dirty == TRUE)
			{
				/*If written to, upload it to memory*/
				bus(cache_mem[cache_No].address, &cache_mem[cache_No].content, write, W_B);
				cache_mem[cache_No].dirty = FALSE;
			}
		}

			
#endif
		/*Write to cache*/
		Hit(cache_No, mdr, R_W, W_B);
#ifdef WT
		/*Write to memory*/
		bus(mar, &cache_mem[cache_No].content, R_W, W_B);
#else
		cache_mem[cache_No].dirty = TRUE;
#endif
	}
	update_cache(cache_No, mar);
}