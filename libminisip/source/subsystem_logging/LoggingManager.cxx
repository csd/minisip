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

#include<string>
#include<iostream>
#include<fstream>
#include<dirent.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>

#include<libmnetutil/TCPSocket.h>
#include<libmnetutil/NetworkException.h>
#include<libminisip/config/UserConfig.h>
#include<libminisip/logging/Logger.h>
#include<libminisip/logging/LoggingManager.h>

using namespace std;

/**
 * Main class in logging module which starts the logger and manages logging functionality
 */
LoggingManager::LoggingManager(LoggingConfiguration* loggingConf) {

	//Initializes the logging server properties
	this->logServerAddress = loggingConf->getLoggingServerAddress();
	this->logServerPort = loggingConf->getLoggingServerPort();
	this->loggingFlag = loggingConf->getLoggingFlag();
	this->localLoggingFlag = loggingConf->getLocalLoggingFlag();
	this->currentSipIdentity = loggingConf->getCurrentSipIdentity();

	//Log directory location definition
	logDirectoryPath = UserConfig::getMiniSIPHomeDirectory() + "/logs";

	//Crash report location definition
	crashDirectoryPath = UserConfig::getMiniSIPHomeDirectory()
			+ "/crash_reports";

	//Sets the Logging Manger to the Logger
	logger = MSingleton<Logger>::getInstance();
	logger->setLoggingManager(this);
	logger->setLogDirectoryPath(logDirectoryPath);
	logger->setCurrentSipIdentity(loggingConf->getCurrentSipIdentity());
	logger->setLoggingFlag(this->loggingFlag);
	logger->setLocalLoggingFlag(this->localLoggingFlag);

	if (this->loggingFlag) {
		logger->startLogger();
	}
}

//Initializes the Logging Manager
void LoggingManager::init() {
}

//Stops the logging moduel functions
void LoggingManager::stop() {
	//Stops logger
	this->logger->stopLogger();
}

//Returns the logging server address
std::string LoggingManager::getLoggingServerAddress() {
	return this->logServerAddress;
}

//Returns the logging server port
std::string LoggingManager::getLoggingServerPort() {
	return this->logServerPort;
}

std::string LoggingManager::getCrashDirectory() {
	return this->crashDirectoryPath;
}

//Sends the crash reports in the startup of minisip
void LoggingManager::sendCrashReports() {
	CrashSender* crashSender = new CrashSender();
	crashSender->start();
}

LoggingConfiguration::LoggingConfiguration(
		MRef<SipSoftPhoneConfiguration*> phoneConf) {
	this->loggingServerAddress = phoneConf->logServerAddr;
	this->loggingServerPort = phoneConf->logServerPort;
	this->loggingFlag = phoneConf->loggingFlag;
	this->currentSipIdentity = phoneConf->defaultIdentity;
	this->localLoggingFlag = phoneConf->localLoggingFlag;
}

//Sets the logging server address
void LoggingConfiguration::setLoggingServerAddress(std::string address) {
	this->loggingServerAddress = address;
}

//Sets the logging server port
void LoggingConfiguration::setLoggingServerPort(std::string port) {
	this->loggingServerPort = port;
}

//Sets the logging flag
void LoggingConfiguration::setLoggingFlag(bool loggingFlag) {
	this->loggingFlag = loggingFlag;
}

//Sets the local logging flag
void LoggingConfiguration::setLocalLoggingFlag(bool localLoggingFlag) {
	this->localLoggingFlag = localLoggingFlag;
}

//Returns the logging server address
std::string LoggingConfiguration::getLoggingServerAddress() {
	return this->loggingServerAddress;
}

//Returns the logging server port
std::string LoggingConfiguration::getLoggingServerPort() {
	return this->loggingServerPort;
}

//Returns the logging sever flag
bool LoggingConfiguration::getLoggingFlag() {
	return this->loggingFlag;
}

