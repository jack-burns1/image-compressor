/**
 ** Max Mitchell and Jack Burns
 ** 
 ** pixelConerter.h
 ** Feb 28 2020
 ** Purpose: STUB
 **/

#ifndef PIXELCONVERTER_INCLUDED
#define PIXELCONVERTER_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "a2methods.h"
#include "uarray2.h"
#include "stack.h"


#define V Pixel_cvc
typedef struct V *V;

#define A Pixel_avg
typedef struct A *A;

#define U Avg_UArray2
typedef struct U *U;

Avg_UArray2 Avg_UArray2_new(int width, int height);
Avg_UArray2 Avg_UArray2_new_with_array(A2Methods_UArray2 avg);
void Avg_UArray2_free(Avg_UArray2 *array);
A2Methods_UArray2 Avg_UArray2_getUArray2(Avg_UArray2 array);
void Avg_UArray2_put(void *ptr, Pixel_avg pixel);


unsigned Pixel_avg_geta(Pixel_avg pixel);
int Pixel_avg_getb(Pixel_avg pixel);
int Pixel_avg_getc(Pixel_avg pixel);
int Pixel_avg_getd(Pixel_avg pixel);
unsigned Pixel_avg_getpbavg(Pixel_avg pixel);
unsigned Pixel_avg_getpravg(Pixel_avg pixel);
Pixel_avg Pixel_avg_new(unsigned a, int b, int c, int d, 
                        unsigned pbavg, unsigned pravg);


int sizeofPixel_cvc();
int sizeofPixel_avg();

void apply_rgbToCvc(int col, int row, A2Methods_UArray2 video,
                          A2Methods_Object *ptr, void *cl);

void apply_cvcToRgb(int col, int row, A2Methods_UArray2 video,
                          A2Methods_Object *ptr, void *cl);



void apply_cosineChroma(int col, int row, A2Methods_UArray2 ppm,
                     A2Methods_Object *ptr, void *cl);

void apply_inverseChroma(int col, int row, A2Methods_UArray2 ppm,
                            A2Methods_Object *ptr, void *cl);

#undef Pixel_avg
#undef Pixel_cvc
#undef Avg_UArray2

#endif /* PIXELCONVERTER_INCLUDED */
