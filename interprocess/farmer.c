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
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>    
#include <unistd.h>         // for execlp
#include <mqueue.h>         // for mq

#include "settings.h"
#include "common.h"

#define STUDENT_NAME "T.L. Nguyen"

static char                 mq_name1[80];
static char                 mq_name2[80];


int main (int argc, char * argv[])
{
    if (argc != 1)
    {
        fprintf (stderr, "%s: invalid arguments\n", argv[0]);
    } 
    // TODO:
    //  * create the message queues (see message_queue_test() in interprocess_basic.c)
    //  * create the child processes (see process_test() and message_queue_test())
    //  * do the farming
    //  * wait until the chilren have been stopped (see process_test())
    //  * clean up the message queues (see message_queue_test())

    // Important notice: make sure that the names of the message queues contain your
    // student name and the process id (to ensure uniqueness during testing)
    
    
    //declare stuff
    char * items[MD5_LIST_NROF]; //array to keep found values
    for (int i = 0; i < MD5_LIST_NROF; i++){ //allocate
		items[i] = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
	}
    pid_t processID;
    mqd_t mq_fd_request;
    mqd_t mq_fd_response;
    MQ_REQUEST_MESSAGE req;
    MQ_RESPONSE_MESSAGE rsp;
    struct mq_attr attrRQ;
    struct mq_attr attrRP;  
    int found = 0; //found md5
    int current = 0; //current md5 jobs being sent
    int alphabet_count = 0; //current first letter
    
    //creating messages queues
    sprintf (mq_name1, "/mq_request_%s_%d", STUDENT_NAME, getpid());
    sprintf (mq_name2, "/mq_response_%s_%d", STUDENT_NAME, getpid());
    
    attrRQ.mq_maxmsg = MQ_MAX_MESSAGES;
    attrRQ.mq_msgsize = sizeof (MQ_REQUEST_MESSAGE);
    mq_fd_request = mq_open(mq_name1, O_WRONLY | O_CREAT | O_EXCL, 0600, &attrRQ);
    
    attrRP.mq_maxmsg = MQ_MAX_MESSAGES;
    attrRP.mq_msgsize = sizeof(MQ_RESPONSE_MESSAGE);
    mq_fd_response = mq_open(mq_name2, O_RDONLY | O_CREAT | O_EXCL, 0600, &attrRP);
    
    //printf ("parent pid:%d\n", getpid()); //printing parent id
    
    //creating child processes
    for (int i = 0; i < NROF_WORKERS; i++){
		processID = fork();
		if (processID < 0) {
			perror("fork() failed");
			exit(EXIT_FAILURE);
		} else {
			if (processID == 0) {
				//execute child
				execlp("./worker", "worker", mq_name1, mq_name2, NULL);
				
				// we should never arrive here...
				perror ("execlp() failed");
			} 
		}
	}
	
	while (found < MD5_LIST_NROF) { //while we havent found every md5
		
		//get attributes of queues
		mq_getattr(mq_fd_request, &attrRQ);
		mq_getattr(mq_fd_response, &attrRP);
		int nb_req = attrRQ.mq_curmsgs;
		int nb_rsp = attrRP.mq_curmsgs;
		
		//fill request messages
		if (nb_req < MQ_MAX_MESSAGES && alphabet_count < ALPHABET_NROF_CHAR && current < MD5_LIST_NROF) {
			req.first_letter = ALPHABET_START_CHAR + alphabet_count;
			req.md5 = md5_list[current];
			req.currentMd5 = current;
			req.start_char = ALPHABET_START_CHAR;
			req.end_char = ALPHABET_END_CHAR;
			req.finished = 0;
			
			//printf ("parent: sending...\n");
			mq_send (mq_fd_request, (char *) &req, sizeof (req), 0);
			
			alphabet_count++;
			if (alphabet_count > ALPHABET_NROF_CHAR){ //if we arrive at the end of alphabet, send jobs for next md5
				current++;
				alphabet_count = 0;
			}
			
		}
		
		
		// read the result and store it in the response message
		
		if (nb_rsp > 0) {
			//printf ("parent: receiving...\n");
			mq_receive(mq_fd_response, (char *) &rsp, sizeof (rsp), NULL);
			//printf ("parent: found MD5 number %d\n", rsp.number_found);
			strcpy(items[rsp.number_found], rsp.DECODED);
			found++;
			current++;
			alphabet_count = 0;
		}	
	}
	
	//order workers to close
	req.finished = 1;
	for (int i = 0; i < NROF_WORKERS; i++){
		//printf ("parent: order closing...\n");
		mq_send (mq_fd_request, (char *) &req, sizeof (req), 0); //it waits if the queue is full
	}
	
	
	//wait until the children have stopped
	while ((processID = waitpid(-1, NULL, 0))) { //not busy waiting because it waitpid waits
		if (errno == ECHILD) {
			break;
			}
	}
    //printf ("all children have finished\n");
    
    //printing found items
    for (int i = 0; i < MD5_LIST_NROF; i++){
		printf("'%s'\n", items[i]);
		free(items[i]); //free the memory
	}
    
    //close messages queues
    mq_close(mq_fd_response);
    mq_close(mq_fd_request);
    mq_unlink(mq_name1);
    mq_unlink(mq_name2);
    
    
    
    return (0);
}

