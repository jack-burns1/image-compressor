/**
 ** Max Mitchell & Jack Burns
 ** uarray2.h
 ** 18 February 2020
 **
 ** Purpose: public interface for uarray2.c
 **/

#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#define T UArray2_T
typedef struct T *T;

/* @function: UArray2_new
 * @purpose: Initialize new UArray2
 *
 * @precondition: 1) width is > 0
 *                2) height is > 0
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
extern T UArray2_new(int width, int height, int size);

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
extern void UArray2_free(T *uarray2);

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
extern int UArray2_width(T uarray2);

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
extern int UArray2_height(T uarray2);

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
extern int UArray2_size(T uarray2);

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
extern void *UArray2_at(T uarray2, int row, int col);

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
extern void UArray2_map_row_major(T uarray2, 
                                    void apply(int col, int row,
                                               UArray2_T x,
                                               void *element, void *cl), 
                                    void *cl);

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
extern void UArray2_map_col_major(T uarray2, 
                                    void apply(int col, int row,
                                               UArray2_T x,
                                               void *element, void *cl), 
                                    void *cl);

#undef T
#endif /* UARRAY2_INCLUDED */
