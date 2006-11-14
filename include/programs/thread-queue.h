/*inspired on gphone*/

#ifndef _THREAD_QUEUE_H
#define _THREAD_QUEUE_H 1

#include <glib.h>

typedef struct Thread_Queue *Thread_Queue;

extern Thread_Queue thread_queue_new(void);
extern void thread_queue_free(Thread_Queue queue);
gboolean thread_queue_empty(Thread_Queue queue);
extern guint thread_queue_length(Thread_Queue queue);
extern gpointer thread_queue_head(Thread_Queue queue);
extern void thread_queue_add(Thread_Queue queue, gpointer item);
extern void thread_queue_remove(Thread_Queue queue);
extern void thread_queue_remove_free(Thread_Queue queue);

#endif /* thread-queue.h */
