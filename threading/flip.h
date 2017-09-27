/* 
 * Operating Systems [2INC0] Practical Assignment
 * Threaded application
 *
 * Joris Geurts
 * j.h.j.geurts@tue.nl
 *
 */

#include "uint128.h"
 
/**
 * NROF_PIECES: size of the board; number of pieces to be flipped
 */
#define NROF_PIECES			3000

/**
 * NROF_THREADS: number of threads that can be run in parallel
 * (value must be between 1 and ... (until you run out of system resources))
 */
#define NROF_THREADS        10

/**
 * buffer[]: datastructure of the pieces; each piece is represented by one bit
 */
static uint128_t			buffer [(NROF_PIECES/128) + 1];
