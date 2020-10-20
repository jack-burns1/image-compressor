/**
 ** Max Mitchell and Jack Burns
 ** 
 ** pixelConverter.c
 ** Feb 28 2020
 ** Purpose: STUB
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#include "bitpack.h"
#include "pnm.h"
#include "pixelConverter.h"
#include "a2methods.h"
#include "uarray2.h"
#include "arith40.h"
#include "a2plain.h"
#include "stack.h"

#define QUANTIZE_UNSIGNED 63.0
#define QUANTIZE_SIGNED 103.3

/* Quantized values must be changed */
/* multiplying .3 by 103.3 gives 31 (upper limit) */
/* for 'a', 63 is the highest number held in 6 bits */

#define V Pixel_cvc
#define A Pixel_avg
#define U Avg_UArray2

/********** struct definitions **********/
struct V {
    float y, pb, pr;     
};

struct A {
    unsigned a;
    int b, c, d;
    unsigned pbavg, pravg;
};

struct U {
    A2Methods_T methods;
    A2Methods_UArray2 avg;
    float sumOfPb, sumOfPr;
    Stack_T y;
};


/********** function prototypes **********/
float rgbInBounds(float rgb);
void chromaAverage(int col, int row, Avg_UArray2 arrayAvg, A newPix);
void cosineTransform(int col, int row, Avg_UArray2 arrayAvg);



Avg_UArray2 Avg_UArray2_new(int width, int height)
{
    assert(width >= 0);
    assert(height >= 0);

    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods);
    
    return Avg_UArray2_new_with_array(methods->new(width, height, sizeof(struct A)));
}
Avg_UArray2 Avg_UArray2_new_with_array(A2Methods_UArray2 avg)
{
    U array = malloc(sizeof(struct U));
    assert(array != NULL);
    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods);
    array->methods = methods;
    array->avg = avg;
    array->y = Stack_new();
    array->sumOfPb = 0;
    array->sumOfPr = 0;
    return array;
}
void Avg_UArray2_free(Avg_UArray2 *array)
{
    assert(array != NULL);
    Stack_T y = (*array)->y;
    Stack_free(&y);

    free(*array);
}
A2Methods_UArray2 Avg_UArray2_getUArray2(Avg_UArray2 array)
{
    assert(array != NULL);
    return array->avg;
}
void Avg_UArray2_put(void *ptr, Pixel_avg pixel)
{
    *(A)ptr = *pixel;
}



int sizeofPixel_avg()
{
    return sizeof(struct A);
}
int sizeofPixel_cvc()
{
    return sizeof(struct V);
}
int sizeofAvg_UArray2()
{
    return sizeof(struct U);
}


unsigned Pixel_avg_geta(Pixel_avg pixel)
{
    assert(pixel != NULL);
    return pixel->a;
}
int Pixel_avg_getb(Pixel_avg pixel)
{
    assert(pixel != NULL);
    return pixel->b;
}
int Pixel_avg_getc(Pixel_avg pixel)
{
    assert(pixel != NULL);
    return pixel->c;
}
int Pixel_avg_getd(Pixel_avg pixel)
{
    assert(pixel != NULL);
    return pixel->d;
}
unsigned Pixel_avg_getpbavg(Pixel_avg pixel)
{
    assert(pixel != NULL);
    return pixel->pbavg;
}
unsigned Pixel_avg_getpravg(Pixel_avg pixel)
{
    assert(pixel != NULL);
    return pixel->pravg;
}
Pixel_avg Pixel_avg_new(unsigned a, int b, int c, int d, 
                        unsigned pbavg, unsigned pravg)
{
    A pixel = malloc(sizeof(struct A));
    pixel->a = a;
    pixel->b = b;
    pixel->c = c;
    pixel->d = d;
    pixel->pbavg = pbavg;
    pixel->pravg = pravg;
    return pixel; 
}

void apply_rgbToCvc(int col, int row, A2Methods_UArray2 video,
                          A2Methods_Object *ptr, void *cl)
{
    /* essentially this apply function is called on a
     * new array that is to hold video component color
     * pixels. It converts a ppm of rgb pixels to cvc
     * values. The ppm holding the rgb pixels for the
     * image is passed in thru cl. 
     */

    (void) video;
    assert(ptr != NULL);
    assert(cl != NULL);

    Pnm_ppm *ppm = cl;

    float denominator = (float)((*ppm)->denominator);
    Pnm_rgb rgb = (Pnm_rgb)((*ppm)->methods->at((*ppm)->pixels, 
                                                     col, row));
    V cvc = (V)ptr;

    float r = (float)rgb->red / denominator;
    float g = (float)rgb->green / denominator;
    float b = (float)rgb->blue / denominator;

    cvc->y = 0.299 * r + 0.587 * g + 0.114 * b;
    cvc->pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
    cvc->pr = 0.5 * r - 0.418688 * g - 0.081312 * b;
}

void apply_cvcToRgb(int col, int row, A2Methods_UArray2 video,
                          A2Methods_Object *ptr, void *cl)
{
    /* this function works exactly like the one
     * above, in reverse. While it is still called
     * on a array of video color, it access the ppm
     * in cl and modifies each pixel based on the
     * values in the corresponding pixel in cvc
     * (after converting back to rgb, of course)
     */

    (void) video;
    assert(ptr != NULL);
    assert(cl != NULL);

    Pnm_ppm *ppm = cl;

    float denominator = (float)((*ppm)->denominator);
    Pnm_rgb rgb = (Pnm_rgb)((*ppm)->methods->at((*ppm)->pixels, 
                                                     col, row));
    V cvc = (V)ptr;

    float y = cvc->y;
    float pb = cvc->pb;
    float pr = cvc->pr;

    float red = denominator * (1.0 * y + 0.0 * pb + 1.402 * pr);
    rgb->red = rgbInBounds(red);

    float green = denominator * (1.0 * y - 0.344136 * pb - 0.714136 * pr);
    rgb->green = rgbInBounds(green);

    float blue = denominator * (1.0 * y + 1.772 * pb + 0.0 * pr);
    rgb->blue = rgbInBounds(blue);
}

