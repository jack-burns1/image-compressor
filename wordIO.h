/**
 ** Max Mitchell and Jack Burns
 ** 
 ** wordPrinter.h
 ** Mar 4 2020
 ** Purpose: STUB
 **/

#ifndef WORDIO_INCLUDED
#define WORDIO_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "a2methods.h"


void writeWords(A2Methods_UArray2 array);
/* array is expected to be a plain uarray2 holding uint32_t */
A2Methods_UArray2 readWords(FILE *fp);
/* fp is expected to be a COMP40 Compressed image format 2 file,
 * and readWords returns a plain uarray2 holding uint32_t
 */

#endif /* WORDIO_INCLUDED */