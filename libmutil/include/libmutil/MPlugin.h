/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien

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

#ifndef MPLUGIN_H
#define MPLUGIN_H

#include<string>
#include<stdint.h>
#include<libmutil/MemObject.h>

class Library;

/**
 * Implements a dynamically loadable plugins support.
 * @author Johan Bilien, jobi@via.ecp.fr
 */

class MPlugin{
	public:
		/**
		 * @returns a short name of the plugin.
		 */
		virtual std::string getName();

		/**
		 * @returns the version of the plugin.
		 */
		virtual uint32_t getVersion();
		
		/**
		 * @returns a more detailed description of the
		 * plugin.
		 */
		virtual std::string getDescription();

		/**
		 * @param file File name/path to a shared library
		 * @param entryPoint Symbol name to a MPlugin::creator
		 * function
		 * @returns a reference to the plugin, or NULL if the
		 * opening failed.
		 */
		static MPlugin * loadFromLibrary( 
				std::string file, 
				std::string entryPoint );


		/**
		 * \typedef MPlugin (* creator)();
		 * \brief MPlugin object factory, entry point in the
		 * shared library.
		 **/
		typedef MPlugin * (* creator)();
		
	private:
		// List of the already opened libraries for plugins,
		// to avoid loading them twice
		static std::list< MRef<Library *> > libraries;

		
};



#endif
