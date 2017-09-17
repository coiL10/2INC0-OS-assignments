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

static char mq_f2w[80];
static char mq_w2f[80];

int workers[NROF_WORKERS];

void create_workers();

int main (int argc, char * argv[])
{
    if (argc != 1)
    {
        fprintf (stderr, "%s: invalid arguments\n", argv[0]);
    }
  //  pid_t processID;
	mqd_t mq_fd_request;
	mqd_t mq_fd_response;
	MQ_REQUEST_MESSAGE req;
	MQ_RESPONSE_MESSAGE rsp;
	struct mq_attr attrReq;
	struct mq_attr attrRsp;

	// set the message queue names
	sprintf(mq_f2w,"/mq_request_%s_%d","ANDREI_BORA",getpid());
	sprintf(mq_w2f,"/mq_request:_%s_%d","ANDREI_BORA",getpid());
	
	//open request queue
	attrReq.mq_maxmsg = MQ_MAX_MESSAGES;
	attrReq.mq_msgsize = sizeof(MQ_REQUEST_MESSAGE);
	mq_fd_request = mq_open(mq_f2w,O_WRONLY|O_CREAT|O_EXCL,0600,&attrReq);

	//open response queue
	attrRsp.mq_maxmsg = MQ_MAX_MESSAGES;
	attrRsp.mq_msgsize = sizeof(MQ_RESPONSE_MESSAGE);
	mq_fd_response = mq_open(mq_w2f,O_RDONLY|O_CREAT|O_EXCL,0600,&attrRsp);

	//do some sanity check

	if(mq_fd_request < 0){
		perror("Farmer couldn't open the message queue\n");
		exit(1);
	}

	if(mq_fd_request < 0){
		perror("Worker couldn't open the message queue\n");
		exit(1);
	}
	//create workers

	create_workers();
	
	int total_received = 0;
	//nr of current messages in the queues
	int nr_rsp_msgs;
	int nr_req_msgs;

//	int total_send = 0;
	while(total_received < MD5_LIST_NROF){
		mq_getattr(mq_fd_request,&attrReq);
		mq_getattr(mq_fd_response,&attrRsp);
		
		nr_rsp_msgs = attrRsp.mq_curmsgs;
		nr_req_msgs = attrReq.mq_curmsgs;

		//REQUEST CODE
		while(nr_req_msgs < MQ_MAX_MESSAGES ){
			//create request
			req.md5_hash = md5_list[total_received];
			for(char f_letter = ALPHABET_START_CHAR;f_letter <= ALPHABET_END_CHAR && nr_req_msgs < MQ_MAX_MESSAGES;f_letter++){ 
				req.first_letter = f_letter;
				//send request
				//int code =.. 
				mq_send(mq_fd_request,(char*)&req,sizeof(req),0);
				nr_req_msgs = attrReq.mq_curmsgs;
				
				//?? We don't need wait because mq_send is blocking???;
			}
			
		}

		//RESPONSE CODE

		while(nr_rsp_msgs < MQ_MAX_MESSAGES){
			int nr_bytes = mq_receive(mq_fd_response,(char*)&rsp,sizeof(rsp),0);
			
			if(nr_bytes == -1){
				perror("Request message failed");
			}else{
				printf("%s\n",rsp.msg_decoded);
				total_received++;
			}

			/*
			nr_rsp_msgs = attrRsp.mq_curmsgs;
			//
			if(nr_rsp_msgs == MQ_MAX_MESSAGES){
				wait(NULL);
			}
			*/
			
		}
	}




	
    // TODO:
    //  * create the message queues (see message_queue_test() in interprocess_basic.c)
    //  * create the child processes (see process_test() and message_queue_test())
    //  * do the farming
    //  * wait until the chilren have been stopped (see process_test())
    //  * clean up the message queues (see message_queue_test())

    // Important notice: make sure that the names of the message queues contain your
    // student name and the process id (to ensure uniqueness during testing)
    
    return (0);
}


void create_workers(){
	for(int i = 0; i < NROF_WORKERS;i++){
		pid_t worker_id;
		worker_id = fork();
		if(worker_id < 0){
			perror("Fork failed\n");
			exit(1);
		}else if(worker_id == 0){
			execlp("./worker","worker",mq_f2w,mq_w2f,NULL);
			perror("execlp failed");
		}else if(worker_id > 0){
			workers[i] = worker_id;
		}

	}
	//int status;
	//for(int i = 0; i < NROF_WORKERS;i++){
	//	wait(&status);
	//}
}

