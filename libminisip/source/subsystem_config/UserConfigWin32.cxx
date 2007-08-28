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

#include<shlobj.h>
#include<io.h>
#include<sys/stat.h>
#include<direct.h>

using namespace std;

std::string UserConfig::getFileName(std::string baseName)
{
#ifndef _WIN32_WCE
	char path[MAX_PATH] = "";
	HRESULT res = SHGetFolderPathA(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, path);

	// ANSI only
	if( res != S_FALSE ){
		// TODO create directory.
		string orgPath = string( path ) + string( "\\MinisipOrg" );
		string appPath = orgPath + string( "\\Minisip" );

		struct stat st;
		memset( &st, 0, sizeof( st ) );
		int res = stat( orgPath.c_str(), &st );

		if( res < 0 ){
			mkdir( orgPath.c_str() );
		}

		res = stat( appPath.c_str(), &st );

		if( res < 0 ){
			mkdir( appPath.c_str() );
		}

		return appPath + string( "\\" ) + baseName;
	}
#endif

	return string( "c:\\" ) + baseName;
}
