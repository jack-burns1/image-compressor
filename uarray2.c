/**
 ** Max Mitchell & Jack Burns
 ** uarray2.c
 ** 4 February 2020
 **
 ** Purpose: ADT for storing data in 2D unboxed array
 **/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "a2methods.h"
#include "uarray.h"
#include "mem.h"
#include "uarray2.h"

#define T UArray2_T
//typedef struct T *T;

/********** struct definitions **********/

struct T
{
    int MAX_ROWS;
    int MAX_COLS;
    int size;
    UArray_T array;
};


/* @function: UArray2_new
 * @purpose: Initialize new UArray2
 *
 * @precondition: 1) width is >= 0
 *                2) height is >= 0
 *                3) size is > 0
 * @postcondition: new type T has been created and returned
 *
 * @parameters: 1) int width, which is going to be the width of the
 *                 new UArray2
 *              2) int height, which is going to be the height of the
 *                 new UArray2
 *              3) int size, which is the bytes per element for the type
 *                 that will be stored in the UArray2
 * @returns: type T, which is the UArray2 
 */
T UArray2_new(int width, int height, int size)
{
    assert(width >= 0);
    assert(height >= 0);
    assert(size > 0);

    T uarray2;

    uarray2 = malloc(sizeof(*uarray2));
    assert(uarray2 != NULL);

    uarray2->MAX_ROWS = height;
    uarray2->MAX_COLS = width;
    uarray2->size = size;

    uarray2->array = UArray_new(height * width, size);

    return uarray2;
}

/* @function: UArray2_free
 * @purpose: deallocate and free all memory associated with UArray2
 *
 * @precondition: T *uarray2 must be valid and initialized type T
 * @postcondition: all memory associated uarray2 must be freed
 *
 * @parameters: T *uarray2, which is valid and initialized type T *
 *              whose memory is going to be freed
 * @returns: none
 */
void UArray2_free(T *uarray2)
{
    assert(uarray2 != NULL);

    UArray_free(&(*uarray2)->array);
    free(*uarray2);
}


/* @function: UArray2_width 
 * @purpose: return width of a given UArray2
 *
 * @precondition: T uarray2 is valid and initialized type T
 * @postcondition: width of T uarray2 has been returned
 *
 * @parameters: T uarray2, which is type T whose width the client
 *              wants to know
 * @returns: integer value equal to the width of T uarray2
 */
int UArray2_width(T uarray2)
{
    assert(uarray2 != NULL);
    return uarray2->MAX_COLS;
}

/* @function: UArray2_height 
 * @purpose: return height of a given UArray2
 *
 * @precondition: T uarray2 is valid and initialized type T
 * @postcondition: height of T uarray2 has been returned
 *
 * @parameters: T uarray2, which is type T whose height the client
 *              wants to know
 * @returns: integer value equal to the height of T uarray2
 */
int UArray2_height(T uarray2)
{
    assert(uarray2 != NULL);
    return uarray2->MAX_ROWS;
}

/* @function: UArray2_size
 * @purpose: return the size of a single element in the UArray2
 *
 * @precondition: T uarray2 is valid and initialized type T
 * @postcondition: size of an element in T uarray2 is returned
 *
 * @parameters: T uarray2, which is type T whose size the client
 *              wants to know
 * @returns: integer value equal to the size of an element in T uarray2
 */
int UArray2_size(T uarray2)
{
    assert(uarray2 != NULL);
    return uarray2->size;
}

/* @function: UArray2_at
 * @purpose: allows client to index value in T uarray2
 *
 * @precondition: 1) T uarray2 is valid and initialized type T
 *                2) row must be less than height of uarray2 and 
 *                   greater than 0
 *                3) col must be less than width of uarray2 and 
 *                   greater than 0
 * @postcondition: void pointer to element at position (row, col) will
 *                 be returned
 *
 * @parameters: 1) T uarray2, which is UArray which the client wants
 *                 to index
 *              2) int row, which is the row the element the client
 *                 wants is in
 *              3) int col, which is the col the element the client 
 *                 wants is in
 * @returns: void pointer to element in uarray2 at position (row, col)
 */
void *UArray2_at(T uarray2, int col, int row)
{
    assert(uarray2 != NULL);
    assert(row >= 0);
    assert(row < uarray2->MAX_ROWS);
    assert(col >= 0);
    assert(col < uarray2->MAX_COLS);

    /* since we use a single uarray, this is the formula
     * under-the-hood for translating row col pairs to an index 
     */
    int index = row * uarray2->MAX_COLS + col;
    return UArray_at(uarray2->array, index);
}

/* @function: UArray2_map_row_major
 * @purpose: map function which performs function void apply to all
 *           elements in uarray2, starting with (0, 0) and iterating
 *           row-major (first row one, then row two, etc.)
 *
 * @precondition: 1) T uarray2 is valid and initialized type T
 *                2) void apply is valid function following 
 *                   parameter specifications
 * @postcondition: function void apply will have been run on all elements,
 *                 in row-major order
 *
 * @parameters: 1) T uarray2, which is UArray whose elements are being acted
 *                 upon
 *              2) void apply(int col, int row, UArray2_T x, 
 *                 void *element, void *cl), which is function that will 
 *                 be run on all elements   
 *              3) void *cl, which is the client specifed pointer
 * @returns: none
 */
void UArray2_map_row_major(T uarray2, 
                                    void apply(int col, int row,
                                               UArray2_T x,
                                               void *element, void *cl), 
                                    void *cl)
{
    assert(uarray2 != NULL);
    assert(apply != NULL);
    assert(cl != NULL);

    for (int r = 0; r < uarray2->MAX_ROWS; r++) {
        for (int c = 0; c < uarray2->MAX_COLS; c++) {

            apply(c, r, uarray2, UArray2_at(uarray2, c, r), cl);
        }
    }
}

/* @function: UArray2_map_col_major
 * @purpose: map function which performs function void apply to all
 *           elements in uarray2, starting with (0, 0) and iterating
 *           col-major (first col one, then col two, etc.)
 *
 * @precondition: 1) T uarray2 is valid and initialized type T
 *                2) void apply is valid function following 
 *                   parameter specifications
 * @postcondition: function void apply will have been run on all elements,
 *                 in col-major order
 *
 * @parameters: 1) T uarray2, which is UArray whose elements are being acted
 *                 upon
 *              2) void apply(int col, int row, UArray2_T x, 
 *                 void *element, void *cl), which is function that will 
 *                 be run on all elements             
 *              3) void *cl, which is the client specifed pointer
 * @returns: none
 */
void UArray2_map_col_major(T uarray2, 
                                    void apply(int col, int row,
                                               UArray2_T x,
                                               void *element, void *cl), 
                                    void *cl)
{
    assert(uarray2 != NULL);
    assert(apply != NULL);
    assert(cl != NULL);

    for (int c = 0; c < uarray2->MAX_COLS; c++) {
        for (int r = 0; r < uarray2->MAX_ROWS; r++) {

            apply(c, r, uarray2, UArray2_at(uarray2, c, r), cl);
        }
    }
}


#undef T