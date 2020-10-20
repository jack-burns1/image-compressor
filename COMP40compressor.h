/**
 ** Max Mitchell and Jack Burns
 ** 
 ** COMP40compressor.h
 ** Mar 4 2020
 ** Purpose: Interface for 40compressor.c, contains basic step-by-step
 **          functions for compression algorithim
 **/

#ifndef COMP40COMPRESSOR_INCLUDED
#define COMP40COMPRESSOR_INCLUDED

#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"

#include <stdlib.h>
#include <stdio.h>


/*  NOTE:

    These functions operate on a simple input output
    system indicated by the parameter and return value.
    It is expected that if you are compressing/decompressing, 
    you will run each of these functions essentially "in" to the
    next, i.e. the output of one function becomes the input
    of the next function. While this is not necessary, 
    and each step can be run independently, this is the 
    easiest way to guarentee input will be true to
    to what each function expects.

 */



/********** compression suite **********/

Pnm_ppm readInput(FILE *fp);
/* fp is expected to be file pointer to ppm file */
A2Methods_UArray2 rgbToCvc(Pnm_ppm ppm); 
/* rgbToCvc returns a blocked uarray2 of struct Pixel_cvc */
A2Methods_UArray2 chromaCosine(A2Methods_UArray2 video); 
/* video is expected to be a blocked uarray2 holding struct Pixel_cvc,
 * and chromaCosine returns a plain uarray2 of struct Pixel_avg   
 */
A2Methods_UArray2 pixelToBit(A2Methods_UArray2 avg);
/* avg is expected to be a plain uarray2 holding struct Pixel_avg,
 * and pixelToBit returns a plain uarray2 of uint32_t
 */
void writeBits(A2Methods_UArray2 bits);
/* bits is expected to be a plain uarray2 holding  uint32_t */



/********* decompression suite *********/

A2Methods_UArray2 readBits(FILE *fp);
/* fp is expected to be a file pointer to a COMP40 Compressed image format 2
 * file, and readBits returns a plain uarray2 of uint32_t
 */
A2Methods_UArray2 bitToPixel(A2Methods_UArray2 bits);
/* bits is expected to be a plain uarray2 of uint32_t, 
 * and bitToPixel returns a plain uarray2 of struct Pixel_avg
 */
A2Methods_UArray2 inverseChroma(A2Methods_UArray2 avg);
/* avg is expected to be a plain uarray2 of struct Pixel_avg,
 * and inverseChroma returns a blocked uarray2 of struct Pixel_cvc
 */
Pnm_ppm cvcToRgb(A2Methods_UArray2 video);
/* video is expected to be a plain uarray2 of struct Pixel_cvc */
void writeImage(Pnm_ppm ppm);
/* ppm is expected to be a valid Pnm_ppm */




/* NOTICE: it is a checked runtime error to pass
 * a NULL value to any function in this interface
 */

#endif /* COMP40COMPRESSOR_INCLUDED */


