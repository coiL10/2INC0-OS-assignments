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
    int parameter; //multiple
    int IsFinished; //boolean to know if the thread has finished computatios
    int index; //index of the thread
    int threadIsUsed; //to know if the thread is being used
} THREAD_STRUCT;


// create a bitmask where bit at position n is set
#define BITMASK(n)          (((uint128_t) 1) << (n))

// check if bit n in v is set
#define BIT_IS_SET(v,n)     (((v) & BITMASK(n)) == BITMASK(n))

// flip bit n in v
#define BIT_FLIP(v,n)        ((v) =  (v) ^ BITMASK(n))

// declare an array of mutexes
static pthread_mutex_t      mutex[(NROF_PIECES/128)+1]; //one mutex per integer

THREAD_STRUCT threads[NROF_THREADS]; //to keep trace of threads running and what they are doing

static int active_threads = 0;

static pthread_mutex_t condVarMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


/*-------------------------------------------------------------------------*/

void* flip(void* arg);

int main (void)
{
    for(int i = 0; i < ((NROF_PIECES/128)+1); i++){
		pthread_mutex_init(&mutex[i], NULL); //initialize mutexes
		buffer[i] = ~0; //set all bits of buffer to 1
	}
	
	
	for(int i = 0; i < NROF_THREADS; i++){
		threads[i].threadIsUsed = 0; //set that no threads is used
	}
	
	int i = 0;
	int multiple = 2;

	while(multiple <= NROF_PIECES && i < NROF_THREADS){ //start NROF_TREADS firsts threads
		threads[i].parameter = multiple;
		threads[i].index = i;
		threads[i].IsFinished = 0;
		threads[i].threadIsUsed = 1;
		pthread_create(&threads[i].thread_id,NULL,flip,&threads[i].index); //create thread
		pthread_mutex_lock(&condVarMutex);
		active_threads++;
		pthread_mutex_unlock(&condVarMutex);
		i++;
		if (i < NROF_THREADS){
			multiple++;
		}
	}
	
	while(multiple <= NROF_PIECES){ 
		
		if (NROF_THREADS == 1) {
			pthread_join(threads[0].thread_id, NULL);
			threads[0].threadIsUsed = 0;
			threads[0].IsFinished = 0;
			multiple++;
			if(multiple <= NROF_PIECES){ //if there is still multiples to flip, create thread
				threads[0].parameter = multiple;
				threads[0].threadIsUsed = 1;
				pthread_mutex_lock(&condVarMutex);
				active_threads++;
				pthread_mutex_unlock(&condVarMutex);
				pthread_create(&threads[0].thread_id,NULL,flip,&threads[0].index);
			}
		} else {
			pthread_mutex_lock(&condVarMutex);
			while (active_threads > NROF_THREADS-1) {
				pthread_cond_wait(&cond, &condVarMutex);
			}
			pthread_mutex_unlock(&condVarMutex);
			
			for(int i = NROF_THREADS; i > 0; i--) { //check if a thread has finished
				if(threads[i].IsFinished == 1){
					pthread_join(threads[i].thread_id, NULL);
					threads[i].threadIsUsed = 0;
					threads[i].IsFinished = 0;
					multiple++;
					if(multiple <= NROF_PIECES){ //if there is still multiples to flip, create thread
						threads[i].parameter = multiple;
						threads[i].threadIsUsed = 1;
						pthread_mutex_lock(&condVarMutex);
						active_threads++;
						pthread_mutex_unlock(&condVarMutex);
						pthread_create(&threads[i].thread_id,NULL,flip,&threads[i].index);
					}
				}
			}
		}
	}
	for (int i = 0; i < NROF_THREADS; i++) { //join (and wait for) all the threads that are being used
		if (threads[i].threadIsUsed == 1){
			pthread_join (threads[i].thread_id, NULL);
		}
	}
	
	
	//print all 1 in the buffer
	for (int i = 1; i < NROF_PIECES; i++){
		if (BIT_IS_SET(buffer[i/128], (i%128))) {
			printf("%d\n", i);
		}
	}
    return (0);
}

void* flip(void* arg){ //function that the threads will run
	
	int index = *(int*)arg;
	int param = threads[index].parameter;
		
	for(int i = param; i < NROF_PIECES; i = i + param){
		pthread_mutex_lock(&mutex[i/128]); //entering CS
		BIT_FLIP(buffer[i/128], (i%128));
		pthread_mutex_unlock(&mutex[i/128]); //exiting CS
	}
	
	threads[index].IsFinished = 1;
	
	pthread_mutex_lock(&condVarMutex);
	active_threads--;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&condVarMutex);
	
	return 0;
	
	
}

