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
    pid_t processID;
    mqd_t mq_fd_request;
    mqd_t mq_fd_response;
    MQ_REQUEST_MESSAGE req;
    MQ_RESPONSE_MESSAGE rsp;
    struct mq_attr attr; 
    
    sprintf (mq_name1, "/mq_request_%s_%d", STUDENT_NAME, getpid());
    sprintf (mq_name2, "/mq_response_%s_%d", STUDENT_NAME, getpid());
    
    attr.mq_maxmsg = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof (MQ_REQUEST_MESSAGE);
    mq_fd_request = mq_open(mq_name1, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);
    
    attr.mq_maxmsg = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof(MQ_RESPONSE_MESSAGE);
    mq_fd_response = mq_open(mq_name2, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);
    
    printf ("parent pid:%d\n", getpid());
    
    for (int i = 0; i < NROF_WORKERS; i++){
		processID = fork();
		if (processID < 0) {
			perror("fork() failed");
			exit(EXIT_FAILURE);
		} else {
			if (processID == 0) {
				//child-stuff
				execlp("./worker", mq_name1, mq_name2, NULL);
				
				// we should never arrive here...
				perror ("execlp() failed");
			} 
		}
	}
	
	
	while ((processID = waitpid(-1, NULL, 0))) {
		if (errno == ECHILD) {
			break;
			}
	}
	waitpid (-1, NULL, 0);   // wait for the child
    printf ("all children have finished");
    
    mq_close(mq_fd_response);
    mq_close(mq_fd_request);
    mq_unlink(mq_name1);
    mq_unlink(mq_name2);
    
    
    return (0);
}

