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

#include <libmutil/libmutil_config.h>

#include<string>
#include<libmutil/mtypes.h>
#include<libmutil/MemObject.h>
#include<libmutil/Library.h>

class MPluginRegistry;

/**
 * Implements a dynamically loadable plugins support.
 *
 *
 * 
 * @author Johan Bilien, jobi@via.ecp.fr
 * @author Erik Eliasson, eliasson@it.kth.se 
 */

class LIBMUTIL_API MPlugin : public virtual MObject{
	public:	
		virtual ~MPlugin();
		/**
		 * @returns a short name of the plugin.
		 */
		virtual std::string getName()const=0;

		/**
		 * @returns the version of the plugin.
		 */
		virtual uint32_t getVersion()const=0;
		
		/**
		 * @returns a more detailed description of the
		 * plugin.
		 */
		virtual std::string getDescription()const=0;

		/**
		 * @returns the plugin type.
		 **/
		virtual std::string getPluginType()const=0;

		/**
		 * \typedef MPlugin (* lister)();
		 * \brief Entry point in a library used to list
		 * the available plugin entry points in the
		 * library.
		 **/
		typedef std::list<std::string> * (* lister)(MRef<Library*> lib);
		
		/**
		 * \typedef MPlugin (* creator)();
		 * \brief MPlugin object factory, entry point in the
		 * shared library.
		 **/
		typedef MPlugin * (* creator)(MRef<Library*> lib);

		/**
		 * @returns the MemObject type.
		 **/
		virtual std::string getMemObjectType() const =0;

	protected:
		MPlugin(MRef<Library*> lib);
		MPlugin();

	private:
		MRef<Library *> library;
};

class LIBMUTIL_API MPluginManager: public MObject{
	public:
		virtual ~MPluginManager();

		static MRef<MPluginManager*> getInstance();

		/**
		 * @param path Path to the directory where the plugins
		 * to load are located
		 * @returns the number of plugins loaded, or -1 if
		 * an error occured.
		 **/
		int32_t loadFromDirectory( const std::string &path );
		
		/**
		 * @param file File name/path to a shared library
		 * @param entryPoint Symbol name to a MPlugin::creator
		 * function
		 * @returns a reference to the plugin, or NULL if the
		 * opening failed.
		 */
		MRef<MPlugin *> loadFromLibrary( 
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
		 * @param filename the file name of the library
		 * containing the the plugins to load
		 * @returns the number of plugins loaded, or -1 if an
		 * error occured.
		 **/
		int32_t loadFromFile( const std::string &filename );

		/**
		 * @param lib The library from which to get the list
		 * of plugin entry points.
		 * @returns a list of entry points to plugins in the
		 * given library
		 **/
		std::list< std::string > * getListFromLibrary(
				MRef<Library *> lib );

		/**
		 * @param p A reference to the plugin to register.
		 **/
		bool registerPlugin( MRef<MPlugin *> p );

		void addRegistry( MPluginRegistry * registry );
		void removeRegistry( MPluginRegistry * registry );

		/**
		 * Set directory search path used by loadFromLibrary,
		 * and loadFromFile.
		 * Win32: On pre-WXPSP1 this method changes
		 *        the applications current working directory.
		 *        
		 **/
		bool setSearchPath( const std::string &searchPath );

	protected:
		MPluginManager();

	private:
		static MRef<MPluginManager*> instance;
		
		// List of the already opened libraries for plugins,
		// to avoid loading them twice
		std::list< MRef<Library *> > libraries;

		// List of the registered plugin registries
		std::list< MPluginRegistry * > registries;

};

class LIBMUTIL_API MPluginRegistry: public MObject {
	public:
		typedef std::list< MRef<MPlugin*> >::const_iterator const_iterator;

		MPluginRegistry();
		virtual ~MPluginRegistry();

		/*
		 * @returns the type of plugins that this registry holds.
		 **/
		virtual std::string getPluginType()=0;

		virtual void registerPlugin( MRef<MPlugin *> p );

		const_iterator begin() const;
		const_iterator end() const;

	protected:
		virtual MRef<MPlugin*> findPlugin( std::string name ) const;

		std::list< MRef<MPlugin *> > plugins;

	private:
		MRef<MPluginManager*> manager;
};

#endif
