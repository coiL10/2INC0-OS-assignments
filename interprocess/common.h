/* 
 * Operating Systems [2INCO] Practical Assignment
 * Interprocess Communication
 *
 * Contains definitions which are commonly used by the farmer and the workers
 *
 */

#ifndef COMMON_H
#define COMMON_H

#include "uint128.h"


// maximum size for any message in the tests
#define MAX_MESSAGE_LENGTH	6
 

// TODO: put your definitions of the datastructures here

typedef struct
{
	char first_letter;
	uint128_t md5;
	int currentMd5;
	char start_char;
	char end_char;
} MQ_REQUEST_MESSAGE;

typedef struct
{
	char DECODED[MAX_MESSAGE_LENGTH];
	int number_found;
} MQ_RESPONSE_MESSAGE;


#endif

