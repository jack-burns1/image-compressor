/**
 ** Max Mitchell and Jack Burns
 ** 
 ** pixelBitpacker.c
 ** Mar 3 2020
 ** Purpose: STUB
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "bitpack.h"
#include "pixelBitpacker.h"
#include "pixelConverter.h"
#include "a2plain.h"



void apply_pixelToBit(int col, int row, A2Methods_UArray2 pixels,
                      A2Methods_Object *ptr, void *cl)
{
    (void) pixels;
    assert(ptr != NULL);
    assert(cl != NULL);

    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods);

    A2Methods_UArray2 bits = cl;

    Pixel_avg newPix = ptr;
    unsigned a = Pixel_avg_geta(newPix);
    int b = Pixel_avg_getb(newPix);
    int c = Pixel_avg_getc(newPix);
    int d = Pixel_avg_getd(newPix);
    unsigned pb = Pixel_avg_getpbavg(newPix);
    unsigned pr = Pixel_avg_getpravg(newPix);
    
    uint32_t word = 0;

    /* WIDTH of a, b, c, d changed to 6 */
    /* second parameter of fitss() changed to 6 (was 5) */
    /* values changed to 31 */
    /* was 15 before, which is highest # that can fit in 4 bits */
    /* 31 is highest number that can fit in 5 bits */

    if (Bitpack_fitss(b, 6) != true) {
        if (b < 0) {
            b  = -31;
        }
        else {
            b = 31;
        }
    }
    if (Bitpack_fitss(c, 6) != true) {
        if (c < 0) {
            c  = -31;
        }
        else {
            c = 31;
        }
    }
    if (Bitpack_fitss(d, 6) != true) {
        if (d < 0) {
            d  = -31;
        }
        else {
            d = 31;
        }
    }
    /* width and lsb changed according to the spec */
    word = Bitpack_newu(word, 6, 26, a);
    word = Bitpack_news(word, 6, 20, b); 
    word = Bitpack_news(word, 6, 14, c); 
    word = Bitpack_news(word, 6, 8, d); 
    word = Bitpack_newu(word, 4, 4, pb);
    word = Bitpack_newu(word, 4, 0, pr);

    *(uint32_t *)(methods->at(bits, col, row)) = word;
}

void apply_bitToPixel(int col, int row, A2Methods_UArray2 pixels,
                      A2Methods_Object *ptr, void *cl)
{
    (void) pixels;
    assert(ptr != NULL);
    assert(cl != NULL);

    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods);

    A2Methods_UArray2 bits = cl;

    uint32_t word = *(uint32_t *)(methods->at(bits, col, row));

    /* lsb and width changed according to spec */
    unsigned a = Bitpack_getu(word, 6, 26);
    int b = Bitpack_gets(word, 6, 20); 
    int c = Bitpack_gets(word, 6, 14); 
    int d = Bitpack_gets(word, 6, 8); 
    unsigned pbavg = Bitpack_getu(word, 4, 4);
    unsigned pravg = Bitpack_getu(word, 4, 0);

    Pixel_avg pixel = Pixel_avg_new(a, b, c, d, pbavg, pravg);

    Avg_UArray2_put(ptr, pixel);

    free(pixel);

}