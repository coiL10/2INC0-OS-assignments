/* 
 * Operating Systems [2INCO] Practical Assignment
 * Condition Variables Application
 *
 * STUDENT_NAME_1 (STUDENT_NR_1)
 * STUDENT_NAME_2 (STUDENT_NR_2)
 *
 * Grading:
 * Students who hand in clean code that fully satisfies the minimum requirements will get an 8. 
 * "Extra" steps can lead to higher marks because we want students to take the initiative. 
 * Extra steps can be, for example, in the form of measurements added to your code, a formal 
 * analysis of deadlock freeness etc.
 */
 
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "prodcons.h"

static ITEM buffer[BUFFER_SIZE]; //buffer
static int nr_elems_buffer = 0; //nr of elements in the buffer
static int current_item = 0;

typedef struct {
	pthread_t thread_id;  
    ITEM storedItem; 
} PRODUCER_STRUCT;

PRODUCER_STRUCT producers[NROF_PRODUCERS]; //to keep trace of producers running and what they are doing

//start end pointer in the circular buffer
int head_pointer = 0;
int tail_pointer = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cons_cond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t prod_cond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t prod_cond_order[NROF_PRODUCERS];


static void rsleep (int t);			// already implemented (see below)
static ITEM get_next_item (void);	// already implemented (see below)


/* producer thread */
static void * 
producer (void * arg)
{
	int id = *(int*)arg; //id of the producer
	free(arg);
	ITEM item;
    while (item != NROF_ITEMS)
    {
        // TODO: 
        // * get the new item
        
        item = get_next_item();
        if (item == NROF_ITEMS){
			return NULL;
		}
		
		producers[id].storedItem = item;
		
        rsleep (100);	// simulating all kind of activities...
		
		// TODO:
		// * put the item into buffer[]
		//
        // follow this pseudocode (according to the ConditionSynchronization lecture):
        //      mutex-lock;
        //      while not condition-for-this-producer
        //          wait-cv;
        //      critical-section;
        //      possible-cv-signals;
        //      mutex-unlock;
        //
        // (see condition_test() in condition_basics.c how to use condition variables)
        
        pthread_mutex_lock(&mutex);
        
        while(item != current_item){
			pthread_cond_wait(&prod_cond_order[id], &mutex);
		}

        while (nr_elems_buffer >= BUFFER_SIZE) {
			pthread_cond_wait(&prod_cond, &mutex);
		}
		
		buffer[tail_pointer] = item;
		tail_pointer = (tail_pointer + 1) % BUFFER_SIZE;
        nr_elems_buffer++;
        current_item++;
        
        if(nr_elems_buffer == 1){   //buffer has something to be consumed
			pthread_cond_signal(&cons_cond);
		}
		
		for(int i = 0; i < NROF_PRODUCERS; i++){
			if (producers[i].storedItem == current_item){
				pthread_cond_signal(&prod_cond_order[i]);
			}
		}
		
		
		pthread_mutex_unlock(&mutex);
		
    }

	return (NULL);
}

/* consumer thread */
static void * 
consumer (void * arg)
{
	int consumed_items = 0;
	ITEM item;
    while (consumed_items < NROF_ITEMS)
    {
        // TODO: 
		// * get the next item from buffer[]
		// * print the number to stdout
        //
        // follow this pseudocode (according to the ConditionSynchronization lecture):
        //      mutex-lock;
        //      while not condition-for-this-consumer
        //          wait-cv;
        //      critical-section;
        //      possible-cv-signals;
        //      mutex-unlock;
		
        rsleep (100);		// simulating all kind of activities...
        
         pthread_mutex_lock(&mutex);
         
         while(nr_elems_buffer <= 0) {
			 pthread_cond_wait(&cons_cond, &mutex);
		 }
		 
		 item = buffer[head_pointer];
		 consumed_items++;
		 head_pointer = (head_pointer+1) % BUFFER_SIZE;
		 nr_elems_buffer--;
		 if (nr_elems_buffer == BUFFER_SIZE - 1) {
			 pthread_cond_signal(&prod_cond);
		 }
		 
		 if (consumed_items >= NROF_ITEMS){
			 pthread_cond_broadcast(&prod_cond);
		 }
		 
		 pthread_mutex_unlock(&mutex);
		 
		 printf("%d\n",item);
    }
	return (NULL);
}

