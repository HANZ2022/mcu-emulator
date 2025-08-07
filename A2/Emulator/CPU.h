#pragma once
#include <stdio.h>

#define CACHE_SIZE 8
#define RCsize 8
#define register 0
#define costant 1
#define Low 0
#define High 1
//#define DIR //DIR or Ass
#define ASS // WBack or WT
//#define TR // WBack or WT
#define WBack // WBack or WT

typedef union word_byte
{
	unsigned short W;
	unsigned char B[2];
}word_byte;


typedef union word_byte_signed
{
	short W;
	char B[2];
}word_byte_signed;


#define B15(x) (((x)>>15) & 0x01)
#define B7(x) (((x)>>7) & 0x01)

typedef struct psw_bits
{
	unsigned short c : 1;
	unsigned short z : 1;
	unsigned short n : 1;
	unsigned short slp : 1;
	unsigned short v : 1;
	unsigned short current : 3; /* Current priority */
	unsigned short faulting : 1; /* 0 - No fault; 1 - Active fault */
	unsigned short reserved : 4;
	unsigned short previous : 3; /* Previous priority */
}PSW;




struct cache_line
{
	unsigned char age;
	unsigned short address; /* 0x0000…0xFFFF */
	word_byte content;
	unsigned char dirty;
};

struct cache_line cache_mem[CACHE_SIZE];

int cache(unsigned short, word_byte*, unsigned char, unsigned char);
void update_cache(int, unsigned short);
void display_cache_lines();
int LRU;
int CPU_clock;

PSW psw; /* PSW */
extern word_byte regfile[2][RCsize];
extern void bus(unsigned short, word_byte*, unsigned char, unsigned char);
extern void update_psw(unsigned short, unsigned short, unsigned short,unsigned short);