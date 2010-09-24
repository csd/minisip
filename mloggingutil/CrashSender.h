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
#include<libmnetutil/TCPSocket.h>

/*
 * Crash report sender thread class
 */
class CrashSender{

public:
	CrashSender();
	~CrashSender();
	bool run();
	bool send();	
	std::string createXML(std::string message,std::string value);	

private:
	TCPSocket* crashSenderSocket;			//Socket which send the crash report

	std::string crashDirectoryPath;			//Path for the crash report directory
	std::string crashPID;					//Crash PID
	std::string logServerAddress;			//Log server address
	std::string logServerPort;				//Log server port
};

