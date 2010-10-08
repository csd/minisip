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

#include<sys/stat.h>
#include<fstream>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/time.h>

#include<libminisip/logging/Logger.h>
#include<libminisip/logging/LoggingManager.h>
#include<libmnetutil/TCPSocket.h>
#include<libmnetutil/NetworkException.h>

using namespace std;

LogSender::LogSender(TCPSocket* senderSocket) {
	this->senderSocket = senderSocket;
}

LogSender::~LogSender() {
}

//Starts the Thread of Log Sender
bool LogSender::start() {
	thread = NULL;
	thread = new Thread(this);
	return !thread.isNull();
}

bool LogSender::stop() {
}

bool LogSender::join() {
	if (thread.isNull()) {
		return false;
	}
	thread->join();
}

//Starts the thread which sends the logs to the log sender
void LogSender::run() {

	//Sends the log
	while (!sendBuffer.empty()) {
		this->senderSocket->write(sendBuffer.front() + "\n");
		sendBuffer.pop();
	}

}

//Places the logs to be taken by the sender
void LogSender::bufferLogs(std::string log) {
	this->sendBuffer.push(log);
}

FILE *logFilePointer;

static std::string GetFileName() {
	std: string filename = "MiniSIPLog-";
	time_t timestamp;
	struct tm * timestampStruct;
	char timestampString[80];

	time(&timestamp);
	timestampStruct = localtime(&timestamp);
	strftime(timestampString, 20, "%s", timestampStruct);
	filename = string("/") + filename + timestampString;
	return (filename);
}

std::string logFileName = GetFileName();

//Flag to check the connection establishments
bool connected = false;

Logger::Logger() {
}

Logger::~Logger() {
}

//Logs the message if the log level is INFO
void Logger::info(std::string id, std::string message) {
	if (loggingFlag) {
		if (level <= INFO) {
			std::string logXMLString = loggerUtils.createLog(id, message);
			if (logFilePointer != NULL && this->localLoggingFlag) {
				//Writing into that file
				fprintf(logFilePointer, "%s\n", logXMLString.c_str());
				fflush(logFilePointer);
			}
			//Sends the logs to the Log sender
			sendLogs(logXMLString);
		}
	}
}

//Logs the message if the log level is DEBUG or lower
void Logger::debug(std::string id, std::string message) {
	if (loggingFlag) {
		if (level <= DEBUG) {
			std::string logXMLString = loggerUtils.createLog(id, message);
			if (logFilePointer != NULL && this->localLoggingFlag) {
				//Writing into that file
				fprintf(logFilePointer, "%s\n", logXMLString.c_str());
				fflush(logFilePointer);
			}
			//Sends the logs to the Log sender
			sendLogs(logXMLString);
		}
	}
}

//Logs the message if the log level is ERROR or lower
void Logger::error(std::string id, std::string message) {
	if (loggingFlag) {
		if (level <= ERROR) {
			std::string logXMLString = loggerUtils.createLog(id, message);
			if (logFilePointer != NULL && this->localLoggingFlag) {
				//Writing into that file
				fprintf(logFilePointer, "%s\n", logXMLString.c_str());
				fflush(logFilePointer);
			}
			//Sends the logs to the Log sender
			sendLogs(logXMLString);
		}
	}
}

//Sets the log level
void Logger::setLevel(std::string logLevel) {
	if (logLevel.compare("INFO") == 0) {
		this->level = INFO;
	} else if (logLevel.compare("DEBUG") == 0) {
		this->level = DEBUG;
	} else if (logLevel.compare("ERROR") == 0) {
		this->level = ERROR;
	} else {
		this->level = DEFAULT_LEVEL;
	}
}

//Initilizes the logging version
void Logger::setLoggingModuleVersion(void) {
	log_version = "0.0.7";
}

//Sets the logging manger reference
void Logger::setLoggingManager(LoggingManager* loggingManager) {
	this->loggingManager = loggingManager;
}

//Sets the log directory path
void Logger::setLogDirectoryPath(std::string logDirectoryPath) {
	this->logDirectoryPath = logDirectoryPath;
}

//Sets the logging flag
void Logger::setLoggingFlag(bool flag) {
	this->loggingFlag = flag;
}

//Sets the logging flag
void Logger::setLocalLoggingFlag(bool flag) {
	this->localLoggingFlag = flag;
}

//Sets the user ID
void Logger::setCurrentSipIdentity(MRef<SipIdentity*> currentSipIdentity) {
	this->currentSipIdentity = currentSipIdentity;
}

//Sends the logs to the sender
void Logger::sendLogs(std::string log) {
	if (connected && senderSocket != NULL) {
		senderSocket->write(log + "\n");
	}
}

