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

#include"LocationDetector.h"

#include<stdio.h>
#include<libmutil/itoa.h>
#include<libmutil/CommandString.h>
#include<libmsip/SipSMCommand.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

LocationDetector::LocationDetector(MRef<SipDialogContainer*> cb):callback(cb){

        Thread t(this);
}

void LocationDetector::run(){

#if 0
//	cerr << "Started LocationDetector thread"<< endl;
	int32_t i=1;
	do{
		struct timespec req;
		struct timespec rem;
		rem.tv_sec=5;
		rem.tv_nsec=0;
		do{
			req=rem;
//			cerr << "Going into sleep when sec="<<req.tv_sec <<" and nsec="<< req.tv_nsec<< endl;
			if (nanosleep(&req,&rem)!=-1)
				rem.tv_sec=rem.tv_nsec=0;
//			cerr << "Awake when sec="<<rem.tv_sec <<" and nsec="<< rem.tv_nsec<< endl;
		}while(!(rem.tv_sec==0 && rem.tv_nsec==0));
//		cerr << "INFO: LocationDetector loop done"<< endl;
//		callback->process_command("location="+itoa(i++));	
		CommandString c("","location",itoa(i++) );
		callback->handleCommand(SipSMCommand(c, SipSMCommand::remote, SipSMCommand::TU));	
		
	
	}while(true);

#endif

}

/*
void LocationDetector::loop_starter(){
	((LocationDetector *)arg)->loop();
	return NULL;
}
*/

