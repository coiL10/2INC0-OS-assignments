/* 
 * Operating Systems [2INCO] Practical Assignment
 * Threaded Application
 *
 * Thanh Loïc Nguyen (1271989)
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
#include <pthread.h>

#include "uint128.h"
#include "flip.h"

typedef struct {
    pthread_t thread_id;  
    int parameter; 
    int IsFinished;
    int index; //index of array when put in thread_collection
    int slotIsUsed; //is the slot used (running or waiting to be joined)
} THREAD_STRUCT;


// create a bitmask where bit at position n is set
#define BITMASK(n)          (((uint128_t) 1) << (n))

// check if bit n in v is set
#define BIT_IS_SET(v,n)     (((v) & BITMASK(n)) == BITMASK(n))

// flip bit n in v
#define BIT_FLIP(v,n)        ((v) =  (v) ^ BITMASK(n))

// declare an array of mutexes
static pthread_mutex_t      mutex[(NROF_PIECES/128)+1]; //one mutex per integer
static pthread_mutex_t      mutexThread;

THREAD_STRUCT threads[NROF_THREADS]; //to keep trace of threads running and what they are doing



/*-------------------------------------------------------------------------*/

void* flip(void* arg);

int main (void)
{
    // TODO: start threads to flip the pieces and output the results
    // (see thread_malloc_free_test() and thread_mutex_test() how to use threads and mutexes,
    //  see bit_test() how to manipulate bits in a large integer)
    pthread_mutex_init(&mutexThread, NULL);
    
    for(int i = 0; i < ((NROF_PIECES/128)+1); i++){
		pthread_mutex_init(&mutex[i], NULL); //initialize mutexes
		buffer[i] = ~0; //set all bits of buffer to 1
	}
	
	for(int i = 0; i < NROF_THREADS; i++){
		threads[i].slotIsUsed = 0; //set that no threads is used
	}
	
	int active_threads = 0;
	int multiple = 2;
	
	while(multiple <= NROF_PIECES && active_threads < NROF_THREADS){ //start NROF_TREADS firsts threads
		threads[active_threads].parameter = multiple;
		threads[active_threads].index = active_threads;
		threads[active_threads].IsFinished = 0;
		threads[active_threads].slotIsUsed = 1;
		pthread_create(&threads[active_threads].thread_id,NULL,flip,&threads[active_threads].index); //create thread
		active_threads++;
		if (active_threads < NROF_THREADS){
			multiple++;
		}
	}
	
	while(multiple <= NROF_PIECES){  //looks like busy waiting, to check
		for(int i = 0; i < NROF_THREADS; i++) { //check if a thread has finished
			if(threads[i].IsFinished == 1){
				pthread_join(threads[i].thread_id, NULL);
				threads[i].slotIsUsed = 0;
				multiple++;
				if(multiple <= NROF_PIECES){
					threads[i].parameter = multiple;
					threads[i].IsFinished = 0;
					threads[i].slotIsUsed = 1;
					pthread_create(&threads[i].thread_id,NULL,flip,&threads[i].index);
				}
			}
		}
	}
	for (int i = 0; i < NROF_THREADS; i++) { //join (and wait for) all the threads that are used
		if (threads[i].slotIsUsed == 1){
			pthread_join (threads[i].thread_id, NULL);
		}
	}
	
	
	//print all 1 in the buffer
	for (int i = 0; i < NROF_PIECES; i++){
		if (BIT_IS_SET(buffer[i/128], (i%128))) {
			printf("%d\n", i);
		}
	}
    return (0);
}

void* flip(void* arg){
	
	int index = *(int*)arg;
	int param = threads[index].parameter;
	printf("%d\n", param);
	
	for(int i = param; i < NROF_PIECES; i = i + param){
		pthread_mutex_lock(&mutex[i/128]);
		BIT_FLIP(buffer[i/128], (i%128));
		pthread_mutex_unlock(&mutex[i/128]);
	}
	
	threads[index].IsFinished = 1;
	
	return 0;
	
	
}

