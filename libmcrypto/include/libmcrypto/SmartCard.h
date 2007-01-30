/*
  Copyright (C) 2005, 2004 Erik Eliasson, Pan Xuan
  
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
 *          Pan Xuan <xuan@kth.se>
*/


#ifndef SMARTCARD_H
#define SMARTCARD_H

#include <libmcrypto/config.h>

#include <libmutil/MemObject.h>
#include <libmutil/Thread.h>
#include <libmutil/MessageRouter.h>
#include <map>

typedef long SCARDCONTEXT;
typedef long SCARDHANDLE;
typedef struct _SCARD_IO_REQUEST SCARD_IO_REQUEST;

class LIBMCRYPTO_API SmartCard : public virtual MObject {
public:
	/* constructor is called to connect to the smart card */
	SmartCard();
		
		/* destructor is called to disconnect from smart card */
		~SmartCard();

		void close();

		/* smart card transaction */
		void startTransaction();
		void endTransaction();

		/* one round trip of APDU exchange */
		bool transmitApdu(unsigned long sendLength, unsigned char * sendBufferPtr, 
						  unsigned long & recvLength, unsigned char * recvBufferPtr);
		
		/* Smart card Pin related functions */
		void setPin(const char * pinCode);
		void setAdminPin(const char * adminPinCode);
		virtual bool verifyPin(int verifyMode) = 0;
		virtual bool changePin( const char * newPinCode) = 0;

protected:
	
/* This is used each time right before you read from or write on smart card. To check out whether the connection has been established */
	bool establishedConnection;							

/* The size of multi-string buffer including NULLs */
	unsigned long  readerLength;				

/* Multi-string name of the readers. Each name is separated by a NULL. The string ends with a double NULL */
	char * readerNamesPtr;

/* This map is used to store the index and reader names: map<int index, char * readerNames> */
	std::map <int,char *> readerMap;

/* The resource manager handler */
	SCARDCONTEXT hContext;

/* The smart card connection handler */
	SCARDHANDLE hCard;
	
	unsigned char * userPinCode;	

	unsigned char * adminPinCode;

	const SCARD_IO_REQUEST * protPci; 

};

class CommandReceiver;

class SmartCardDetector : public virtual Runnable, public virtual CommandReceiver{
public:
	SmartCardDetector(MRef<CommandReceiver*> callback);

	virtual void handleCommand(std::string subsystem, const CommandString& command);
	virtual CommandString handleCommandResp(std::string subsystem, const CommandString&);

	void run();
	void start();
	void join();

private:
	void connect();
	std::string reader;
	MRef<CommandReceiver*> callback;
	bool doStop;
	ThreadHandle th;
	bool pinVerified;


};




#endif

