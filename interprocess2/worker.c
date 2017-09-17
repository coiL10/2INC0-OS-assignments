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
#include <string.h>
#include "common.h"
#include "md5s.h"

static void rsleep (int t);

static char mq_f2w[80];
static char mq_w2f[80];

//count the nr of decoded md5 hash code;
static int total_send = 0;

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
    
	mqd_t mq_fd_request;
	mqd_t mq_fd_response;
	MQ_REQUEST_MESSAGE req;
	MQ_RESPONSE_MESSAGE rsp;
	struct mq_attr attrReq;
	struct mq_attr attrRsp;
	//get the name of the queues from cmd arg
	strcpy(mq_f2w,argv[1]);
	strcpy(mq_w2f, argv[2]);
	

	//open the request and response queue
	mq_fd_request = mq_open(mq_f2w,O_RDONLY);
	mq_fd_response = mq_open(mq_w2f,O_WRONLY);

	if(mq_fd_request < 0)
		perror("Worker couldn't open the request queue\n");
	if(mq_fd_response < 0)
		perror("Worker couldn't open the response queue\n");
	
	
	uint_128_t hash_to_check;
	char first_letter;
	char decoded[MAX_MESSAGE_LENGTH+1];
	while(total_send < MD5_LIST_NROF){
		
		mq_getattr(mq_fd_response,&attrRsp);
		//mq_getattr(mq_fd_request,&attrReq);
		
		int nr_rsp_msgs = attrReq.mq_curmsgs;
	//	int nr_req_msgs = attrRsp.mq_curmsgs;
		
		//RESPONSE CODE
		while(nr_rsp_msg < MQ_MAX_MESSAGES){
			printf("			child receiving...\n");
			mq_receive(mq_fd_request,(char*)&req,sizeof(req),NULL);
			hash_to_check = req.md5_hash;
			first_letter = req.first_letter;
			
			rsleep(10000);

			int ok = find_md5(hash_to_check,first_letter,&decoded);
			if(ok == 1){
				total_send++;
				strcpy(rsp.msg_decoded,decoded);
				mq_send(mq_fd_response,(char*)&rsp,sizeof(rsp),0);
			}
	
		/*just for TEST
		printf("%c\n",req.first_letter);
		printf("0x%016lx,0x%016lx\n",HI(req.md5_hash),LO(req.md5_hash));	
	    */
		}
	}

	return (0);
}

int find_md5(uint128_t hash_to_check,char first_letter,char* decoded){
	//TODO generate all combinations of MAX_MESSAGE_LENGTH starting with first_letter;

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


