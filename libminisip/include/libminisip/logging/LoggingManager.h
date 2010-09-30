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
#ifndef LOGGINGMANAGER_H
#define LOGGINGMANAGER_H

#include "Logger.h"
#include<libmutil/Thread.h>
#include<libmnetutil/TCPSocket.h>
#include<libmutil/MemObject.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>

using namespace std;

class Logger;
class LoggingConfiguration;
/*
 * Main class of the logging module.
 */
class LoggingManager {

public:
	LoggingManager(LoggingConfiguration* loggingConf);
	void init(); 								//Initializes the logging sub system
	void stop(); 								//Stops the logging sub system
	std::string getLoggingServerAddress();		//Returns the logging server address
	std::string getLoggingServerPort();			//Returns the logging server port
	std::string getCrashDirectory();			//Returns the crash directory path

private:
	void sendCrashReports();				//Send the crash reports in the startup of the minisip
	MRef<Logger*> logger;					//Logger

	std::string logDirectoryPath;			//Path for the logging directory
	std::string crashDirectoryPath;			//Path for the crash report directory

	std::string logServerAddress;			//Address of the log server
	std::string logServerPort;				//Port of the log server
	bool loggingFlag;						//Logging Flag
	bool localLoggingFlag;					//Flag to indicate storing log files locally

	MRef<SipIdentity*> currentSipIdentity;
};

/**
 * Class which holds the configuration of logging
 */
class LoggingConfiguration{

public:
	LoggingConfiguration(MRef<SipSoftPhoneConfiguration *> phoneConf);
	void setLoggingServerAddress(std::string address);	//Sets the logging server address
	void setLoggingServerPort(std::string port);		//Sets the logging server port
	void setLoggingFlag(bool loggingFlag);				//Sets the logging flag
	void setLocalLoggingFlag(bool localLoggingFlag);	//Sets the local logging flag

	std::string getLoggingServerAddress();				//Returns the logging server address
	std::string getLoggingServerPort();					//Returns the logging server port
	bool getLoggingFlag();								//Returns the logging flag
	bool getLocalLoggingFlag();							//Returns the local logging flag
	MRef<SipIdentity*> getCurrentSipIdentity();				//Returns the user id

private:
	std::string loggingServerAddress;					//Log server address
	std::string loggingServerPort;						//Log server port
	bool loggingFlag;									//Flag to on and off logging
	bool localLoggingFlag;								//Flag to switch the local logging

	MRef<SipIdentity*> currentSipIdentity;			//User Default Identity
};

/*
 * Crash report sender thread class
 */
class CrashSender: public Runnable{

public:
	CrashSender();
	~CrashSender();
	bool start();															//Starts the thread
	bool stop();															//Stops the thread
	bool join();
	virtual void run();														//Starts the Log sender
	void send();															//Sends the crash report
	std::string createXML(std::string message,std::string value);			//Creates the XML to send

private:
	TCPSocket* crashSenderSocket;			//Socket which send the crash report
	MRef<Thread *> thread;					//Thread

	std::string crashDirectoryPath;			//Path for the crash report directory
	std::string crashPID;					//Crash PID
	std::string logServerAddress;			//Log server address
	std::string logServerPort;				//Log server port
};

#endif
