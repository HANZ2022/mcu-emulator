#include <stdio.h>
#include "Memory.h"
#include "CPU.h"

word_byte regfile[2][RCsize] = { {0xF0A0, 0xF1A1, 0xF2A2, 0xF3A3, 0xF4A4, 0xF5A5, 0xF6A6, 0xF7A7},
    {0x0000, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0xFFFF} };

void bl(unsigned short mst)
{
    short offset = mst & 0x1FFF; //mask to get 0-12bit
    if (BIT(offset,12) == 1)
    {
        /*Bits 13 to 15 of Offset = 1*/
        offset = offset | 0xE000;
    }
    else
    {
        /*Bits 13 to 15 of Offset = 0*/
        offset = offset & 0x1FFF;
    }
    PC = PC + (offset << 1);
}

void beq_bra(unsigned short mst)
{
    short offset = mst & 0x03FF; //mask to get 0-12bit
    if (BIT(offset,9))
    {
        /*Bits 10 to 15 of Offset = 1*/
        offset = offset | 0xFC00;
    }
    else
    {
        /*Bits 10 to 15 of Offset = 0*/
        offset = offset & 0x03FF;
    }
    offset = offset << 1;

    switch ((mst >> 10) & 0x07)
    {
        /*Check the branch type*/
    case 0:
        /*beq_bz*/
        PC = psw.z == 1 ? PC + offset : PC;
        break;
    case 1:
        /*bne_bnz*/
        PC = psw.z == 0 ? PC + offset : PC;
        break;
    case 2:
        /*bc_bhs*/
        PC = psw.c == 1 ? PC + offset : PC;
        break;
    case 3:
        /*bnc_blo*/
        PC = psw.c == 0 ? PC + offset : PC;
        break;
    case 4:
        /*bn*/
        PC = psw.n == 1 ? PC + offset : PC;
        break;
    case 5:
        /*bge*/
        PC = (psw.n ^ psw.v) == 0 ? PC + offset : PC;
        break;
    case 6:
        /*blt*/
        PC = (psw.n ^ psw.v) == 1 ? PC + offset : PC;
        break;
    case 7:
        /*bra*/
        PC = PC + offset;
        break;
    }
}

int add_bis(unsigned short mst)
{
    unsigned char DSTReg = DST(mst);
    unsigned char SRCReg = SRC(mst);
    unsigned char RC = BIT(mst,7);
    unsigned char WB = BIT(mst,6);
    unsigned short SRCVal; 
    unsigned short DSTVal;
    word_byte Res;

    if (WB == word)
    {
        SRCVal = regfile[RC][SRCReg].W;
        DSTVal = regfile[register][DSTReg].W;
    }
    else
    {
        SRCVal = regfile[RC][SRCReg].B[0];
        DSTVal = regfile[register][DSTReg].B[0];
    }

    switch ((mst >> 8) & 0b1111)
    {
    case 0:
        /*ADD*/
        Res.W = SRCVal + DSTVal;
        break;
    case 1:
        /*ADDC*/
        Res.W = SRCVal + DSTVal + psw.c;
        break;
    case 2:
        /*SUB*/
        Res.W = ~SRCVal + DSTVal + 1;
        break;
    case 3:
        /*SUBC*/
        Res.W = ~SRCVal + DSTVal + psw.c;
        break;
    case 4:
        /*DADD*/
        Res.W = SRCVal + DSTVal;
        break;
    case 5:
        /*CMP*/
        Res.W = ~SRCVal + DSTVal + 1;
        break;
    case 6:
        /*XOR*/
        Res.W = SRCVal ^ DSTVal + psw.c;
        break;
    case 7:
        /*AND*/
        Res.W = SRCVal & DSTVal + psw.c;
        break;
    case 8:
        /*OR*/
        Res.W = SRCVal | DSTVal;
        break;
    case 9:
        /*BIT*/
        Res.W = DSTVal & (1 << SRCVal);
        break;
    case 10:
        /*BIC*/
        Res.W = DSTVal & ~(1 << SRCVal);
        break;
    case 11:
        /*BIS*/
        Res.W = DSTVal | (1 << SRCVal);
        break;
    default:
        return 1;
    }



    switch ((mst >> 8) & 0b1111)
    {
    case 5:
    case 9:
        break;
    default:
        if (WB == word)
        {
            regfile[register][DSTReg].W = Res.W;
        }
        else
        {
            regfile[register][DSTReg].B[0] = Res.B[0];
        }
    }
    update_psw(SRCVal, DSTVal, Res.W, WB);
    return 0;
}

