/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef RTCPREPORTXR_H
#define RTCPREPORTXR_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif


#include<vector>
#include<libminisip/XRReportBlock.h>
#include<libminisip/RtcpReport.h>
#include<vector>


class LIBMINISIP_API RtcpReportXR : public RtcpReport{
	public:
		RtcpReportXR(void *build_from, int max_length);
		virtual ~RtcpReportXR(){}
//		virtual vector<unsigned char> get_bytes();
		virtual void debug_print();
		virtual int size();
		
	private:
		unsigned ssrc_or_csrc;
		std::vector <XRReportBlock *> xr_blocks;
};

#endif
