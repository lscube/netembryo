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
  
  $Id: thread-queue.h 1.1 Sun, 12 Sep 1999 18:20:08 -0500 dreier $
*/

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
