/*
 * $FILE: dispatcher.c
 *
 * Job dispatcher
 *
 * Author: Jordi Sánchez <jorsanp3@upvnet.upv.es>
 */

#include <bitmap.h>
#include <dispatcher.h>
#include <string.h>

/*
// Multiprocessor systems
struct localSched {
   dQueue_t queue[DQUEUE_PRIORITY_LEVELS];// Queue array
   bitmap_t priobmap;                     // Indexer bitmap
   int cPrio;                             // Current priority queue
};
struct localSched localSched[NR_CPUS];
*/

static int fIndex;                               // Free node index
static dNode_t nodePool[DQUEUE_NODEPOOL_SIZE];   // Node pool
static dNode_t *fNodeList;                       // List of free nodes
static dQueue_t systemQ[DQUEUE_PRIORITY_LEVELS]; // Queue array
static bitmap_t priobmap;                        // Indexer bitmap
static int maxPrio, currPrio;                    // Maximum and current priority queue

static inline dNode_t *AllocNode(void)
{
   dNode_t *dNode;

   if (fNodeList == NULL) {
      if (fIndex > DQUEUE_NODEPOOL_SIZE) {
         return NULL;
      }
      return &nodePool[fIndex++];
   }

   dNode = fNodeList;
   fNodeList = fNodeList->next;

   return dNode;
}

static inline void DQueueInsert(dQueue_t *dq, dNode_t *n)
{
   if (dq->tail)
      dq->tail->next = n;

   dq->tail = n;

   if (!dq->head)
      dq->head = n;
}

void AddJob(void (*job)(void *), void *arg, unsigned int prio)
{
   dNode_t *node;

   __hw_cli(); // Disable interrupts

   if (prio > DQUEUE_PRIORITY_LEVELS) {
      return;
   }

   if (!(node=AllocNode())) {
      // Check if there are no nodes left -> Error, reached limit of jobs
      return;
   }

   node->job = job;
   node->arg = arg;
   node->next = NULL;

   DQueueInsert(&systemQ[prio], node);
   priobmap = setbit(priobmap, prio);
   if (prio > maxPrio) {
      maxPrio = prio;
   }

   __hw_sti(); // Enable interrupts
}

// Non preemptive dispatcher
void Dispatcher(void)
{
   dNode_t *node;
   dQueue_t *currQ;

   while (1) {
      __hw_cli(); // Disable interrupts

      maxPrio = _ffs(priobmap);

      if (maxPrio > DQUEUE_PRIORITY_LEVELS) {
         maxPrio = -1;
         idle();
      } else if (maxPrio < 0) {
         idle();
      }

      currPrio = maxPrio;
      currQ = &systemQ[currPrio];
      while (currPrio == maxPrio) {
         __hw_sti(); // Enable interrupts

         // Fetch next job
         node = currQ->head;
         node->job();

         __hw_cli(); // Disable interrupts

         // Dispatch Node is released
         node->next = fNodeList;
         fNodeList = node;

         // Next job
         currQ->head = currQ->head->next;
         if (currQ->head == NULL) {
            priobmap = clearbit(priobmap, cPrio);
            currQ->tail = NULL;
            break;
         }
      }
      __hw_sti(); // Enable interrupts
   }
}
/*
// Process jobs of a given queue
void DQueueProcessor(dQueue_t *dQueue)
{
   dNode_t *next, *node;

   //__hw_sti(); // Enable interrupts

   while (1) {
      // Fetch next job
      node = dQueue->head;
      node->job();

      __hw_cli(); // Disable interrupts

      // Dispatch Node is released
      node->next = fNodeList;
      fNodeList = node;

      // Next job
      dQueue->head = dQueue->head->next;
      if (dQueue->head == NULL) {
         priobmap = clearbit(priobmap, cPrio);
         dQueue->tail = NULL;
         __hw_sti(); // Enable interrupts
         return;
      }
      __hw_sti(); // Enable interrupts
   }
}
*/

void InitDispatcher(void)
{
   memset(systemQ, 0, sizeof(systemQ));
   fIndex = 0;
   fNodeList = NULL;
}

#if 0 // Preemptive dispatcher

#include <bitmap.h>
#include <dispatcher.h>
#include <string.h>

/*
// Multiprocessor systems
struct localSched {
   dQueue_t queue[DQUEUE_PRIORITY_LEVELS];// Queue array
   bitmap_t priobmap;                     // Indexer bitmap
   int cPrio;                             // Current priority queue
};
struct localSched localSched[NR_CPUS];
*/

