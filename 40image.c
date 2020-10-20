/**
 ** Max Mitchell and Jack Burns
 ** 
 ** 40image.c
 ** Mar 2 2020
 ** Purpose: Driver for compression algorithim. Handles user input and
 **          calls functions to perform each step of the algorithim from
 **          various modules.
 **/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"
#include "compress40.h"
#include "COMP40compressor.h"


static void (*compress_or_decompress)(FILE *input) = compress40;


int main(int argc, char *argv[])
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1);    /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }

        return EXIT_SUCCESS; 
}

void compress40(FILE *fp)
{
        /* STEP ONE:
         * Read input and crop
         */
        Pnm_ppm ppm = readInput(fp);

        /* STEP TWO:
         * Convert from RGB to Component Video Color (CVC)
         */
        A2Methods_UArray2 video = rgbToCvc(ppm);

        /* STEP THREE & FOUR:
         * Chroma averaging and cosine transformation
         */
        A2Methods_UArray2 avg = chromaCosine(video);
        
        /* STEP FIVE:
         * Bitpacking
         */
        A2Methods_UArray2 bits = pixelToBit(avg);

        /* STEP SIX:
         * Write output
         */
        writeBits(bits);
}

void decompress40(FILE *fp)
{
        /* STEP ONE:
         * Read input
         */
        A2Methods_UArray2 bits = readBits(fp);

        /* STEP TWO:
         * Unpacking bits
         */
        A2Methods_UArray2 avg = bitToPixel(bits);


        /* STEP THREE & FOUR:
         * Inverse cosine transformation and chroma conversion
         */
        A2Methods_UArray2 video = inverseChroma(avg);

        /* STEP FIVE:
         * Convert from CVC to RGB
         */
        Pnm_ppm ppm = cvcToRgb(video);


        /* STEP SIX:
         * Write output
         */
        writeImage(ppm);
}
