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
    printf("%d\n", getpid());
    
    mqd_t mq_fd_request;
    mqd_t mq_fd_response;
    MQ_REQUEST_MESSAGE req;
    MQ_RESPONSE_MESSAGE rsp;
    
    //open the two messages queue
    mq_fd_request = mq_open(argv[0], O_RDONLY);
    mq_fd_response = mq_open(argv[1], O_WRONLY);
    
    // read the message queue and store it in the request message
    printf ("                                   child: receiving...\n");
    mq_receive (mq_fd_request, (char *) &req, sizeof (req), NULL);

    printf ("                                   child: received: %d, %d, %d\n",
            req.a, req.b, req.c);
            
    rsleep(10000);
    
    //do job
    
    
    
    
    // send the response
    printf ("                                   child: sending...\n");
    mq_send (mq_fd_response, (char *) &rsp, sizeof (rsp), 0);
    
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


