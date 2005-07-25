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
class MPluginRegistry;

/**
 * Implements a dynamically loadable plugins support.
 * @author Johan Bilien, jobi@via.ecp.fr
 */

class MPlugin : public virtual MObject{
	friend class MPluginRegistry;
	public:
		/**
		 * @returns a short name of the plugin.
		 */
		virtual std::string getName()=0;

		/**
		 * @returns the version of the plugin.
		 */
		virtual uint32_t getVersion()=0;
		
		/**
		 * @returns a more detailed description of the
		 * plugin.
		 */
		virtual std::string getDescription()=0;

		/**
		 * @returns the plugin type.
		 **/
		virtual std::string getPluginType()=0;

		/**
		 * @param path Path to the directory where the plugins
		 * to load are located
		 * @returns the number of plugins loaded, or -1 if
		 * an error occured.
		 **/
		static int32_t loadFromDirectory( const std::string &path );
		
		/**
		 * @param file File name/path to a shared library
		 * @param entryPoint Symbol name to a MPlugin::creator
		 * function
		 * @returns a reference to the plugin, or NULL if the
		 * opening failed.
		 */
		static MRef<MPlugin *> loadFromLibrary( 
				const std::string &file, 
				const std::string &entryPoint );
		
		/**
		 * @param lib An previously dlopened shared library object
		 * @param entryPoint Symbol name to a MPlugin::creator
		 * function
		 * @returns a reference to the plugin, or NULL if the
		 * opening failed.
		 */
		static MRef<MPlugin *> loadFromLibrary( 
				MRef<Library *> lib, 
				const std::string &entryPoint );

		/**
		 * @param lib The library from which to get the list
		 * of plugin entry points.
		 * @returns a list of entry points to plugins in the
		 * given library
		 **/
		static std::list< std::string > * getListFromLibrary(
				MRef<Library *> lib );

		/**
		 * @param p A reference to the plugin to register.
		 **/
		static void registerPlugin( MRef<MPlugin *> p );
		
		/**
		 * \typedef MPlugin (* lister)();
		 * \brief Entry point in a library used to list
		 * the available plugin entry points in the
		 * library.
		 **/
		typedef std::list<std::string> * (* lister)();
		
		/**
		 * \typedef MPlugin (* creator)();
		 * \brief MPlugin object factory, entry point in the
		 * shared library.
		 **/
		typedef MRef<MPlugin *> (* creator)();

		/**
		 * @returns the MemObject type.
		 **/
		virtual std::string getMemObjectType()=0;
		
	private:
		// List of the already opened libraries for plugins,
		// to avoid loading them twice
		static std::list< MRef<Library *> > libraries;

		// List of the registered plugin registries
		static std::list< MPluginRegistry * > registries;

		// Plugin type, for matching with a MPluginRegistry
		std::string pluginType;
		
};

class MPluginRegistry {
	public:
		MPluginRegistry();
		virtual ~MPluginRegistry();

		/*
		 * @returns the type of plugins that this registry holds.
		 **/
		virtual std::string getPluginType()=0;

		void registerPlugin( MRef<MPlugin *> p );
	private:
		std::list< MRef<MPlugin *> > plugins;

	
};



#endif
