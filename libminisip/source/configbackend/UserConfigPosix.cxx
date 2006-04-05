/*
 *  Copyright (C) 2006  Mikael Magnusson
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

#include<config.h>

#include<libminisip/configbackend/UserConfig.h>
#include<libmutil/dbg.h>

#include<stdlib.h>
#include<string>

using namespace std;

std::string UserConfig::getFileName(std::string baseName)
{
        char *home = NULL;

	home = getenv("HOME");
        if (home){
		return string(home)+ string("/.") + baseName;
	}

	merr << "WARNING: Could not determine home directory"<<end;

	return string("/.") + baseName;
}