static int fIndex;                               // Free node index
static dNode_t nodePool[DQUEUE_NODEPOOL_SIZE];   // Node pool
static dNode_t *fNodeList;                       // List of free nodes
static dQueue_t *currentQ;                       // Current queue
static dQueue_t systemQ[DQUEUE_PRIORITY_LEVELS]; // Queue array
static bitmap_t priobmap;                        // Indexer bitmap
static int cPrio;                                // Current priority queue

static inline dNode_t *AllocNode(void)
{
   dNode_t *dNode;

   if (fNodeList == NULL) {
      if (fIndex > DQUEUE_NODEPOOL_SIZE) {
         return NULL;
      }
      return &nodePool[fIndex++];
   }

   dNode = fNodeList;
   fNodeList = fNodeList->next;

   return dNode;
}

static inline void DQueueInsert(dQueue_t *dq, dNode_t *n)
{
   if (dq->tail)
      dq->tail->next = n;

   dq->tail = n;

   if (!dq->head)
      dq->head = n;
}

void AddJob(void (*job)(void *), void *arg, unsigned int prio)
{
   dNode_t *node;

   __hw_cli(); // Disable interrupts

   if (prio > DQUEUE_PRIORITY_LEVELS) {
      return;
   }

   if (!(node=AllocNode())) {
      // Check if there are no nodes left -> Error, reached limit of jobs
      return;
   }
   node->job = job;
   node->arg = arg;
   node->next = NULL;

   DQueueInsert(&systemQ[prio], node);
   priobmap = setbit(priobmap, prio);

   __hw_sti(); // Enable interrupts
}

void Dispatcher(void)
{
   int top;
   dQueue_t *newQ;

   __hw_cli(); // Disable interrupts

   top = _ffs(priobmap);
   if (top > DQUEUE_PRIORITY_LEVELS) {
      top = -1;
   }
   if (top < 0) {
      goto idle;
   }
   if (top != cPrio) {
      cPrio = top;
      newQ = &systemQ[top];
      SwitchQueue(newQ, &currentQ);
   }

   __hw_sti(); // Enable interrupts
   return;

idle:
   __hw_sti(); // Enable interrupts
   idle();
}

void DQueueLauncher()
{
   while (1) {
      DQueueProcessor();
   }
}

#define JUMP_TASK(entry, arg) \
   __asm__ __volatile__("p0 = %0;\n\t" \
                        "r0 = %1;\n\t" \
                        "reti = p0;\n\t" \
                        "rti;\n\t" \
                        : :"r"(entry), "r"(arg));

// Process jobs of a given queue
// This function should be called from an arch dependent
// DQueueProcessorStartup(). The function must pop the program
// counter and call DQueueProcessor().
void DQueueProcessor(dQueue_t *dQueue)
{
   dNode_t *next, *node;

   while (1) {
      node = dQueue->head; // Fetch next job

      // Execute Job
      node->job(node->args);

      // Dispatch Node is released
      node->next = fNodeList;
      fNodeList = node;

      // Next job
      dQueue->head = dQueue->head->next;
      if (dQueue->head == NULL) {
         priobmap = clearbit(priobmap, cPrio);
         dQueue->tail = NULL;
         sleep();
      }
   }
}

void InitDispatcher(void)
{
   int32_t i;
   T_CTSK pk_ctsk;

   memset(systemQ, 0, sizeof(systemQ));
   fIndex = 0;
   fNodeList = NULL;



   // Este código va a nivel de usuario junto con la librería del dispatcher
   // Assign main task attributes
   // Create one task for each priority level
   pk_ctsk.task        = DQueueProcessor;
   pk_ctsk.rdyque.this = pk_ctsk.synque.this = pk_ctsk.slpque.this = main_task;
   pk_ctsk.tskstat     = TTS_RDY;
   pk_ctsk.exinf       = TSK_VPARG;
   for (i=0; i<DQUEUE_PRIORITY_LEVELS; ++i) {
      pk_ctsk.tskpri = i;
      pk_ctsk.tskid = i;
      pk_ctsk.extdat = &systemQ[i];
      pk_ctsk.stk = USER_STACK;
      pk_ctsk.stksz = USER_STACK_SIZE;
      cre_tsk_sys(i, &pk_ctsk);
   }
}


#endif