void sra_sxt(unsigned short mst)
{
    unsigned char DSTReg = DST(mst);
    word_byte DSTVal = regfile[register][DSTReg];
    word_byte Result = regfile[register][DSTReg];
    //unsigned char msb;
    //unsigned char lsb;

    switch ((mst>>3) & 0b111)
    {
    case 0:
        /*sra, Arithmetic shift*/
        if (BIT(mst, 6) == 0)
        {
            Result.W = ((BIT(DSTVal.W, 15) == 1) ? 0X8000:0) + (DSTVal.W >> 1);
        }
        else
        {
            Result.B[0] = ((BIT(DSTVal.B[0], 7) == 1) ? 0X80 : 0) + (DSTVal.B[0] >> 1);
        }
        break;

    case 1:
        /*rrc*/
        if (BIT(mst, 6) == 0)
        {
            Result.W = ((BIT(DSTVal.W, 0) == 1) ? 0X8000 : 0) + (DSTVal.W >> 1);
        }
        {
            Result.B[0] = ((BIT(DSTVal.B[0], 0) == 1) ? 0X80 : 0) + (DSTVal.B[0] >> 1);
        }
        break;
    case 2:
        /*comp*/
        if (BIT(mst, 6) == 0)
        {
            Result.W = ~DSTVal.W;
        }
        else
        {
            Result.B[0] = ~DSTVal.B[0];
        }
        break;
    case 3:
        /*swpb*/
        Result.B[0] = DSTVal.B[1];
        Result.B[1] = DSTVal.B[0];
        break;
    case 4:
        /*sxt*/
        if (BIT(DSTVal.W, 7) == 0)
        {
            Result.B[1] = 0;
        }
        else
        {
            Result.B[1] = 1;
        }
        break;
    }
    regfile[register][DSTReg] = Result;
}

void mov_clrcc(unsigned short mst)
{
    unsigned char DSTReg = DST(mst);
    unsigned char SRCReg = SRC(mst);
    word_byte SRCVal;
    word_byte Result;

    switch ((mst >> 7) & 0b111)
    {
    case 0:
        /*mov*/
        SRCVal = regfile[register][SRCReg];
        Result = regfile[register][DSTReg];
        if (BIT(mst, 6) == 0)
        {
            Result = SRCVal;
        }
        else 
        {
            Result.B[0] = SRCVal.B[0];
        }
        regfile[register][DSTReg] = Result;
        break;
    case 1:
        /*swap*/
        SRCVal = regfile[register][SRCReg];
        regfile[register][SRCReg] = regfile[register][DSTReg];
        regfile[register][DSTReg] = SRCVal;
        break;
    case 2:
        /*sra to sxt*/
        sra_sxt(mst);
        break;
    case 3:
        /*Error*/
        printf("Error\n");
        break;
    }

}

void cex(unsigned short mst)
{

}

