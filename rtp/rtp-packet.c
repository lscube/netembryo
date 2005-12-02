/*
  Librtp - a library for the RTP/RTCP protocol
  Copyright (C) 2000  Roland Dreier
  
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
  
  $Id: rtp-packet.c,v 1.3 2001/05/07 18:55:06 dreier Exp $
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <netembryo/rtp-packet.h>
#include <netembryo/rtp.h>
#include <netembryo/wsocket.h>

#include <stdlib.h>
#include <glib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>


Rtp_Packet
rtp_packet_new_take_data(gpointer data, guint data_len)
{
  Rtp_Packet packet;

  g_return_val_if_fail(data_len < RTP_MTU, NULL);

  packet = g_malloc(sizeof *packet);

  packet -> data = data;
  packet -> data_len = data_len;

  return packet;
}

Rtp_Packet
rtp_packet_new_copy_data(gpointer data, guint data_len)
{
  Rtp_Packet packet;

  g_return_val_if_fail(data_len < RTP_MTU, NULL);

  packet = g_malloc(sizeof *packet);

  packet -> data = g_memdup(data, data_len);
  packet -> data_len = data_len;

  return packet;
}

Rtp_Packet
rtp_packet_new_allocate(guint payload_len, guint pad_len, guint csrc_count)
{
  guint len;
  Rtp_Packet packet;

  g_return_val_if_fail(csrc_count <= 15, NULL);

  len = RTP_HEADER_LEN
    + csrc_count * sizeof(guint32)
    + payload_len + pad_len;

  g_return_val_if_fail(len < RTP_MTU, NULL);

  packet = g_malloc(sizeof *packet);

  packet -> data_len = len; 
  packet -> data = g_malloc(len);

  return(packet);
}


void
rtp_packet_free(Rtp_Packet packet)
{
  g_return_if_fail(packet != NULL);

  g_free(packet -> data);
  g_free(packet);
}

/*chicco*/
Rtp_Packet
rtp_packet_read(Sock *fromaddr, int nbytes)
{
  int packlen;
  gpointer buf;

  buf = g_malloc(RTP_MTU);

  packlen = Sock_read(fromaddr,buf,nbytes);
  

  if (packlen < 0) {
    g_error("rtp_packet_read: recvfrom: %s", strerror(errno));
    exit(1);
  }

  return rtp_packet_new_take_data(buf, packlen);
}

/*chicco*/
void
rtp_packet_send(Rtp_Packet packet, Sock *toaddr, int nbytes)
{
  g_return_if_fail(packet != NULL);

  //sendto(fd, (void *) packet -> data,packet -> data_len, 0,toaddr, tolen);
  Sock_write(toaddr, packet -> data, nbytes);
}

guint8
rtp_packet_get_version(Rtp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return ((Rtp_Header) packet -> data) -> version;
}

void
rtp_packet_set_version(Rtp_Packet packet, guint8 version)
{
  g_return_if_fail(packet != NULL);
  g_return_if_fail(version < 0x04);

  ((Rtp_Header) packet -> data) -> version = version;
}

guint8
rtp_packet_get_padding(Rtp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return ((Rtp_Header) packet -> data) -> padding;
}

void
rtp_packet_set_padding(Rtp_Packet packet, guint8 padding)
{
  g_return_if_fail(packet != NULL);
  g_return_if_fail(padding < 0x02);

  ((Rtp_Header) packet -> data) -> padding = padding;
}

guint8
rtp_packet_get_csrc_count(Rtp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return ((Rtp_Header) packet -> data) -> csrc_count;
}

guint8
rtp_packet_get_extension(Rtp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return ((Rtp_Header) packet -> data) -> extension;
}

void
rtp_packet_set_extension(Rtp_Packet packet, guint8 extension)
{
  g_return_if_fail(packet != NULL);
  g_return_if_fail(extension < 0x02);

  ((Rtp_Header) packet -> data) -> extension = extension;
}

void
rtp_packet_set_csrc_count(Rtp_Packet packet, guint8 csrc_count)
{
  g_return_if_fail(packet != NULL);
  g_return_if_fail(csrc_count < 0x04);

  ((Rtp_Header) packet -> data) -> csrc_count = csrc_count;
}

guint8
rtp_packet_get_marker(Rtp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return ((Rtp_Header) packet -> data) -> marker;
}

void
rtp_packet_set_marker(Rtp_Packet packet, guint8 marker)
{
  g_return_if_fail(packet != NULL);
  g_return_if_fail(marker < 0x02);

  ((Rtp_Header) packet -> data) -> marker = marker;
}

guint8
rtp_packet_get_payload_type(Rtp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return ((Rtp_Header) packet -> data) -> payload_type;
}

void
rtp_packet_set_payload_type(Rtp_Packet packet, guint8 payload_type)
{
  g_return_if_fail(packet != NULL);
  g_return_if_fail(payload_type < 0x80);

  ((Rtp_Header) packet -> data) -> payload_type = payload_type;
}

guint16
rtp_packet_get_seq(Rtp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return g_ntohs(((Rtp_Header) packet -> data) -> seq);
}

void
rtp_packet_set_seq(Rtp_Packet packet, guint16 seq)
{
  g_return_if_fail(packet != NULL);

  ((Rtp_Header) packet -> data) -> seq = g_htons(seq);
}

guint32
rtp_packet_get_timestamp(Rtp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return g_ntohl(((Rtp_Header) packet -> data) -> timestamp);
}

void
rtp_packet_set_timestamp(Rtp_Packet packet, guint32 timestamp)
{
  g_return_if_fail(packet != NULL);

  ((Rtp_Header) packet -> data) -> timestamp = g_htonl(timestamp);
}

guint32
rtp_packet_get_ssrc(Rtp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return g_ntohl(((Rtp_Header) packet -> data) -> ssrc);
}

void
rtp_packet_set_ssrc(Rtp_Packet packet, guint32 ssrc)
{
  g_return_if_fail(packet != NULL);

  ((Rtp_Header) packet -> data) -> ssrc = g_htonl(ssrc);
}

guint
rtp_packet_get_payload_len(Rtp_Packet packet)
{
  guint len;

  g_return_val_if_fail(packet != NULL, 0);

  len = packet -> data_len
    - RTP_HEADER_LEN
    - rtp_packet_get_csrc_count(packet) * sizeof(guint32);

  if (rtp_packet_get_padding(packet)) {
    len -= ((guint8 *) packet -> data)[packet -> data_len - 1];
  }

  return len;
}

gpointer
rtp_packet_get_payload(Rtp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, NULL);

  return ((char *) packet -> data)
    + RTP_HEADER_LEN
    + rtp_packet_get_csrc_count(packet) * sizeof(guint32);
}

guint
rtp_packet_get_packet_len(Rtp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return packet -> data_len;
}
