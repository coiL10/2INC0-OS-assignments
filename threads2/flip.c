/* 
 * Operating Systems [2INCO] Practical Assignment
 * Threaded Application
 *
 * STUDENT_NAME_1 (STUDENT_NR_1)
 * STUDENT_NAME_2 (STUDENT_NR_2)
 *
 * Grading:
 * Students who hand in clean code that fully satisfies the minimum requirements will get an 8. 
 * ”Extra” steps can lead to higher marks because we want students to take the initiative. 
 * Extra steps can be, for example, in the form of measurements added to your code, a formal 
 * analysis of deadlock freeness etc.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>     // for usleep()
#include <time.h>       // for time()
#include "uint128.h"
#include "flip.h"




// create a bitmask where bit at position n is set
#define BITMASK(n)          (((uint128_t) 1) << (n))

// check if bit n in v is set
#define BIT_IS_SET(v,n)     (((v) & BITMASK(n)) == BITMASK(n))

// flip bit n in v
#define BIT_FLIP(v,n)        ((v) =  (v) ^  BITMASK(n))

// clear bit n in v
#define BIT_CLEAR(v,n)      ((v) =  (v) & ~BITMASK(n))

// declare a mutex, and it is initialized as well
//static pthread_mutex_t      mutex          = PTHREAD_MUTEX_INITIALIZER;

/*-------------------------------------------------------------------------*/


int main (void)
{
    // TODO: start threads to flip the pieces and output the results
    // (see thread_malloc_free_test() and thread_mutex_test() how to use threads and mutexes,
    //  see bit_test() how to manipulate bits in a large integer)
    
   	for(int i = 0; i < (NROF_PIECES/128) + 1;i++){
		buffer[i] = ~0;
	}
    for (int i = 1; i < NROF_PIECES; i++){
		for(int j = i; j < NROF_PIECES; j = j + i + 1){
			BIT_FLIP(buffer[j/128], (j%128));
		}
	}
	
	for (int i = 0; i < 100; i++){
		if (BIT_IS_SET(buffer[i/128], (i%128))) {
			printf("%d\n", i+1);
		}
	}
    return (0);
}