void ld_st(unsigned short mst)
{
    unsigned char DSTReg = DST(mst);
    unsigned char SRCReg = SRC(mst);
    unsigned char PRPO = (mst >> 9) & 0b1; //bit 9
    unsigned char DEC = (mst >> 8) & 0b1; //bit 8
    unsigned char INC = (mst >> 7) & 0b1; //bit 7
    unsigned char WB = (mst >> 6) & 0b1; //bit 6
    char ID = 0;
    unsigned short Addr;

    if (DEC)
    {
        /*If word, ID = -2*/
        ID = (WB == 0) ? -2 : -1;
    }
    else if (INC)
    {
        /*If word, ID = 2*/
        ID = (WB == 0) ? 2 : 1;
    }



    if ((BIT(mst, 10)) == 0)
    {
        /*LD instruction*/
        Addr = regfile[0][SRCReg].W;
        if (PRPO == 1)
        {
            /*Pre-incr*/
            Addr = Addr + ID;
            cache(Addr, &regfile[register][DSTReg], read, WB);
        }
        else
        {
            /*Post-incr*/
            cache(Addr, &regfile[register][DSTReg], read, WB);
            Addr = Addr + ID;
        }
        regfile[register][SRCReg].W = Addr;
    }
    else
    {
        /*ST instruction*/
        Addr = regfile[0][DSTReg].W;
        if (PRPO)
        {
            /*pre-incr*/
            Addr = Addr + ID;
            cache(Addr, &regfile[register][SRCReg], write, WB);
        }
        else
        {
            /*Post-incr*/
            cache(Addr, &regfile[register][SRCReg], write, WB);
            Addr = Addr + ID;
        }
        regfile[0][DSTReg].W = Addr;
    }
}

void add_st(unsigned short mst)
{
    switch ((mst >> 10) & 0b111)
    {
    case 0:
    case 1:
    case 2:
        add_bis(mst);
        break;
    case 3:
        mov_clrcc(mst);
        break;
    case 4:
        cex(mst);
        break;
    case 6:
    case 7:
        ld_st(mst);
        break;
    }
}

void movl_movh(unsigned short mst)
{
    unsigned char DSTReg = DST(mst);
    word_byte Result = regfile[register][DSTReg];
    unsigned char Byte = (mst>>3) & 0b11111111;
    switch ((mst>>11)&(0x3))
    {
    case 0:
        /*movl*/
        Result.B[Low] = Byte;
        break;
    case 1:
        /*movlz*/
        Result.B[Low] = Byte;
        Result.B[High] = 0;
        break;
    case 2:
        /*movls*/
        Result.B[Low] = Byte;
        Result.B[High] = 0xFF;
        break;
    case 3:
        /*movh*/
        Result.B[High] = Byte;
        break;
    }
    regfile[register][DSTReg].W = Result.W;
}

void ldr_str(unsigned short mst)
{
    unsigned char DSTReg = DST(mst);
    unsigned short Addr;
    unsigned char SRCReg = SRC(mst); 
    unsigned char WB = W_B(mst);
    short offset = (mst >> 7) & 0x7F;
    if ((offset >> 6) == 1)
    {
        /*If it is negative*/
        offset =(offset|0xFF80) ;
    }
    if ((BIT(mst,14)) == 0)
    {
        /*Ldr*/
        //printf("LDR");
        Addr = (regfile[register][SRCReg].W + offset) & ((WB == 0) ? 0xFFFE : 0xFFFF);
        cache(Addr, &regfile[0][DSTReg], read, WB);
    }
    else
    {
        /*Str*/
       // printf("STR");
        Addr = (regfile[register][DSTReg].W + offset) & ((WB == 0) ? 0xFFFE : 0xFFFF);
        cache(Addr, &regfile[0][SRCReg], write, WB);
    }
}

void decoder(unsigned short mst)
{
    //printf("sdsad    %x", mst);
    switch ((mst>>13)&0x07)
    {
    /*Check bit 15,14,13*/
    case 0:
        bl(mst);
        break;
    case 1:
        beq_bra(mst);
        break;
    case 2:
        add_st(mst);
        //PC = PC + 2;
        break;
    case 3:
        movl_movh(mst);
        //PC = PC + 2;
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        ldr_str(mst);
        //PC = PC + 2;
        break;
    }
    PC = PC + 2;
}

