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

#include<config.h>

#include<libminisip/gui/Bell.h>

#include<stdlib.h>
#include<stdio.h>
#include<signal.h>
#include<sys/stat.h>
#include<sys/types.h>

#ifdef _MSC_VER
#else
#	include<sys/time.h>
#	include<unistd.h>
#endif

#ifdef IPAQ
#	include <sys/ioctl.h>
#endif

#include<fcntl.h>


#include<iostream>

using namespace std;

Bell::Bell(){
	running=false;
}

Bell::~Bell(){
//	timeout_provider.cancel_request(this, "bell");
}

void Bell::start(){
	
	running=true;	
	delayindex=0;
//	timeout_provider.request_timeout(1, this, "bell");
#ifdef IPAQ
	if ((ipaq_buzzer = open("/dev/misc/buzzer",O_WRONLY))==-1){
#ifdef DEBUG_OUTPUT
		cerr << "Could not open IPAQ buzzer"<< endl;
#endif
		return;
	}
	struct buzzer_time t;
	t.on_time=500;
	t.off_time=1000;
	if (ioctl( ipaq_buzzer, IOC_SETBUZZER, &t)){
#ifdef DEBUG_OUTPUT
		cerr << "Could not start IPAQ buzzer"<< endl;
#endif
	}
#endif	
	
}

void Bell::stop(){
	running=false;
#ifdef IPAQ
	struct buzzer_time t;
	t.on_time=0;
	t.off_time=0;
	if (ipaq_buzzer>0){
		if (ioctl( ipaq_buzzer, IOC_SETBUZZER, &t)){
#ifdef DEBUG_OUTPUT
			cerr << "Could not stop IPAQ buzzer"<< endl;
#endif
		}
		close(ipaq_buzzer);
	}
#endif
}

/*
void Bell::timeout(const string &command){
	int32_t delays[6]={250,500,250,500, 250, 1000};
	int32_t n_delays=6;
	
//	cerr << "Bell: running timeout with index"<< delayindex<< endl;;

//	putchar((char)7);
//	fflush(stdout);

	if (running)
		timeout_provider.request_timeout(delays[delayindex], this, "bell");
	
	delayindex = (++delayindex) % n_delays;
}
*/

