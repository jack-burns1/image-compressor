/**
 ** Max Mitchell & Jack Burns
 ** uarray2b.c
 ** 13 February 2020
 **
 ** Purpose: ADT for storing data in 2D blocked UArray
 **/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <uarray.h>
#include "uarray2.h"
#include "a2methods.h"
#include "uarray2b.h"
#include <math.h>

#define SIXTYFOURK 65536

#define T UArray2b_T

/********** struct definitions **********/

struct T
{
    int BLOCK_ROWS;
    int BLOCK_COLS;
    int MAX_ROWS;
    int MAX_COLS;
    int size;
    int blocksize;
    int numCells;
    UArray2_T array;
};

/* @function: UArray2b_new
 * @purpose: Initialize new UArray2b
 *
 * @precondition: 1) width is >= 0
 *                2) height is >= 0
 *                3) size is > 0
 *                4) blocksize >= 0
 * @postcondition: new type T has been created and returned
 *
 * @parameters: 1) int width, which is going to be the width of the
 *                 new UArray2b
 *              2) int height, which is going to be the height of the
 *                 new UArray2b
 *              3) int size, which is the bytes per element for the type
 *                 that will be stored in the UArray2b
 *              4) int blocksize, which is the size of one side of the 
 *                 'blocks' in the UArray2b
 * @returns: type T, which is the UArray2b
 */
extern T UArray2b_new (int width, int height, int size, int blocksize)
{
    assert(width >= 0);
    assert(height >= 0);
    assert(size > 0);
    assert(blocksize > 0);

    T uarray2b = malloc(sizeof(*uarray2b));
    assert(uarray2b != NULL);

    /* dividing width and height by blocksize and rounding up. 
     * This gives us the number of blocks we need and ensures
     * we won't run out of space.
     */
    int roundedWidth = width / blocksize;
    if (blocksize % width != 0) {
        roundedWidth += 1;
    }
    int roundedHeight = height / blocksize;
    if (blocksize % height != 0) {
        roundedHeight += 1;
    }   

    uarray2b->MAX_ROWS = height;
    uarray2b->BLOCK_ROWS = roundedHeight;

    uarray2b->MAX_COLS = width;
    uarray2b->BLOCK_COLS = roundedWidth;

    uarray2b->blocksize = blocksize;
    uarray2b->numCells = blocksize * blocksize;
    uarray2b->size = size;

    uarray2b->array = UArray2_new(roundedWidth, roundedHeight, 
                                  sizeof(UArray_T));

    for (int row = 0; row < uarray2b->BLOCK_ROWS; row++) {
        for (int col = 0; col < uarray2b->BLOCK_COLS; col++) {
            *(UArray_T *)UArray2_at(uarray2b->array, col, row) = 
                         UArray_new(uarray2b->numCells, size);
        }
    }

    return uarray2b;
}

/* @function: UArray2b_new_64K_block
 * @purpose: Initialize new UArray2b with blocksize such that
 *           blocks are as big as possible while fitting within
 *           64 kilobytes (or 65,536 bytes)
 *
 * @precondition: 1) width is >= 0
 *                2) height is >= 0
 *                3) size is > 0
 * @postcondition: new type T has been created and returned
 *
 * @parameters: 1) int width, which is going to be the width of the
 *                 new UArray2b
 *              2) int height, which is going to be the height of the
 *                 new UArray2b
 *              3) int size, which is the bytes per element for the type
 *                 that will be stored in the UArray2b
 * @returns: type T, which is the UArray2b
 */
extern T UArray2b_new_64K_block(int width, int height, int size)
{
    assert(width >= 0);
    assert(height >= 0);
    assert(size > 0);
    double numCells = SIXTYFOURK / size;
    /*must round down*/
    int blocksize = sqrt(numCells);
    if (blocksize == 0) {
        blocksize = 1;
    }
    return UArray2b_new(width, height, size, blocksize);
}

/* @function: UArray2_free
 * @purpose: deallocate and free all memory associated with UArray2b
 *
 * @precondition: T *array2b must be valid and initialized type T
 * @postcondition: all memory associated uarray2 must be freed
 *
 * @parameters: T *array2b, which is valid and initialized type T *
 *              whose memory is going to be freed
 * @returns: none
 */
extern void UArray2b_free (T *array2b)
{
    assert(array2b != NULL);
    for (int row = 0; row < (*array2b)->BLOCK_ROWS; row++) {
        for (int col = 0; col < (*array2b)->BLOCK_COLS; col++) {
            UArray_free(((UArray_T *)(UArray2_at((*array2b)->array, 
                        col, row))));
        }
    }
    UArray2_free(&(*array2b)->array);
    free(*array2b);
}

