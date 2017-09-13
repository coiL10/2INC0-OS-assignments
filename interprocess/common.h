/* 
 * Operating Systems [2INCO] Practical Assignment
 * Interprocess Communication
 *
 * Contains definitions which are commonly used by the farmer and the workers
 *
 */

#ifndef COMMON_H
#define COMMON_H


// maximum size for any message in the tests
#define MAX_MESSAGE_LENGTH	6
 

// TODO: put your definitions of the datastructures here
static char                 mq_name1[80];
static char                 mq_name2[80];



typedef struct
{
    // a data structure with 3 members
    int                     a;
    int                     b;
    int                     c;
} MQ_REQUEST_MESSAGE;

typedef struct
{
    // a data structure with 3 members, where the last member is an array with 3 elements
    int                     d;
    int                     e;
    int                     f[3];
} MQ_RESPONSE_MESSAGE;


#endif

