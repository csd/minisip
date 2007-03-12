/*
 Copyright (C) 2004-2006 the Minisip Team
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef XRREPORTBLOCK_H
#define XRREPORTBLOCK_H

#include<libminisip/libminisip_config.h>

#define LOSS_RLE_REPORT            1
#define DUPLICATE_RLE_REPORT       2
#define TIMESTAMP_REPORT           3
#define STATISTIC_SUMMARY_REPORT   4
#define RECEIVER_TIMESTAMP_REPORT  5
#define DLRR_REPORT                6
#define VOIP_METRICS_REPORT        7

class LIBMINISIP_API XRReportBlock{
	public:
		virtual ~XRReportBlock(){}
		static XRReportBlock *build_from(void *from, int max_length);

#ifdef DEBUG_OUTPUT
		virtual void debug_print()=0;
#endif
		virtual int size()=0;
		
	protected:
		void parse_header(void *from);
		
		unsigned block_type;
		unsigned type_specific;
		unsigned block_length;

};

#endif
