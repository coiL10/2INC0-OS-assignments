/* 
 * Operating Systems [2INCO] Practical Assignment
 * Threaded Application
 *
 * Thanh Loic Nguyen (1271989)
 * Andrei Bora (1279165)
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

typedef struct{
	pthread_t thread_id;
	bool finished;
	int multiple;
} thread_struct;

pthread_mutex_t mtx[(NROF_PIECES/128) + 1];
thread_struct thread[NROF_THREADS+1];

void* flip(void* arg){
	int nr_active_thread;
	nr_active_thread = *(int *)arg;
	free(arg);
	int multiple = thread[nr_active_thread].multiple;

	for(int j = multiple; j < NROF_PIECES; j = j + multiple + 1){
		pthread_mutex_lock(&mtx[multiple/128]);
		BIT_FLIP(buffer[j/128], (j%128));
		pthread_mutex_unlock(&mtx[multiple/128]);
	}
	
	thread[nr_active_thread].finished = true;

	return 0;
}



int main (void)
{

	int* parameter;

	//initialize the mutexes

	for(int i = 0;i < NROF_PIECES/128 + 1;i++){
		pthread_mutex_init(&mtx[i],NULL);
	}

	//initialize the all bits to 1(black)
   	for(int i = 0; i < (NROF_PIECES/128) + 1;i++){
		buffer[i] = ~0;
	}

	int active_threads = 0;
	int multiple = 1;

	
	while(multiple < NROF_PIECES){
		
		//keep all the time at most 10 threads active
	    while(active_threads < NROF_THREADS && multiple < NROF_PIECES){
			parameter = malloc(sizeof(int));
			*parameter = active_threads;
			thread[active_threads].multiple = multiple;
			thread[active_threads].finished = false;
			pthread_create(&(thread[active_threads].thread_id),NULL,flip,parameter);
			active_threads++;
			multiple++;
		}

		for(int i = NROF_THREADS-1; i >= 0 && active_threads > 0;i--){
				if(thread[i].finished){
					pthread_join(thread[i].thread_id,NULL);
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