//Returns the local logging sever flag
bool LoggingConfiguration::getLocalLoggingFlag() {
	return this->localLoggingFlag;
}

//Returns the User ID
MRef<SipIdentity*> LoggingConfiguration::getCurrentSipIdentity() {
	return this->currentSipIdentity;
}

bool initialized = false;
std::string getTimeStamp();

CrashSender::CrashSender() {
	//Crash report location definition
	crashDirectoryPath = UserConfig::getMiniSIPHomeDirectory()
			+ "/crash_reports";
	ifstream crashConfFile((crashDirectoryPath + "/.crash_conf").c_str());

	if (crashConfFile.is_open()) {
		getline(crashConfFile, this->logServerAddress);
		getline(crashConfFile, this->logServerPort);
		getline(crashConfFile, this->crashPID);
		initialized = true;
	}
}

CrashSender::~CrashSender() {
}

bool CrashSender::start() {
	thread = NULL;
	thread = new Thread(this);
	return !thread.isNull();
}

bool CrashSender::stop() {
}

bool CrashSender::join() {
	if (thread.isNull()) {
		return false;
	}
	thread->join();
}

//Starts the thread which sends the crash report
void CrashSender::run() {

	if (initialized) {
		try {
			//sends the crash report files to the server
			//Scans the crash report directory for files to be sent
			int count, i;
			struct dirent **files;

			count = scandir(crashDirectoryPath.c_str(), &files, 0, alphasort);

			if (count >= 0) {
				if (count == 0) {
					cerr << "No crash reports to be sent" << endl;
				} else {
					for (i = 1; i < count + 1; ++i) {
						//Reads the files with .report extension
						std::string fileName = string(files[i - 1]->d_name);
						std::string extension = ".report";
						size_t pos = fileName.find(".");

						if (strcmp(fileName.substr(pos).c_str(),
								extension.c_str()) == 0) {

							//Reading from the crash report
							std::string crashReport = crashDirectoryPath + "/"
									+ fileName;
							ifstream crashReportFile(crashReport.c_str());

							if (crashReportFile.is_open()) {
								//Establishing the connection with logging server
								int32_t serverPort;
								sscanf(this->logServerPort.c_str(), "%d",
										&serverPort);
								crashSenderSocket = new TCPSocket(
										this->logServerAddress, serverPort);

								string crashLine = "", line;
								while (!crashReportFile.eof()) {
									getline(crashReportFile, line);
									crashLine = crashLine + line + "\\n";
								}

								//closes the connection
								crashSenderSocket->write(createXML(crashLine,
										"system.crashreport"));
								crashSenderSocket->close();
							}

							//closes the file and renames it with "-sent"
							crashReportFile.close();
							rename(crashReport.c_str(),
									(crashReport + "-sent").c_str());
						}
					}
				}
				free(files);
			}

		} catch (ConnectFailed &) {
			cerr << "Connection Failed with the logging server" << endl;
		} catch (HostNotFound &) {
			cerr << "Logging Sever Not Found" << endl;
		}
	}
}

void CrashSender::send() {
	this->run();
}

//Creates the log_id part of the log
std::string CrashSender::createXML(std::string value, std::string message) {
	//starting log_id part   <log_id>
	std::string logIDString = "<log>";

	//Adding the timestamp
	logIDString = logIDString + "<ts>" + getTimeStamp() + "</ts>";

	//Adding the process ID
	logIDString = logIDString + "<pid>" + this->crashPID + "</pid>";

	//Adding the user ID
	logIDString = logIDString + "<uid></uid>";

	//Adding the call ID
	logIDString = logIDString + "<call_id></call_id>";

	//Adding the Log Data
	logIDString = logIDString + "<log_id>" + message + "</log_id>";

	//Adding the log value
	logIDString = logIDString + "<log_value>" + value + "</log_value>";

	//Terminating the log tag
	logIDString = logIDString + "</log>";

	return logIDString;
}

std::string getTimeStamp() {
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

