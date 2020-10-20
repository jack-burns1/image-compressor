/**
 ** Max Mitchell and Jack Burns
 ** 
 ** pixelBitpacker.h
 ** Mar 3 2020
 ** Purpose: STUB
 **/

#ifndef PIXELBITPACKER_INCLUDED
#define PIXELBITPACKER_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bitpack.h"
#include "a2methods.h"

void apply_pixelToBit(int col, int row, A2Methods_UArray2 pixels,
                      A2Methods_Object *ptr, void *cl);

void apply_bitToPixel(int col, int row, A2Methods_UArray2 pixels,
                      A2Methods_Object *ptr, void *cl);

#endif /* PIXELBITPACKER_INCLUDED */