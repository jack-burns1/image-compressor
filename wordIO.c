/**
 ** Max Mitchell and Jack Burns
 ** 
 ** wordPrinter.h
 ** Mar 4 2020
 ** Purpose: STUB
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "a2methods.h"
#include "a2plain.h"
#include "bitpack.h"
#include "wordIO.h"



void apply_writeWords(int col, int row, A2Methods_UArray2 array,
                      A2Methods_Object *ptr, void *cl)
{
    (void) array;
    (void) col;
    (void) row;
    assert(cl != NULL);
    assert(ptr != NULL);

    FILE *fp = cl;

    uint32_t word = *(uint32_t *)ptr;

    for (int i = 0; i < 4; i++) {
        int lsb = 24 - (8 * i);
        unsigned char byte = Bitpack_getu(word, 8, lsb);
        fputc(byte, fp);                // STUB  putc(byte, fp);
    }
}

void writeWords(A2Methods_UArray2 array)
{
    A2Methods_T methods = uarray2_methods_plain;
    assert(methods != NULL);

    FILE *fp = fopen("output.txt", "w");    // STUB

    unsigned width = methods->width(array) * 2;
    unsigned height = methods->height(array) * 2;
    fprintf(fp, "COMP40 Compressed image format 2\n%u %u", width, height);  // STUB
    fprintf(fp, "\n");                                                      // STUB

    methods->map_default(array, apply_writeWords, fp);       // STUB stdout
    fclose(fp);   // STUB
}



void apply_readWords(int col, int row, A2Methods_UArray2 array,
                     A2Methods_Object *ptr, void *cl)
{
    (void) col;
    (void) row;
    (void) array;

    FILE *fp = cl;
    uint32_t word = 0;

    for (int i = 0; i < 4; i++) {
        int lsb = 24 - (8 * i);
        unsigned char byte = getc(fp);
        word = Bitpack_newu(word, 8, lsb, byte);
    }

    *(uint32_t *)ptr = word;
}

A2Methods_UArray2 readWords(FILE *fp)
{
    A2Methods_T methods = uarray2_methods_plain;
    assert(methods != NULL);

    unsigned height, width;
    int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u", &width, &height);
    assert(read == 2);
    int c = getc(fp);
    assert(c == '\n');

    A2Methods_UArray2 bits = methods->new(width / 2, height / 2, sizeof(uint32_t));
    methods->map_default(bits, apply_readWords, fp);

    return bits;


}
