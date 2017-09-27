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
#include <pthread.h>



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

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void* flip(void* arg){
	int param;
	int* rtnval;
	param = *(int *)arg;
	free(arg);
	
	for(int j = param; j < NROF_PIECES; j = j + param + 1){
		pthread_mutex_lock(&mtx);
		BIT_FLIP(buffer[j/128], (j%128));
	//	printf("bit %d\n",param);
		pthread_mutex_unlock(&mtx);
	}
	rtnval = malloc(sizeof(int));
	*rtnval = 1;
	return (rtnval);
}



int main (void)
{
    // TODO: start threads to flip the pieces and output the results
    // (see thread_malloc_free_test() and thread_mutex_test() how to use threads and mutexes,
    //  see bit_test() how to manipulate bits in a large integer)
	int* parameter;
	pthread_t thread_id[NROF_THREADS+1];

	//initialize the all bits to 1(black)
   	for(int i = 0; i < (NROF_PIECES/128) + 1;i++){
		buffer[i] = ~0;
	}
	
	int active_threads = 0;
	int multiple = 1;
	int* rtnval;

	while(multiple < NROF_PIECES){
		
		//keep all the time at most 10 threads active
	    while(active_threads < NROF_THREADS && multiple < NROF_PIECES){
			parameter = malloc(sizeof(int));
			*parameter = multiple;
			pthread_create(&thread_id[active_threads],NULL,flip,parameter);
			active_threads++;
			multiple++;
		}

		for(int i = 0; i < NROF_THREADS && active_threads > 0;i++){
				pthread_join(thread_id[i],(void**)&rtnval);
				//when a thread finished the job it returns a non-zero value;
			//	printf("finished %d\n",*rtnval);
				if(*rtnval){
					active_threads--;
				}
		}
	}


	//printing of the result
	for (int i = 0; i < NROF_PIECES; i++){
		if (BIT_IS_SET(buffer[i/128], (i%128))) {
			printf("%d\n", i+1);
		}
	}
    return (0);
}

