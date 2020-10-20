/**
 ** Max Mitchell and Jack Burns
 ** 
 ** bitpack.c
 ** Feb 27 2020
 ** Purpose: STUB
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "bitpack.h"
#include <math.h>

#include "except.h"
Except_T Bitpack_Overflow = { "Overflow packing bits" };

/********** function prototypes **********/

bool Bitpack_fitsu(uint64_t n, unsigned width);
bool Bitpack_fitss( int64_t n, unsigned width);
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb); 
 int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb);
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value); 
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  
                       int64_t value); 


bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    assert(width <= 64);
    uint64_t bits = (uint64_t)1 << width;
    uint64_t range = (uint64_t)bits - 1;
    return n <= range;
}

bool Bitpack_fitss( int64_t n, unsigned width)
{
    assert(width <= 64);
    int64_t bits = (int64_t)1 << width;
    int64_t rangeUpper = ((int64_t)bits - 1) / 2;
    int64_t rangeLower = (rangeUpper * -1) - 1;

    return n >= rangeLower && n <= rangeUpper;
}


uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    uint64_t mask = ~0;
    mask = mask >> (64 - width) << lsb;

    uint64_t value = word & mask;
    value = value << (64 - width - lsb);
    value = value >> (64 - width);
    return value; 
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
 {
    uint64_t mask = ~0;
    mask = mask >> (64 - width) << lsb;
    mask = ~mask;

    int64_t value = word | mask;
    //value = value >> lsb;
    value = value << (64 - width - lsb);
    value = value >> (64 - width);
    return value; 
 }

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value)
{
    if ((lsb + width) > 64) {
        RAISE(Bitpack_Overflow);
        assert(0);
    }
    return Bitpack_news(word, width, lsb, value);
}


uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  
                       int64_t value)
{
    if ((lsb + width) > 64) {
        RAISE(Bitpack_Overflow);
        assert(0);
    }

    uint64_t mask = ~0;
    mask = mask >> (64 - width) << lsb;

    value = value << lsb;
    value = value & mask;

    mask = ~mask;

    return (word & mask) | value;
}







