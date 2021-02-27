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
static dQueue_t systemQ[DQUEUE_PRIORITY_LEVELS]; // Queue array

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
   //priobmap = setbit(priobmap, prio);
   rel_wai();
}

// Process jobs of a given queue
void DQueueProcessor(void *arg)
{
   dNode_t *node;
   dQueue_t *dQueue = arg;

   while (1) {
      if (dQueue->head == NULL) {
         priobmap = clearbit(priobmap, cPrio);
         dQueue->tail = NULL;
         slp_tsk();
      }
      node = dQueue->head; // Fetch next job

      // Execute Job
      node->job(node->args);

      // Dispatch Node is released
      node->next = fNodeList;
      fNodeList = node;

      // Next job
      dQueue->head = dQueue->head->next;
   }
}

void InitDispatcher(void)
{
   int32_t i;
   T_CTSK pk_ctsk;

   memset(systemQ, 0, sizeof(systemQ));
   fIndex = 0;
   fNodeList = NULL;

   // Create one task for each priority level
   pk_ctsk.task        = DQueueProcessor;
   pk_ctsk.rdyque.this = pk_ctsk.synque.this = pk_ctsk.slpque.this = main_task;
   pk_ctsk.tskstat     = TTS_RDY;
   pk_ctsk.exinf       = TSK_VPARG;
   pk_ctsk.stksz       = DQUEUE_STACK_SIZE;
   for (i=0; i<DQUEUE_PRIORITY_LEVELS; ++i) {
      pk_ctsk.tskpri = i;
      pk_ctsk.tskid = i;
      pk_ctsk.extdat = &systemQ[i];
      pk_ctsk.stk = systemQ[i].stack;
      cre_tsk(i, &pk_ctsk);
   }
}