float rgbInBounds(float rgb)
{
    if (rgb < 0) {
        return 0;
    }
    if (rgb > 255) {
        return 255;
    }
    return rgb;
}


void apply_cosineChroma(int col, int row, A2Methods_UArray2 video,
                        A2Methods_Object *ptr, void *cl)
{
    (void) video;
    assert(ptr != NULL);
    assert(cl != NULL);
    /* create variable of the closure struct type */
    U arrayAvg = cl;
    V cvc = (V)ptr;

    /* add to pbSum and prSum */
    arrayAvg->sumOfPb += cvc->pb;
    arrayAvg->sumOfPr += cvc->pr;

    /* track the next y value */
    Stack_push(arrayAvg->y, &(cvc->y));

    /* testing if in bottom right corner of 2x2 block */
    if (row % 2 != 0 && col % 2 != 0) {
        /* create new pixel to store in the array and assign the correct average values*/
        A newPix = malloc(sizeof(*newPix));
        assert(newPix != NULL);

        chromaAverage(col, row, arrayAvg, newPix);
        cosineTransform(col, row, arrayAvg); 
        free(newPix);
    }
} 

void chromaAverage(int col, int row, Avg_UArray2 arrayAvg, A newPix)
{
    A2Methods_T methods = arrayAvg->methods; 
    assert(methods);

    /* create local variables for the total sums*/
    float sumOfPb = arrayAvg->sumOfPb;
    float sumOfPr = arrayAvg->sumOfPr;

    /* Divide sums by 4 Convert to 4 bit values (use given function) */
    unsigned pbAverage = Arith40_index_of_chroma(sumOfPb / 4.0);
    unsigned prAverage = Arith40_index_of_chroma(sumOfPr / 4.0);

    /* Add to correct Pixel_avg in *cl (divide row and col by two using int math) */
    int newCol = col / 2;
    int newRow = row / 2;

    /* add values */
    newPix->pbavg = pbAverage;
    newPix->pravg = prAverage;

    /* add newPix to the array in the closure */
    *(A)(methods->at(arrayAvg->avg, newCol, newRow)) = *newPix;

    /* reset original sums to 0 */
    arrayAvg->sumOfPb = 0.0;
    arrayAvg->sumOfPr = 0.0;
}

void cosineTransform(int col, int row, Avg_UArray2 arrayAvg)
{
    int newCol = col / 2;
    int newRow = row / 2;
    A newPix = arrayAvg->methods->at(arrayAvg->avg, newCol, newRow);

    Stack_T y = arrayAvg->y;
    float Y4 = *(float *)Stack_pop(y);
    float Y3 = *(float *)Stack_pop(y);
    float Y2 = *(float *)Stack_pop(y);
    float Y1 = *(float *)Stack_pop(y);

    newPix->a = QUANTIZE_UNSIGNED * ((Y4 + Y3 + Y2 + Y1) / 4.0);
    newPix->b = QUANTIZE_SIGNED * (Y4 + Y3 - Y2 - Y1) / 4.0;
    newPix->c = QUANTIZE_SIGNED * (Y4 - Y3 + Y2 - Y1) / 4.0;
    newPix->d = QUANTIZE_SIGNED * (Y4 - Y3 - Y2 + Y1) / 4.0;
}

void apply_inverseChroma(int col, int row, A2Methods_UArray2 ppm,
                            A2Methods_Object *ptr, void *cl)
{
    (void) ppm;
    assert(ptr != NULL);
    assert(cl != NULL);

    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods);

    int newCol = col / 2;
    int newRow = row /2;

    U arrayAvg = cl;

    V cvc = (V)ptr;
    A oldPixel = methods->at(arrayAvg->avg, newCol, newRow);

    /* assign pbavg and pravg to pb and pr */
    cvc->pb = Arith40_chroma_of_index(oldPixel->pbavg);
    cvc->pr = Arith40_chroma_of_index(oldPixel->pravg);

    float a = ((float)oldPixel->a) / QUANTIZE_UNSIGNED;
    float b = ((float)oldPixel->b) / QUANTIZE_SIGNED;
    float c = ((float)oldPixel->c) / QUANTIZE_SIGNED;
    float d = ((float)oldPixel->d) / QUANTIZE_SIGNED;
    /* calculate Y values and reassign */
    if (col % 2 == 0 && row % 2 == 0) {
        /* top left cell of block, Y1 */
        /* Y1 = a - b - c + d */
        cvc->y = a - b - c + d;
    }
    else if (col % 2 != 0 && row % 2 == 0) {
        /* top right cell of block, Y2 */
        /* Y2 = a - b + c - d */
        cvc->y = a - b + c - d;
    }
    else if (col % 2 == 0 && row % 2 != 0) {
        /* bottom left cell of block, Y3 */
        /* Y3 = a + b - c - d */
        cvc->y = a + b - c - d;
    }
    else {
        /* bottom right cell of block, Y4 */
        /* Y4 = a + b + c + d */
        cvc->y = a + b + c + d;
    }

}


#undef V
#undef A
#undef U
