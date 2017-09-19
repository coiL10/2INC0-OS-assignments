/* 
 * Operating Systems [2INCO] Practical Assignment
 * Interprocess Communication
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
#include <string.h>
#include <errno.h>          // for perror()
#include <unistd.h>         // for getpid()
#include <mqueue.h>         // for mq-stuff
#include <time.h>           // for time()
#include <complex.h>

#include "common.h"
#include "md5s.h"

static void rsleep (int t);

int inc(char * c, char start, char end);


int main (int argc, char * argv[])
{
    // TODO:
    // (see message_queue_test() in interprocess_basic.c)
    //  * open the two message queues (whose names are provided in the arguments)
    //  * repeatingly:
    //      - read from a message queue the new job to do
    //      - wait a random amount of time (e.g. rsleep(10000);)
    //      - do that job 
    //      - write the results to a message queue
    //    until there are no more tasks to do
    //  * close the message queues
    
    //printf("process %d started !\n", getpid());
    
    //declarations
    mqd_t mq_fd_request;
    mqd_t mq_fd_response;
    MQ_REQUEST_MESSAGE req;
    MQ_RESPONSE_MESSAGE rsp;
    struct mq_attr attrRQ;
    int finished = 0;
    int boolFound = 0;
    
    //open the two messages queue
    mq_fd_request = mq_open(argv[1], O_RDONLY);
    mq_fd_response = mq_open(argv[2], O_WRONLY);
    
    
    do {             //while the queue isn't empty for at least 10 tries
		mq_getattr(mq_fd_request, &attrRQ);
		
		// read the message queue and store it in the request message
		//printf ("                                   child %d: receiving...\n", getpid());
		mq_receive (mq_fd_request, (char *) &req, sizeof (req), NULL);
		
		finished = req.finished;
		
		if (finished == 1) {
			//printf ("                                   child %d: bye bye!\n", getpid());
		} else {		
			//printf("                                   child %d: received letter %c and md5 no %d\n", getpid(), req.first_letter, req.currentMd5);

            rsleep(10000);
            
            //DO JOB
            //string allocation
            char first_letter[1];
            char* testString = calloc(MAX_MESSAGE_LENGTH, sizeof(char)); 
            char* concatenedString = calloc(MAX_MESSAGE_LENGTH+1, sizeof(char));
            
            first_letter[0] = req.first_letter;
            
            
            if (md5s(first_letter, 1) == req.md5){ //test if first letter only is hash
				boolFound = 1;
				strcpy(rsp.DECODED, first_letter);
				rsp.number_found = req.currentMd5;
			} else {
				int i = 1;
				while (i < MAX_MESSAGE_LENGTH && boolFound == 0){ 
					int j = 0;
					while (j < i) {
						testString[j] = req.start_char;
						j++;
					}
					testString[i] = 0;
					do {
						strcpy(concatenedString, first_letter);
						strcat(concatenedString, testString);
						if (md5s(concatenedString, i+1) == req.md5) {
							boolFound = 1;
							strcpy(rsp.DECODED, concatenedString);
							rsp.number_found = req.currentMd5;
						}
					} while (inc(testString, req.start_char, req.end_char) && boolFound == 0);
					i++;
				}
			}
            
            // send the response if found
            if (boolFound == 1) {
				//printf ("                                   child %d: sending...\n", getpid());
				mq_send (mq_fd_response, (char *) &rsp, sizeof (rsp), 0);
				boolFound = 0;
			}
			free(testString);
			free(concatenedString);
		}
		
	} while (finished == 0);
		
    
    //close messages queues
    mq_close (mq_fd_response);
    mq_close (mq_fd_request);
    
    return (0);
}

/*
 * rsleep(int t)
 *
 * The calling thread will be suspended for a random amount of time
 * between 0 and t microseconds
 * At the first call, the random generator is seeded with the current time
 */
static void rsleep (int t)
{
    static bool first_call = true;
    
    if (first_call == true)
    {
        srandom (time (NULL) % getpid ());
        first_call = false;
    }
    usleep (random() % t);
}

/*
 * inc(char *c, char start, char end)
 *
 * Helper function for the string generation
 */
int inc(char *c, char start, char end){
    if(c[0]==0) return 0;
    if(c[0]==end){
        c[0]=start;
        return inc(c+sizeof(char), start, end);
    }   
    c[0]++;
    return 1;
}



