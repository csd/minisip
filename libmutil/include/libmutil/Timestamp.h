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

/*
IMPORTANT ... IT IS TOTALLY USELESS IN WINDOWS SYSTEMS ... SEE THE .CXX FILE
*/

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#ifdef WIN32
	//do not alert in mingw32 ... i don't know the equivalent pragma message ...
#	ifndef __MINGW32__
#		pragma message ("Using libmutil::Timestamp.h in windows system ... useless ...")
#	endif
#endif

#include <libmutil/libmutil_config.h>

#ifdef WIN32
#else
#	include<stdint.h>
#	include<sys/time.h>
#endif


#include<time.h>
#include<fstream>
#include<string>
#include<sstream>
#include<ctime>

#define FILE_NAME "/tmp/minisip_ts"
#define INVITE_START 	0
#define INVITE_END   	1
#define MIKEY_START  	2
#define MIKEY_END    	3
#define RINGING      	4
#define PACKET_IN    	5
#define PACKET_OUT   	6
#define TLS_START    	7
#define TLS_END		8
#define DH_PRECOMPUTE_START 	9
#define DH_PRECOMPUTE_END	10
#define MIKEY_CREATE_START	11
#define MIKEY_CREATE_END	12
#define RAND_START		13
#define RAND_END		14
#define SIGN_START		15
#define SIGN_END		16
#define AUTH_START		17
#define AUTH_END		18
#define MIKEY_PARSE_START	19
#define MIKEY_PARSE_END		20
#define TGK_START		21
#define TGK_END			22
#define USER_ACCEPT		23

#define TMP	     24

typedef clock_t SystemTime;

class LIBMUTIL_API Timestamp{
	public:
		Timestamp();
		Timestamp(const Timestamp&);
		~Timestamp();

		void save( uint32_t id );
		void save(std::string descr);
		void print();
		void print(std::string fileName);

#if 0
		/**
		 * initialize the file and writes the init_data
		 * in it.
		 * @param filename  the filename where the measurement
		 *                  results should be saved.
		 * @param init_data all the stuff you want to write in the
		 *                  file before the measurement results.
		 */
		void init(std::string filename, std::string init_data);

		/**
		 * start the timer
		 */
		void start();

		/**
		 * stop the timer
		 */
		void stop();

		/**
		 * compute the elapsed time in seconds and write it to the
		 * file defined in init().
		 * @param descr a descriptor for the time value
		 * @return the elapsed time as string
		 */
		std::string writeElapsedTime(std::string descr);
#endif
	private:
		uint32_t index;

		struct timezone * tz;

		struct timeval * values;
		int32_t * ids;

		int auto_id;
		std::string *strings;


		///Time that the timer was started
		double startTime;

		///Time that the timer was stopped
		double stopTime;

		///the name of the file for saving the results
		std::string filename;


};

extern LIBMUTIL_API Timestamp ts;

#endif