/* @function: UArray2b_width 
 * @purpose: return width of a given UArray2b
 *
 * @precondition: T array2b is valid and initialized type T
 * @postcondition: width of T array2b has been returned
 *
 * @parameters: T array2b, which is type T whose width the client
 *              wants to know
 * @returns: integer value equal to the width of T array2b
 */
extern int UArray2b_width (T array2b)
{
    assert(array2b != NULL);
    return array2b->MAX_COLS;
}

/* @function: UArray2b_height 
 * @purpose: return height of a given UArray2b
 *
 * @precondition: T array2b is valid and initialized type T
 * @postcondition: height of T array2b has been returned
 *
 * @parameters: T array2b, which is type T whose height the client
 *              wants to know
 * @returns: integer value equal to the height of T array2b
 */
extern int UArray2b_height (T array2b)
{
    assert(array2b != NULL);
    return array2b->MAX_ROWS;
}

/* @function: UArray2_size
 * @purpose: return the size of a single element in the UArray2
 *
 * @precondition: T array2b is valid and initialized type T
 * @postcondition: size of an element in T array2b is returned
 *
 * @parameters: T array2b, which is type T whose size the client
 *              wants to know
 * @returns: integer value equal to the size of an element in T array2b
 */
extern int UArray2b_size (T array2b)
{
    assert(array2b != NULL);
    return array2b->size;
}

/* @function: UArray2b_blocksize
 * @purpose: return the size of a one side of a 'block' from the UArray2
 *
 * @precondition: T array2b is valid and initialized type T
 * @postcondition: size of one side of a 'block' in T array2b is returned
 *
 * @parameters: T array2b, which is type T whose blocksize the client
 *              wants to know
 * @returns: integer value equal to the blocksize of T array2b
 */
extern int UArray2b_blocksize(T array2b)
{
    assert(array2b != NULL);
    return array2b->blocksize;
}

/* @function: UArray2b_at
 * @purpose: allows client to index value in T array2b
 *
 * @precondition: 1) T array2b is valid and initialized type T
 *                2) row must be less than height of array2b and 
 *                   greater than 0
 *                3) col must be less than width of array2b and 
 *                   greater than 0
 * @postcondition: void pointer to element at position (row, col) will
 *                 be returned
 *
 * @parameters: 1) T array2b, which is UArray2b which the client wants
 *                 to index
 *              2) int col, which is the col the element the client
 *                 wants is in
 *              3) int row, which is the row the element the client 
 *                 wants is in
 * @returns: void pointer to element in uarray2 at position (col, row)
 */
extern void *UArray2b_at(T array2b, int column, int row)
{
    assert(array2b != NULL);
    assert(row >= 0);
    assert(row < array2b->MAX_ROWS);
    assert(column >= 0);
    assert(column < array2b->MAX_COLS);

    int blockRow = row / array2b->blocksize;
    int blockCol = column / array2b->blocksize;
    UArray_T block;
    block = *(UArray_T *)UArray2_at(array2b->array, blockCol, blockRow);

    int index = (array2b->blocksize * (row % array2b->blocksize))
                 + (column % array2b->blocksize);
    return UArray_at(block, index);
}


/* @function: UArray2_map
 * @purpose: map function which performs function void apply to all
 *           elements in array2b, starting with the first block and iterating
 *           block-major (first all elements in block one, then block two, 
 *           etc.)
 *
 * @precondition: 1) T array2b is valid and initialized type T
 *                2) void apply is valid function following 
 *                   parameter specifications
 * @postcondition: function void apply will have been run on all elements,
 *                 in block-major order
 *
 * @parameters: 1) T array2b, which is UArray2b whose elements are being acted
 *                 upon
 *              2) void apply(int col, int row, T array2b, 
 *                 void *elem, void *cl), which is function that will 
 *                 be run on all elements   
 *              3) void *cl, which is the client specifed pointer
 * @returns: none
 */
extern void UArray2b_map(T array2b,
                        void apply(int col, int row, T array2b,
                                   void *elem, void *cl),
                        void *cl)
{
    assert(array2b != NULL);
    assert(apply != NULL);

    int blocksize = array2b->blocksize;
    for (int i = 0; i < array2b->BLOCK_ROWS; i++) {
        for (int j = 0; j < array2b->BLOCK_COLS; j++) {
            for (int k = 0; k < array2b->numCells; k++) {
                int col = (k % blocksize) + (blocksize * j);
                int row = (k / blocksize) + (blocksize * i);

                if (row >= array2b->MAX_ROWS || col >= array2b->MAX_COLS) {
                    continue;
                }

                void *elem = UArray2b_at(array2b, col, row);
                apply(col, row, array2b, elem, cl);
            }
        }
    }
}




