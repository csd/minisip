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

#ifndef BELL_H
#define BELL_H

#include<config.h>
#include<sys/types.h>
#include<signal.h>
#include<string>
#include<libmutil/MemObject.h>
//#include<libmutil/TimeoutProvider.h>

using namespace std;

class Bell: public MObject{
	public:
		Bell();
		~Bell();
		void start();
		void stop();
		void loop();

		void timeout(const string &command);
		virtual std::string getMemObjectType(){return "Bell";};
	private:
//		TimeoutProvider<string, MRef<Bell*> > timeout_provider;
		volatile bool running;
		int32_t delayindex;
#ifdef IPAQ
		int ipaq_buzzer;
#endif
};

#ifdef IPAQ
#define BUZZER_IOCTL_BASE       'Z'
struct buzzer_time{
	unsigned int on_time;
	unsigned int off_time;
};
#define IOC_SETBUZZER   _IOW(BUZZER_IOCTL_BASE, 0, struct buzzer_time)
#endif


#endif