void Logger::startLogger() {
	//Creating the folder if it does not exist
	struct stat st;

	if (this->localLoggingFlag) {
		if (stat(logDirectoryPath.c_str(), &st) == 0)
			cerr << "[Logger] The directory is already present" << endl;
		else {
			//Creating a new directory
			if (mkdir(logDirectoryPath.c_str(), 0777) == -1) {
				cerr << "[Logger] Error opening the new directory" << endl;
			}
		}

		//Opening the log file
		logFilePointer = fopen((logDirectoryPath + logFileName).c_str(), "w");
		if (logFilePointer == NULL) {
			cerr << "[Logger] Error opening the log file " << (logDirectoryPath
					+ logFileName) << endl;
		} else {
			cerr << "[Logger] Log file opened successfully\n" << endl;
		}
	}

	//Establishing a TCP connection with the logging server
	try {
		int32_t serverPort;
		sscanf(this->loggingManager->getLoggingServerPort().c_str(), "%d",
				&serverPort);
		senderSocket = new TCPSocket(
				this->loggingManager->getLoggingServerAddress().c_str(),
				serverPort);
		cerr << "Connection Established with logging server "
				<< this->loggingManager->getLoggingServerPort() << " "
				<< serverPort << endl;
		connected = true;
	} catch (ConnectFailed &) {
		cerr << "Connection Failed with the logging server" << endl;
	} catch (HostNotFound &) {
		cerr << "Logging Sever Not Found" << endl;
	} catch (...) {
		cerr << "Unknown network exception" << endl;
	}

	//Initializes the log count to 0
	logCount = 0;

	//sets the user ID in log utils
	loggerUtils.setCurrentSipIdentity(this->currentSipIdentity);

	FILE *crashFilePointer = fopen((this->loggingManager->getCrashDirectory()
			+ "/.crash_conf").c_str(), "w");
	if (crashFilePointer != NULL) {
		fprintf(
				crashFilePointer,
				(this->loggingManager->getLoggingServerAddress() + "\n").c_str());
		fprintf(crashFilePointer, (this->loggingManager->getLoggingServerPort()
				+ "\n").c_str());
		fprintf(crashFilePointer,
				(this->loggerUtils.getProcessId() + "\n").c_str());
		fclose(crashFilePointer);
	}
}

//Closes the connection with logging server and closes the files refering to by the logger
void Logger::stopLogger() {
	//Closes the TCP connection
	if (connected) {
		cerr << "closes the connection with logging server" << endl;
		this->senderSocket->close();
	}

	if (this->localLoggingFlag) {
		//Close the file
		cerr << "closes the local log file" << endl;
		if (logFilePointer != NULL) {
			fclose(logFilePointer);
		}
	}
}

LoggerUtils::LoggerUtils() {
	pid_t pid;
	char procid[100];
	if ((pid = getpid()) < 0) {
		cout << "Unable to get PID" << endl;
	} else {
		srand(pid);
		sprintf(procid, "%04x%04x-%04x-%04x-%04x-%04x%04x%04x",
				(uint16_t) rand(), (uint16_t) rand(), (uint16_t) rand(),
				(((uint16_t) rand() & 0x0fff) | 0x4000), (uint16_t) rand()
						% 0x3fff + 0x8000, (uint16_t) rand(),
				(uint16_t) rand(), (uint16_t) rand());
	}
	this->processId = procid;
}

void LoggerUtils::setCallId(std::string callId) {
	this->callId = callId;
}

std::string LoggerUtils::getTimeStamp() {
	size_t maxsize = 50;
	time_t rawtime;
	struct tm *timeinfo;
	struct timeval tm;
	struct timezone tz;
	char micro_sec[10];
	char str[50];
	size_t size;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	gettimeofday(&tm, &tz);
	size = strftime(str, maxsize, "%Y-%m-%d %H:%M:%S.", timeinfo);
	sprintf(micro_sec, "%06ld", tm.tv_usec);
	strcat(str, micro_sec);
	return str;
}

std::string LoggerUtils::getProcessId() {
	return processId;
}

//Creates the log_id part of the log
std::string LoggerUtils::createLog(std::string value, std::string message) {
	//starting log_id part   <log_id>
	std::string logIDString = "<log>";

	//Adding the timestamp
	logIDString = logIDString + "<ts>" + getTimeStamp() + "</ts>";

	//Adding the process ID
	logIDString = logIDString + "<pid>" + getProcessId() + "</pid>";

	//Adding the user ID
	logIDString = logIDString + "<uid>"
			+ currentSipIdentity->identityIdentifier + "</uid>";

	//Adding the call ID
	logIDString = logIDString + "<call_id>" + callId + "</call_id>";

	//Adding the Log Data
	logIDString = logIDString + "<log_id>" + message + "</log_id>";

	//Adding the log value
	logIDString = logIDString + "<log_value>" + value + "</log_value>";

	//Terminating the log tag
	logIDString = logIDString + "</log>";

	return logIDString;
}

//Sets the user ID
void LoggerUtils::setCurrentSipIdentity(MRef<SipIdentity*> currentSipIdentity) {
	this->currentSipIdentity = currentSipIdentity;
}
