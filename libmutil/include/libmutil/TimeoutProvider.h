/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#ifndef TIMEOUTPROVIDER_H
#define TIMEOUTPROVIDER_H

/* Name
 *	TimeoutProvider.h
 * Purpose
 * 	Provides a way to request timeouts after a number of milli seconds. A command
 * 	is associated to each timeout.
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se, 2003
*/

/*
 *TODO:
 * - Complexity should be: Insert new timeout: O(log n); "Pop" timeout: O(1). Now it is O(n/2) and O(1). 
 *   This can be fixed with priority queues (heaps). It is not fixed since
 *   the implementation also needs to be able to return a list of all
 *   pending timeouts (for debugging purposes). This is not always the case
 *   of a heap (investigate or implement our own).
*/ 

#include<list>

#ifdef _MSC_VER
#ifdef LIBMUTIL_EXPORTS
#define LIBMUTIL_API __declspec(dllexport)
#else
#define LIBMUTIL_API __declspec(dllimport)
#endif
#else
#define LIBMUTIL_API
#endif

#include<assert.h>
#include"Mutex.h"

#include<libmutil/mtime.h>

#include"Thread.h"

#include<libmutil/minilist.h>
#include<libmutil/itoa.h>
#include<libmutil/MemObject.h>
#include<libmutil/CondVar.h>

/**
 * Reprsents a request of a "timeout" (delivery of a command to a
 * "timeout receiver" after at least a specified time period).
 * NOTE: This class is only used internaly.
 * @author Erik Eliasson
*/
template<class TOCommand, class TOSubscriber>
class TPRequest{
	public:
//		TPRequest(){}

		TPRequest( TOSubscriber tsi, int timeout_ms, const TOCommand &command):subscriber(tsi){
			
			when_ms = mtime();
			when_ms += timeout_ms;
			this->command=command;
		}
		
		/**
		 * @param t  ms since Epoch
		 */
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

		/**
		 * Two timeout requests are considered equeal if they have
		 * the same subscriber AND command AND time when they
		 * occur.
		 */
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
		uint64_t when_ms; 	 /// Time since Epoch in ms when the 
					 /// timeout will happen
					 
		TOCommand command;	 /// Command that will be delivered to the
					 /// receiver (subscriber) of the timeout.
};


/**
 * Class to generate objects giving timeout functionality.
 * @author Erik Eliasson
 */
template<class TOCommand, class TOSubscriber>
class TimeoutProvider : public MObject, Runnable{

	public:
		string getMemObjectType(){return "TimeoutProvider";}

		/**
		 * The purpose of this method is mainly
		 * monitoring/debugging features in applications.
		 * 
		 * @return All timeouts waiting to occur.
		 */
/*		string getTimeouts(){
			string ret;

			synch_lock.lock();
			
			for (int i=0 ; i<requests.size(); i++){
				int ms= requests[i].get_ms_to_timeout();
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
*/  

		std::list<TPRequest<TOCommand, TOSubscriber> > getTimeoutRequests(){
			std::list<TPRequest<TOCommand, TOSubscriber> > retlist;
			synch_lock.lock();
			for (int i=0; i< requests.size(); i++)
				retlist.push_back(requests[i]);
			synch_lock.unlock();
			
			return retlist;   
		}


		/**
		 * @param signal_no	The TimeoutProvider uses signals internally. Which 
		 * 			signal being used is specified as argument to the 
		 * 			constructor. The signal is only reveiced by the 
		 * 			internal thread (but it still has side effects such
		 * 			as setting signal handler).
		 */		
		TimeoutProvider(): requests(),waitCond(),synch_lock(),stop(false){
			thread = new Thread(this);
		}

		/**
		 * The destructor is not guaranteed to finnish since it 
		 * waits for the thread delivering timeouts to return. That
		 * thread will stop if it is either sleeping or after it
		 * has finished delivering a timeout.
		 */
		~TimeoutProvider(){
			stop=true;
			wake();
			thread->join();
			delete thread;
			thread=NULL;
		}

		/**
		 * @param time_ms	Number of milli-seconds until the timeout is 
		 * 			wanted. Note that a small additional period of time is
		 * 			added that depends on execution speed.
		 * @param subscriber	The receiver of the callback when the command has timed
		 * 			out. This argument must not be NULL.
		 * @param command	Specifies the String command to be passed back in the
		 * 			callback.
		 */
		void request_timeout(int32_t time_ms, TOSubscriber subscriber, const TOCommand &command){
			assert(subscriber);
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
                        synch_lock.lock();
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
		void run(){
			loop();
		}

	private:

                void wake(){
                        waitCondLock.lock();
			waitCond.signal();
                        waitCondLock.unlock();
                }

		void sleep(int ms){
			if (ms > 0){
                                waitCondLock.lock();
				waitCond.wait(&waitCondLock,ms);
                                waitCondLock.unlock();
                        }
		}
                
		void loop(){

			do{
				int32_t time=3600000;
                                synch_lock.lock();
				int32_t size=0;
				if ((size=requests.size())>0)
					time = requests[0].get_ms_to_timeout();
				if (time==0 && size > 0){
					if (stop){		//This must be checked so that we will
								//stop even if we have timeouts to deliver.
						return;
					}
					TPRequest<TOCommand, TOSubscriber> req = requests[0];
					TOSubscriber subs=req.get_subscriber();
					TOCommand command=req.get_command();
					requests.remove(req);
                                        synch_lock.unlock();
					assert(subs);		
					subs->timeout(command);
				}else{
                                        synch_lock.unlock();
					if (stop){		// If we were told to stop while delivering 
								// a timeot we will exit here
						return;
					}
					this->sleep(time);
					if (stop){		// If we are told to exit while sleeping we
								// will exit here return;
						return;
					}
				}
			}while(true);
		}

		minilist<TPRequest<TOCommand, TOSubscriber> > requests; /// List of timeouts waiting to be delivered or canceled.
									/// The timeouts are ordered in the order of which they 
									/// will expire. Nearest in future is first in list.
									///TODO: Change this functionality to a heap/priority queue 
									/// to improve scalability

		CondVar waitCond;	///Used to block until a signal from 
					///another thread or a timeout.
					
		Mutex waitCondLock;	
		
                Mutex synch_lock;	/// Protects the internal data structures
					/// from simultaneous modification
					/// by worker thread (loop) and
					/// external threads (request/cancel/...)

		Thread *thread;		/// Worker thread running "loop".
	
		bool stop;		/// Flag to tell the worker thread
					/// to terminate. Set to true and
					/// wake the worker thread to
					/// terminate it.
};

#endif
