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

#include <config.h>

#include<libminisip/media/rtp/XRReportBlock.h>
#include<libminisip/media/rtp/XRVoIPReportBlock.h>

#include<stdlib.h>
//#include<netinet/in.h>
#include<iostream>

using namespace std;



XRReportBlock *XRReportBlock::build_from(void *from, int max_length){
	unsigned char *ucptr = (unsigned char *)from;
	switch(*ucptr){
		case LOSS_RLE_REPORT:
			cerr << "XR report type LOSS_RLE_REPORT not implemented"<< endl;
			exit(1);
			break;
		case DUPLICATE_RLE_REPORT:
			cerr << "XR report type DUPLICATE_RLE_REPORT not implemented"<< endl;
			exit(1);
			break;
		case TIMESTAMP_REPORT:
			cerr << "XR report type TIMESTAMP_REPORT not implemented"<< endl;
			exit(1);
			break;
		case STATISTIC_SUMMARY_REPORT:
			cerr << "XR report type STATISTIC_SUMMARY_REPORT not implemented"<< endl;
			exit(1);
			break;
		case RECEIVER_TIMESTAMP_REPORT:
			cerr << "XR report type RECEIVER_TIMESTAMP_REPORT not implemented"<< endl;
			exit(1);
			break;
		case DLRR_REPORT:
			cerr << "XR report type DLRR_REPORT not implemented"<< endl;
			exit(1);
			break;
		case VOIP_METRICS_REPORT:
			return new XRVoIPReportBlock(from, max_length);
			break;
		default:
			cerr << "ERROR: Unknown RTCP XR Report block"<< endl;
			exit(1);
	}

	return NULL;	
}

void XRReportBlock::parse_header(void *from){
	unsigned char *ucptr = (unsigned char *)from;
	block_type = *ucptr;
	type_specific = *(ucptr+1);
	//unsigned short *sptr = (unsigned short *)from;
	//block_length = ntohs( *(sptr+1) );
	block_length = U16_AT( (unsigned short *)from );
}

