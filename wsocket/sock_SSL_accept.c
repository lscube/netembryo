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


#include <config.h>
#include <netembryo/wsocket.h>
#include <openssl/ssl.h>

int sock_SSL_accept(SSL **ssl_con, int new_fd)
{
	int ssl_err;
	X509 * client_cert;
	char * str;
	
	*ssl_con = get_ssl_connection(new_fd);

	if(!(*ssl_con)) {
		net_log(NET_LOG_ERR, "sock_SSL_accept: get_ssl_connection() returned NULL.\n");
		return WSOCK_ERROR;
	}

	if (SSL_accept(*ssl_con) <= 0) {
		net_log(NET_LOG_ERR, "sock_SSL_accept: SSL_accept() failed.\n");
		sock_SSL_close(*ssl_con);
		return WSOCK_ERROR;
	}

#if 0
	/*Client Cert. Not used*/
	client_cert = SSL_get_peer_certificate (*ssl_con);
	if (client_cert != NULL) {
		printf ("Client certificate:\n");
		str = X509_NAME_oneline (X509_get_subject_name (client_cert), 0, 0);
		/*printf ("\t subject: %s\n", str);
		free (str);*/
		str = X509_NAME_oneline (X509_get_issuer_name  (client_cert), 0, 0);
		/*printf ("\t issuer: %s\n", str);
		free (str);*/
		X509_free (client_cert);
	}
	/*---------------------*/
#endif

	return WSOCK_OK;
}
