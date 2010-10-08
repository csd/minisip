/*
 Copyright (C) 2006  Mikael Magnusson
 
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

/*
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

#include<config.h>

#include<libminisip/config/UserConfig.h>
#include<libmutil/dbg.h>

#include<stdlib.h>
#include<string>
#include<sys/stat.h>
#include<sys/types.h>

using namespace std;

std::string UserConfig::getMiniSIPHomeDirectory(void) {
	struct stat st;
	std::string MiniSIPHomeDirectoryName = string(getenv("HOME")) + "/.minisip";

	if (stat(MiniSIPHomeDirectoryName.c_str(), &st) == 0) {
		if (!S_ISDIR(st.st_mode)) {
			cerr << "[UserConfig] It is a file and not directory" << endl;
			unlink(MiniSIPHomeDirectoryName.c_str());
			if (mkdir(MiniSIPHomeDirectoryName.c_str(), 0777) == -1) {
				cerr << "[UserConfig] Error opening the new directory" << endl;
			}
		}
	} else {
		if (mkdir(MiniSIPHomeDirectoryName.c_str(), 0777) == -1) {
			cerr << "[UserConfig] Error creating the new directory" << endl;
		} else
			cerr << "Directory created successfully" << endl;
	}
	return MiniSIPHomeDirectoryName;
}

std::string UserConfig::getFileName(std::string baseName) {
	return (getMiniSIPHomeDirectory() + string("/") + baseName);
}