int main (void)
{
    // TODO: 
    // * startup the producer threads and the consumer thread
    // * wait until all threads are finished  
	pthread_t producer_thread[NROF_PRODUCERS];
	pthread_t consumer_thread;

	//start the producers
   	for(int i = 0; i < NROF_PRODUCERS;i++){
		pthread_cond_init(&prod_cond_order[i], NULL);
		int* nr_thread = malloc(sizeof(int));
		*nr_thread = i;
		pthread_create(&producer_thread[i],NULL,producer,(void*)nr_thread);
		producers[i].thread_id = i;
	}

	//start consumer
	pthread_create(&consumer_thread,NULL,consumer,NULL);

	//wait for producers
	for(int i = 0;i < NROF_PRODUCERS;i++){
		pthread_join(producer_thread[i],NULL);
	}
	
	//wait for consumer
	pthread_join(consumer_thread,NULL);
	
    return (0);
}

/*
 * rsleep(int t)
 *
 * The calling thread will be suspended for a random amount of time between 0 and t microseconds
 * At the first call, the random generator is seeded with the current time
 */
static void 
rsleep (int t)
{
    static bool first_call = true;
    
    if (first_call == true)
    {
        srandom (time(NULL));
        first_call = false;
    }
    usleep (random () % t);
}


/* 
 * get_next_item()
 *
 * description:
 *		thread-safe function to get a next job to be executed
 *		subsequent calls of get_next_item() yields the values 0..NROF_ITEMS-1 
 *		in arbitrary order 
 *		return value NROF_ITEMS indicates that all jobs have already been given
 * 
 * parameters:
 *		none
 *
 * return value:
 *		0..NROF_ITEMS-1: job number to be executed
 *		NROF_ITEMS:		 ready
 */
static ITEM
get_next_item(void)
{
    static pthread_mutex_t	job_mutex	= PTHREAD_MUTEX_INITIALIZER;
	static bool 			jobs[NROF_ITEMS+1] = { false };	// keep track of issued jobs
	static int              counter = 0;    // seq.nr. of job to be handled
    ITEM 					found;          // item to be returned
	
	/* avoid deadlock: when all producers are busy but none has the next expected item for the consumer 
	 * so requirement for get_next_item: when giving the (i+n)'th item, make sure that item (i) is going to be handled (with n=nrof-producers)
	 */
	pthread_mutex_lock (&job_mutex);

    counter++;
	if (counter > NROF_ITEMS)
	{
	    // we're ready
	    found = NROF_ITEMS;
	}
	else
	{
	    if (counter < NROF_PRODUCERS)
	    {
	        // for the first n-1 items: any job can be given
	        // e.g. "random() % NROF_ITEMS", but here we bias the lower items
	        found = (random() % (2*NROF_PRODUCERS)) % NROF_ITEMS;
	    }
	    else
	    {
	        // deadlock-avoidance: item 'counter - NROF_PRODUCERS' must be given now
	        found = counter - NROF_PRODUCERS;
	        if (jobs[found] == true)
	        {
	            // already handled, find a random one, with a bias for lower items
	            found = (counter + (random() % NROF_PRODUCERS)) % NROF_ITEMS;
	        }    
	    }
	    
	    // check if 'found' is really an unhandled item; 
	    // if not: find another one
	    if (jobs[found] == true)
	    {
	        // already handled, do linear search for the oldest
	        found = 0;
	        while (jobs[found] == true)
            {
                found++;
            }
	    }
	}
    jobs[found] = true;
			
	pthread_mutex_unlock (&job_mutex);
	return (found);
}


