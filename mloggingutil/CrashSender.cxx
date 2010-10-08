#include "CrashSender.h"
#include<stdlib.h>
#include<stdio.h>
#include<string>
#include<iostream>
#include<fstream>
#include<time.h>
#include<dirent.h>
#include<sys/time.h>
#include<libmnetutil/TCPSocket.h>
#include<libmnetutil/NetworkException.h>

using namespace std;

bool initialized = false;
std::string getTimeStamp();

CrashSender::CrashSender() {
	//Crash report location definition
	crashDirectoryPath =  string(getenv("HOME"))+"/.minisip/crash_reports";
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


//Starts the thread which sends the crash report
bool CrashSender::run() {
	bool sent = false;
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
								if(crashSenderSocket != NULL){
									string crashLine = "", line;
									while (!crashReportFile.eof()) {
										getline(crashReportFile, line);
										crashLine = crashLine + line + "\\n";
									}

									//Sends the crash report and closes the connection
									crashSenderSocket->write(createXML(crashLine,"system.crashreport"));
									crashSenderSocket->close();
									sent = true;
								}
							}

							//closes the file and renames it with "-sent"
							crashReportFile.close();
							rename(crashReport.c_str(),
									(crashReport + "-sent").c_str());
						}
					}
				}
				free(files);
				return sent;
			}

		} catch (ConnectFailed &) {
			cerr << "Connection Failed with the logging server" << endl;
			return false;
		} catch (HostNotFound &) {
			cerr << "Logging Sever Not Found" << endl;
			return false;
		}
	}
}

bool CrashSender::send(){
	return this->run();
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

