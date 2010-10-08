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
#ifndef LOGGER_H
#define LOGGER_H

#include<string>
#include<queue>

#include "LoggingManager.h"
#include<libmutil/Thread.h>
#include<libmutil/MSingleton.h>
#include<libmnetutil/TCPSocket.h>
#include<libmsip/SipDialogConfig.h>

#define DEFAULT_LEVEL 0
#define INFO 0
#define DEBUG 1
#define ERROR 2

using namespace std;

class LoggingManager;

/*
 * Log utilities
 */
class LoggerUtils {

private:
	std::string processId;
	std::string callId;
	MRef<SipIdentity*> currentSipIdentity;
	std::string getTimeStamp();

public:
	LoggerUtils();
	std::string createLog(std::string value,std::string message); //Creates the log message into the XML format
	std::string getProcessId();										//Returns the process ID
	void setCurrentSipIdentity(MRef<SipIdentity*> currentSipIdentity);	//Sets the user ID
	void setCallId(std::string callId);				//Sets call ID
};

/*
 * Class which is a separate thread to send the logs to the logging server
 */
class LogSender: public Runnable{

public:
	LogSender(TCPSocket* senderSocket);
	~LogSender();
	bool start();							//Starts the thread
	bool stop();							//Stops the thread
	bool join();
	virtual void run();						//Starts the Log sender
	void bufferLogs(std::string log);		//Buffers the logs in the sender

private:
	TCPSocket* senderSocket;				//TCP socket to send the logs
	queue<string> sendBuffer;				//Buffer
	MRef<Thread *> thread;					//Thread

};


/*
 * Class which provides the logging interface
 */
class Logger : public MSingleton<Logger>, public MObject{

private:
	int level; 							//Log level
	int logCount;						//Number of Logs in the buffer
	bool loggingFlag;
	bool localLoggingFlag;
	std::string logDirectoryPath;		//Log Directory path
	MRef<SipIdentity*> currentSipIdentity;		//User ID

	queue<string> temporaryBuffer;		//Temporary buffer which keeps the logs in logger
	TCPSocket* senderSocket;			//TCP socket for sending logs

	LoggingManager* loggingManager; 	//Logging Manager
	LogSender* logSender;				//Log Sender

	void sendLogs(std::string log);		//Initializes the Log sender appropriately

public:
	std::string log_version;            //The version of the logging module
	LoggerUtils loggerUtils;			//Logger Utils

	Logger();
	~Logger();
	void info(std::string id, std::string message);  	//Logs the messages of information level
	void debug(std::string id, std::string message); 	//Logs the messages of debug level
	void error(std::string id, std::string message); 	//Logs the messages of error level

	void setLevel(std::string logLevel); 			 	//Sets the log level
	void setLoggingModuleVersion(void);					//Sets the Logging Version
	void setLoggingManager(LoggingManager* loggingManager);		//Sets the Logging Manager
	void setLogDirectoryPath(std::string logDirectoryPath);		//Sets the Log Directory path
	void setLoggingFlag(bool flag);						//Sets the logging flag
	void setLocalLoggingFlag(bool flag);						//Sets the local logging flag
	void setCurrentSipIdentity(MRef<SipIdentity*>);			//Sets the SipIdentity
	void startLogger();									//Starts the logger
	void stopLogger();									//Stops the logger

};

#endif
