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

#ifndef TIMEOUTPROVIDER_H
#define TIMEOUTPROVIDER_H

/* Name
 *	TimeoutProvider.h
 * Purpose
 * 	Provides a way to request timeouts after a number of milli seconds. To each a
 * 	command is associated.
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se, 2003
*/

/*
 *TODO: Handle destruction ("join" threads)
*/ 

/*
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
*/
#include<list>

/*
#ifdef HAVE_SIGNAL_H
#include <sys/types.h>
#include<signal.h>
#include<sys/time.h>
#endif
*/

/*
#ifdef WIN32
#include<windows.h>
#endif
*/

#include<assert.h>
#include"Mutex.h"

//#ifdef WIN32
//#include<libmutil/gettimeofday.h>
//#else
//#include<sys/time.h>
//#include<time.h>
//#endif

#include<libmutil/mtime.h>

#include"Thread.h"

#include<libmutil/minilist.h>
#include<libmutil/itoa.h>
#include<libmutil/MemObject.h>
#include<libmutil/CondVar.h>
//#include<stdint.h>


/**
 * NOTE: This class is only used internaly.
 * @author Erik Eliasson
*/
template<class TOCommand, class TOSubscriber>
class TPRequest{
	public:
		TPRequest(){}
		TPRequest( TOSubscriber tsi, int timeout_ms, const TOCommand &command):subscriber(tsi){
			
			when_ms = mtime();
			when_ms += timeout_ms;
			this->command=command;
		}
		
		bool happens_before(uint64_t t){
			if (when_ms < t)
				return true;
			if (when_ms > t)
				return false;
			return false; //if equal it does not "happens_before"
			
		}
		
		bool happens_before(const TPRequest &req){
			return happens_before(req.when_ms);
		}
		
		/**
		 * Number of milli seconds until timeout from when this method is
		 * called
		 */
		int get_ms_to_timeout(){
			uint64_t now=mtime();
			if (happens_before(now))
				return 0;
			else
				return when_ms - now;
		}
		
		TOCommand get_command(){
			return command;
		}
		
		TOSubscriber get_subscriber(){
			return subscriber;
		}

		bool operator==(const TPRequest<TOCommand, TOSubscriber> &req){
			if (req.subscriber==subscriber && 
					req.command==command && 
					req.when_ms == when_ms)
				return true;
			else 
				return false;
		}
		
	private:
		TOSubscriber subscriber;
		uint64_t when_ms;
		TOCommand command;
};


/**
 * Class to generate objects giving timeout functionality.
 * @author Erik Eliasson
 */
template<class TOCommand, class TOSubscriber>
class TimeoutProvider : public MObject{

	public:
		string getMemObjectType(){return "TimeoutProvider";}
//#ifdef DEBUG_OUTPUT
		string getTimeouts(){
			string ret;

			synch_lock.lock();
			//typename std::list<TPRequest<TOCommand> >::iterator i=requests.begin();
			
			for (int i=0 ; i<requests.size(); i++){
				//int ms= (*i).get_ms_to_timeout();
				int ms= requests[i].get_ms_to_timeout();
				//TimeoutSubscriberInterface<TOCommand, TOSubscriber> *receiver = requests[i].get_subscriber();
				TOSubscriber receiver = requests[i].get_subscriber();
				ret = ret + "      " 
					+ string("Command: ") + requests[i].get_command() 
					+ "  Time: " + itoa(ms/1000) + "." + itoa(ms%1000)
					+ "  Receiver ObjectId: "+itoa((int)receiver)
					+"\n";
			}
			synch_lock.unlock();
			return ret;
		}
                
		std::list<TPRequest<TOCommand, TOSubscriber> > getTimeoutRequests(){
			std::list<TPRequest<TOCommand, TOSubscriber> > retlist;
			synch_lock.lock();
			for (int i=0; i< requests.size(); i++)
				retlist.push_back(requests[i]);
//			retlist = requests;
			synch_lock.unlock();
			
			//return requests;   
			return retlist;   
		}
//#endif

		/**
		 * @param signal_no	The TimeoutProvider uses signals internally. Which 
		 * 			signal being used is specified as argument to the 
		 * 			constructor. The signal is only reveiced by the 
		 * 			internal thread (but it still has side effects such
		 * 			as setting signal handler).
		 */		
		TimeoutProvider(): requests(){
			Thread::createThread(loop_starter, (void*)this);
		}

		~TimeoutProvider(){
			cerr << "ERROR: BUG: Destructing timeout provider is not yet supported"<<endl;
		}

		/**
		 * @param time_ms	Number of milli-seconds until the timeout is 
		 * 			wanted. Note that a small additional period of time is
		 * 			added that depends on execution speed.
		 * @param subscriber	The receiver of the callback when the command has timed
		 * 			out.
		 * @param command	Specifies the String command to be passed back in the
		 * 			callback.
		 */
		void request_timeout(int32_t time_ms, TOSubscriber subscriber, const TOCommand &command){
			TPRequest<TOCommand, TOSubscriber> request(subscriber, time_ms, command);


                        synch_lock.lock();

			assert(subscriber);
			if (requests.size()==0){
				requests.push_front(request);
                                synch_lock.unlock();
                                wake();
				return;
			}

			if (request.happens_before(requests[0])){
				requests.push_front(request);
                                synch_lock.unlock();
                                wake();
				return;
			}

			if (requests[requests.size()-1].happens_before(request)){
				requests.push_back(request);
                                synch_lock.unlock();
                                wake();
				return;
			}

			int i=0;
			while (requests[i].happens_before(request))
				i++;

			requests.insert(i,request);             

                        synch_lock.unlock();
                        wake();
		}
		
		/**
		 * @see request_timeout
		 */
		void cancel_request(TOSubscriber subscriber, const TOCommand &command){
			//typename std::list<TPRequest<TOCommand, TOSubscriber> >::iterator i;
                        synch_lock.lock();
			//int size=requests.size();
			int loop_count=0;
			for (int i=0; loop_count<requests.size(); i++){
				if (requests[i].get_subscriber()==subscriber && requests[i].get_command()==command){
					requests.remove(i);
					i=0;
				}
				loop_count++;
			}
                        synch_lock.unlock();
		}

	private:

                void wake(){
			waitCond.signal();
                }

		void sleep(int ms){
			if (ms > 0)
				waitCond.wait(ms);
		}
                
		void loop(){

			do{
				int32_t time=3600000;
                                synch_lock.lock();
				int32_t size=0;
				if ((size=requests.size())>0)
					time = requests[0].get_ms_to_timeout();
				if (time==0 && size > 0){
					TPRequest<TOCommand, TOSubscriber> req = requests[0];
					TOSubscriber subs=req.get_subscriber();
					TOCommand command=req.get_command();
					requests.remove(req);
                                        synch_lock.unlock();
#ifdef DEBUG_OUTPUT
					mdbg << "TIMEOUTPROVIDER: timeout:"<<command<< end;
#endif
					subs->timeout(command);
				}else{
                                        synch_lock.unlock();
					this->sleep(time);
				}

			}while(true);
		}

		static void *loop_starter(void *tp){
			TimeoutProvider *t = (TimeoutProvider*)tp;
			t->loop();
//			((TimeoutProvider<TOCommand, TOSubscriber> *)tp)->loop();
			return NULL;
		}
		
		minilist<TPRequest<TOCommand, TOSubscriber> > requests;
		CondVar waitCond;
                Mutex synch_lock;
};

#endif
