/**
 ** Max Mitchell and Jack Burns
 ** 
 ** COMP40compressor.c
 ** Mar 4 2020
 ** Purpose: Implementation of 40compressor.h. These files are used to
 **          organize primary functions to be used by driver 40image.c. This
 **          module serves as a means to simplify the driver and hide the
 **          majority of the implementation from the client.
 **/

#include "bitpack.h"
#include "pixelConverter.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "stack.h"
#include "pixelBitpacker.h"
#include "wordIO.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/********* function prototypes *********/
void apply_ppmCopy(int col, int row, A2Methods_UArray2 pixels,
                   A2Methods_Object *ptr, void *cl);


/********** compression suite **********/
Pnm_ppm readInput(FILE *fp)
{
    assert(fp != NULL);
    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods);

    Pnm_ppm ppm = Pnm_ppmread(fp, methods);
    unsigned width = ppm->width;
    unsigned height = ppm->height;

    /* if width or height is not even, trim */
    if (ppm->width % 2 != 0) {
        width -= 1;
    }
    if (ppm->height % 2 != 0) {
        height -= 1;
    }

    /* makes copy of ppm that is properly trimmed and sized */
    A2Methods_UArray2 trimmed = methods->new(width, height, sizeof(struct Pnm_rgb));
    methods->map_default(trimmed, apply_ppmCopy, &ppm);

    A2Methods_UArray2 pixels = ppm->pixels;
    methods->free(&pixels);

    /* assign new info to ppm and return */
    ppm->width = width;
    ppm->height = height;
    ppm->pixels = trimmed;

    return ppm;
}


A2Methods_UArray2 rgbToCvc(Pnm_ppm ppm)
{
    assert(ppm != NULL);
    A2Methods_T methodsBlocked = uarray2_methods_blocked; 
    assert(methodsBlocked);

    int width = ppm->width;
    int height = ppm->height;

    /* making blocked array to hold our Pixel_cvc structs */
    A2Methods_UArray2 video = 
    methodsBlocked->new_with_blocksize(width, height, sizeofPixel_cvc(), 2);

    /* converts all pixels in ppm from Pnm_rgb (RGB) to Pixel_cvc
     * (Component Video Color) 
     */
    methodsBlocked->map_default(video, apply_rgbToCvc, &ppm);

    Pnm_ppmfree(&ppm);
    return video;
}

A2Methods_UArray2 chromaCosine(A2Methods_UArray2 video)
{
    assert(video != NULL);
    A2Methods_T methodsBlocked = uarray2_methods_blocked; 
    assert(methodsBlocked);

    int width = methodsBlocked->width(video);
    int height = methodsBlocked->height(video);

    /* makes array with halved width and height, as we are now
     * averaging together each 2x2 block of pixels into one
     */ 
    Avg_UArray2 avg_array = Avg_UArray2_new(width / 2, height / 2);
    assert(avg_array != NULL);

    /* applys cosine transformation and averages Pb and Pr values
     * together
     */
    methodsBlocked->map_block_major(video, apply_cosineChroma, avg_array);
    methodsBlocked->free(&video);
    A2Methods_UArray2 array = Avg_UArray2_getUArray2(avg_array);

    Avg_UArray2_free(&avg_array);
    return array;
}

A2Methods_UArray2 pixelToBit(A2Methods_UArray2 avg)
{
    assert(avg != NULL);
    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods);

    int width = methods->width(avg);
    int height = methods->height(avg);

    /* new array that will hold bitpacked data in integers */
    A2Methods_UArray2 bits = methods->new(width, height, sizeof(uint32_t));
    assert(bits !=NULL);
    /* converts data to bitpacked integers and inserts into new array bits */
    methods->map_default(avg, apply_pixelToBit, bits);

    methods->free(&avg);
    return bits;
}

void writeBits(A2Methods_UArray2 bits)
{
    assert(bits != NULL);
    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods);

    /* writes to stdout */
    writeWords(bits);
    methods->free(&bits);
}

