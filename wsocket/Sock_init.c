/* * 
 *  $Id$
 *  
 *  This file is part of NetEmbryo 
 *
 * NetEmbryo -- default network wrapper 
 *
 *  Copyright (C) 2005 by
 *  	
 *	- Federico Ridolfo	<federico.ridolfo@polito.it>
 * 
 *  NetEmbryo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NetEmbryo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NetEmbryo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 * */


#include <netembryo/wsocket.h>
#include <stdarg.h>

#if HAVE_SSL
#include <openssl/ssl.h>
#endif

static void net_log_default(int level, const char *fmt, ...);

void (*net_log)(int, const char*, ...);

void Sock_init(void (*log_func)(int, const char*, ...))
{
#if HAVE_SSL
	SSL_library_init();
	SSL_load_error_strings();
#endif

	if (log_func) {
		net_log = log_func;
	} else {
		net_log = net_log_default;
	}

	return;
}

static void net_log_default(int level, const char *fmt, ...){
	va_list args;
	int no_print=0;

	switch (level) {
		case NET_LOG_FATAL:
			fprintf(stderr, "[fatal error] ");
			break;
		case NET_LOG_ERR:
			fprintf(stderr, "[error] ");
			break;
		case NET_LOG_WARN:
			fprintf(stderr, "[warning] ");
			break;
		case NET_LOG_DEBUG:
#ifdef DEBUG
			fprintf(stderr, "[debug] ");
#else
			no_print=1;	 
#endif
			break;
		case NET_LOG_VERBOSE:
#ifdef VERBOSE
			fprintf(stderr, "[verbose debug] ");
#else
			no_print=1;	 
#endif
			break;
		default: /*NET_LOG_INFO*/
			fprintf(stderr, "[info] ");
			break;
	}

	if(!no_print){
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		va_end(args);
		fflush(stderr);
	}
}
