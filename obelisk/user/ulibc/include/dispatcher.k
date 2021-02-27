/*
 * $FILE: dispatcher.h
 *
 * Job dispatcher
 *
 * Author: Jordi Sánchez <jorsanp3@upvnet.upv.es>
 */

#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

#include <arch/spinlock.h>

#define DQUEUE_READY           0
#define DQUEUE_BUSY            1

#define DQUEUE_NODEPOOL_SIZE   16   // Should be configurable
#define DQUEUE_PRIORITY_LEVELS 8    // Should be configurable
#define DQUEUE_STACK_SIZE      1024 // Should be configurable

typedef struct dispatchNode {
   void (*job)(void *);
   void *arg;
   struct dispatchNode *next;
} dNode_t;

typedef struct dispatchQueue {
   struct dispatchNode *head;
   struct dispatchNode *tail;
   int status;
   spinlock_t primaryLock;
   spinlock_t secondaryLock;
   unsigned char stack[DQUEUE_STACK_SIZE];
   /* Additional information may be included
    *    Remaining jobs
    *    Resources
    *    Global WCET
    */
} dQueue_t;

#endif //_DISPATCHER_H_





