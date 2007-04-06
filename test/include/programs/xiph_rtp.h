/* *
 *  $Id$
 *
 *  This file is part of NetEmbryo
 *
 *  NetEmbryo -- default network wrapper
 *
 *  Copyright (C) 2006 by
 *
 *      - Luca Barbato          <lu_zero@gentoo.org>
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

/*client side*/

#define RTP_HEADER  12

#define RTP_DATA(pkt)           (pkt+RTP_HEADER)

#define RTP_XIPH_ID(pkt)        (RTP_DATA(pkt)[0]<<16 +\
                                 RTP_DATA(pkt)[1]<<8 +\
                                 RTP_DATA(pkt)[2]    )

#define RTP_XIPH_F(pkt)         ((RTP_DATA(pkt)[3]& 0xc0)>> 6)

#define RTP_XIPH_T(pkt)         ((RTP_DATA(pkt)[3]& 0x30)>> 4)

#define RTP_XIPH_PKTS(pkt)      (RTP_DATA(pkt)[3]& 0x0F)

#define RTP_XIPH_LEN(pkt, off)  (RTP_DATA(pkt)[off]<<8+\
                                 RTP_DATA(pkt)[off+1])

#define RTP_XIPH_DATA(pkt, off) (RTP_DATA(pkt)[off+2])


/*server side*/


