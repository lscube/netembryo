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
  
  $Id:rtcp-packet.h 3 2005-12-06 10:40:45Z federico $
*/

#ifndef _RTCP_PACKET_H
#define _RTCP_PACKET_H 

#include <programs/rtp.h>
#include <netembryo/wsocket.h>
#include <stdlib.h>
#include <glib.h>
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Rtcp_Packet_Struct *Rtcp_Packet;
typedef struct Rtcp_Compound_Struct *Rtcp_Compound;

struct Rtcp_Packet_Struct {
  gpointer data;
  guint data_len;
  gboolean free_data;
};

struct Rtcp_Compound_Struct {
  gpointer data;
  guint data_len;
  guint max_len;
};

typedef struct Rtcp_Common_Header *Rtcp_Common_Header;
typedef struct Rtcp_RR_Header *Rtcp_RR_Header;
typedef struct Rtcp_SR_Header *Rtcp_SR_Header;
typedef struct Rtcp_SDES_Header *Rtcp_SDES_Header;
typedef struct Rtcp_BYE_Header *Rtcp_BYE_Header;
typedef struct Rtcp_APP_Header *Rtcp_APP_Header;
typedef struct SDES_Item *SDES_Item;
typedef struct Rtcp_RR *Rtcp_RR;

struct Rtcp_Common_Header {
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
  unsigned int count:5;         /* varies by packet type */
  unsigned int padding:1;       /* padding flag */
  unsigned int version:2;       /* protocol version */
#elif G_BYTE_ORDER == G_BIG_ENDIAN
  unsigned int version:2;       /* protocol version */
  unsigned int padding:1;       /* padding flag */
  unsigned int count:5;     /* varies by packet type */
#else
#error "G_BYTE_ORDER should be big or little endian."
#endif
  unsigned int packet_type:8;   /* RTCP packet type */
  guint16 length;               /* pkt len in words, w/o this word */
};

struct Rtcp_RR {
  guint32 ssrc;                 /* data source being reported */
  unsigned int fraction:8;      /* fraction lost since last SR/RR */
  int lost:24;                  /* cumul. no. pkts lost (signed!) */
  guint32 last_seq;             /* extended last seq. no. received */
  guint32 jitter;               /* interarrival jitter */
  guint32 lsr;                  /* last SR packet from this source */
  guint32 dlsr;                 /* delay since last SR packet */
};

struct Rtcp_SR_Header {
  guint32 ssrc;                 /* sender generating this report */
  guint32 ntp_sec;              /* NTP timestamp */
  guint32 ntp_frac;
  guint32 rtp_ts;               /* RTP timestamp */
  guint32 psent;                /* packets sent */
  guint32 osent;                /* octets sent */
  struct Rtcp_RR rr[1];         /* variable-length list */
};

struct Rtcp_RR_Header {
  guint32 ssrc;                 /* receiver generating this report */
  struct Rtcp_RR rr[1];         /* variable-length list */
};

struct SDES_Item {
  guint8 type;                  /* type of item (Rtcp_Sdes_Type) */
  guint8 length;                /* length of item (in octets) */
  gchar data[1];                /* text, not null-terminated */
};

struct Rtcp_SDES_Header {
  guint32 src;                  /* first SSRC/CSRC */
  struct SDES_Item item[1];     /* list of SDES items */
};
    
struct Rtcp_BYE_Header {
  guint32 src[1];               /* list of sources */
  gchar data[1];                /* reason for leaving */
};

struct Rtcp_APP_Header {
  guint32 ssrc;                 /* source */
  gchar name[4];                /* name */
  gchar data[1];                /* application data */
};


	
typedef void (*Rtcp_Foreach_Func) (Rtcp_Packet packet,
                                   gpointer data);
typedef Rtp_Source *(*Rtcp_Find_Member_Func) (guint32 src);
typedef void (*Rtcp_Member_SDES_Func) (Rtp_Source *s,
                                       guint8 type,
                                       char *data,
                                       guint8 length);


Rtcp_Compound rtcp_compound_new(void);
Rtcp_Compound rtcp_compound_new_allocate(guint len);
Rtcp_Compound rtcp_compound_new_take_data(gpointer data, guint data_len);
Rtcp_Compound rtcp_compound_new_copy_data(gpointer data, guint data_len);
void rtcp_compound_free(Rtcp_Compound compound);

void rtcp_compound_foreach(Rtcp_Compound compound,
                           Rtcp_Foreach_Func func,
                           gpointer data);
void rtcp_read_sdes(Rtcp_Packet packet,
                   Rtcp_Find_Member_Func find_member,
                   Rtcp_Member_SDES_Func member_sdes);

/*chicco*/
Rtcp_Compound rtcp_compound_read(Sock *fromaddr, int nbytes);
/*chicco*/
void rtcp_compound_send(Rtcp_Compound compound, Sock *toaddr, int nbytes);

Rtcp_Packet rtcp_packet_new(void);
Rtcp_Packet rtcp_packet_new_take_data(gpointer data, guint data_len);
Rtcp_Packet rtcp_packet_new_copy_data(gpointer data, guint data_len);
void rtcp_packet_free(Rtcp_Packet packet);
void rtcp_compound_add_sr(Rtcp_Compound compound,
                          guint32 ssrc,
                          guint32 timestamp,
                          guint32 packets_sent,
                          guint32 octets_sent);
void rtcp_compound_add_sdes(Rtcp_Compound compound,
                            guint32 ssrc,
                            guint nsdes,
                            Rtcp_Sdes_Type type[],
                            char *value[],
                            gint8 length[]);
void rtcp_compound_add_app(Rtcp_Compound compound,
                           guint32 ssrc,
                           const gchar name[4],
                           gpointer data,
                           guint data_len);

guint16 rtcp_compound_get_length(Rtcp_Compound compound);
void rtcp_compound_set_length(Rtcp_Compound compound, guint16 len);
guint8 rtcp_packet_get_version(Rtcp_Packet packet);
void rtcp_packet_set_version(Rtcp_Packet packet, guint8 version);
guint8 rtcp_packet_get_padding(Rtcp_Packet packet);
void rtcp_packet_set_padding(Rtcp_Packet packet, guint8 padding);
guint8 rtcp_packet_get_count(Rtcp_Packet packet);
void rtcp_packet_set_count(Rtcp_Packet packet, guint8 count);
guint8 rtcp_packet_get_packet_type(Rtcp_Packet packet);
void rtcp_packet_set_packet_type(Rtcp_Packet packet, guint8 packet_type);
guint16 rtcp_packet_get_length(Rtcp_Packet packet);
void rtcp_packet_set_content_length(Rtcp_Packet packet, guint16 length);
gpointer rtcp_packet_get_data(Rtcp_Packet packet);
gpointer rtcp_packet_get_content(Rtcp_Packet packet);
gchar *rtcp_app_packet_get_name(Rtcp_Packet packet);
gpointer rtcp_app_packet_get_data(Rtcp_Packet packet);

#ifdef __cplusplus
}
#endif

#endif /* rtcp-packet.h */