/********* decompression suite *********/
A2Methods_UArray2 readBits(FILE *fp)
{
    assert(fp != NULL);
    /* reads input from file and writes to array */
    A2Methods_UArray2 bits = readWords(fp);
    return bits;
}

A2Methods_UArray2 bitToPixel(A2Methods_UArray2 bits)
{
    assert(bits != NULL);
    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods);

    int width = methods->width(bits);
    int height = methods->height(bits);

    /* makes new array and unpacks bits to our Pixel_avg type */
    A2Methods_UArray2 avg = methods->new(width, height, sizeofPixel_avg());
    methods->map_default(avg, apply_bitToPixel, bits);

    methods->free(&bits);
    return avg;
}

A2Methods_UArray2 inverseChroma(A2Methods_UArray2 avg)
{
    assert(avg != NULL);
    A2Methods_T methodsBlocked = uarray2_methods_blocked;
    assert(methodsBlocked != NULL);

    A2Methods_T methods = uarray2_methods_plain;
    assert(methods != NULL);

    /* creates Avg_UArray2 type needed to decompress from Pixel_avg
     * back to Pixel_cvc
     */
    Avg_UArray2 avg_array = Avg_UArray2_new_with_array(avg);

    int width = methods->width(avg) * 2;
    int height = methods->height(avg) * 2;

    A2Methods_UArray2 video = 
    methodsBlocked->new_with_blocksize(width, height, sizeofPixel_cvc(), 2);

    /* maps and performs inverse cosine transformation and reassigns avg
     * Pb and Pr values to each Pixel_cvc
     */
    methodsBlocked->map_block_major(video, apply_inverseChroma, avg_array);

    methods->free(&avg);
    Avg_UArray2_free(&avg_array);
    return video;
}

Pnm_ppm cvcToRgb(A2Methods_UArray2 video)
{
    assert(video != NULL);
    A2Methods_T methodsBlocked = uarray2_methods_blocked;
    assert(methodsBlocked != NULL);

    int width = methodsBlocked->width(video);
    int height = methodsBlocked->height(video);

    A2Methods_T methods = uarray2_methods_plain;
    assert(methods != NULL);

    /* make new array to hold RGB values in Pnm_rgb type. These will be
     * be stored in the Pnm_ppm we are about to create
     */
    A2Methods_UArray2 array = methods->new(width, height, sizeof(struct Pnm_rgb));

    /* making Pnm_ppm and assigning proper values */
    Pnm_ppm ppm = malloc(sizeof(*ppm));
    ppm->pixels = array;
    ppm->width = width;
    ppm->height = height;
    /* 255 picked as it is the range limit of RGB values (0-255) */
    ppm->denominator = 255;
    ppm->methods = methods;

    /* apply function converts from Pixel_cvc to Pnm_rgb */
    methodsBlocked->map_default(video, apply_cvcToRgb, &ppm);

    methodsBlocked->free(&video);
    return ppm;
}

void writeImage(Pnm_ppm ppm)
{
    assert(ppm != NULL);
    /* writes ppm file to stdout */
    FILE *fp = fopen("output.txt", "w");
    Pnm_ppmwrite(fp, ppm);   // STUB Pnm_ppmwrite(stdout, ppm);
    fclose(fp);
    Pnm_ppmfree(&ppm);
}




/********** helper functions **********/

void apply_ppmCopy(int col, int row, A2Methods_UArray2 pixels,
                          A2Methods_Object *ptr, void *cl)
{
    /* copies from ppm in cl onto uarray pixels */
    (void) pixels;

    assert(ptr != NULL);
    assert(cl != NULL);

    Pnm_ppm *ppm = cl;

    *(Pnm_rgb)ptr = *(Pnm_rgb)((*ppm)->methods->at((*ppm)->pixels, 
                                                     col, row)); 
}