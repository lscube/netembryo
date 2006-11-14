/*
  Gnome-o-Phone - A program for internet telephony
  Copyright (C) 1999  Roland Dreier
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
  
  $Id: thread-queue.c 1.1 Sun, 12 Sep 1999 18:20:08 -0500 dreier $
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <programs/thread-queue.h>

/*
  The current implementation uses a linked list and appends new items.
  It doesn't keep track of the end of the list, which means that
  adding a lot of items to a queue could get slow.  It might be nice
  to add a pointer to the end of the queue and use that to append, but 
  I don't feel like getting all the boundary conditions correct at the 
  moment.
*/

struct Thread_Queue {
	GMutex *mutex;
	GSList *list;
};

Thread_Queue thread_queue_new(void)
{
	Thread_Queue queue;

	queue = g_malloc(sizeof *queue);
	queue->mutex = g_mutex_new();
	queue->list = NULL;

	return queue;
}

void thread_queue_free(Thread_Queue queue)
{
	if (queue != NULL) {
		g_mutex_lock(queue->mutex);
		if (g_slist_length(queue->list) != 0) {
			g_warning("Freeing a thread-queue with nonempty list.");
		}
		g_free(queue->mutex);
		g_free(queue);
	} else {
		g_warning("Freeing a NULL thread-queue.");
	}
}

gboolean thread_queue_empty(Thread_Queue queue)
{
	gboolean empty;

	g_assert(queue != NULL);

	g_mutex_lock(queue->mutex);
	empty = ((queue->list) == NULL) ? TRUE : FALSE;
	g_mutex_unlock(queue->mutex);

	return empty;
}

guint thread_queue_length(Thread_Queue queue)
{
	guint length;

	g_assert(queue != NULL);

	g_mutex_lock(queue->mutex);
	length = g_slist_length(queue->list);
	g_mutex_unlock(queue->mutex);

	return length;
}

gpointer thread_queue_head(Thread_Queue queue)
{
	gpointer item;

	g_mutex_lock(queue->mutex);
	if (g_slist_length(queue->list) != 0) {
		item = queue->list->data;
	} else {
		item = NULL;
	}
	g_mutex_unlock(queue->mutex);

	return item;
}

void thread_queue_add(Thread_Queue queue, gpointer item)
{
	g_assert(queue != NULL);

	g_mutex_lock(queue->mutex);
	queue->list = g_slist_append(queue->list, item);
	g_mutex_unlock(queue->mutex);
}

void thread_queue_remove(Thread_Queue queue)
{
	g_assert(queue != NULL);

	g_mutex_lock(queue->mutex);
	queue->list = g_slist_remove_link(queue->list, queue->list);
	g_mutex_unlock(queue->mutex);
}

void thread_queue_remove_free(Thread_Queue queue)
{
	g_assert(queue != NULL);

	g_mutex_lock(queue->mutex);
	g_free(queue->list->data);
	queue->list = g_slist_remove_link(queue->list, queue->list);
	g_mutex_unlock(queue->mutex);
}
