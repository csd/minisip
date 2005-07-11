#include<libmutil/MPlugin.h>
#include<libmutil/Library.h>

#include<sys/types.h>
#include<sys/stat.h>
#include <unistd.h>

#include<dirent.h>

using namespace std;

list< MRef<Library *> > MPlugin::libraries;
std::list< MPluginRegistry * > MPlugin::registries;

string MPlugin::getMemObjectType(){
	return "MPlugin";
}

list<string> * MPlugin::getListFromLibrary( MRef<Library *> lib ){
	MPlugin::lister listerFunction;

	listerFunction = (MPlugin::lister)lib->getFunctionPtr( "listPlugins" );

	if( listerFunction ){
		return listerFunction();
	}

	return NULL;
}

int32_t MPlugin::loadFromDirectory( const string &path ){
	struct stat dirStat;
	DIR * dirHandle;
	struct dirent * dirEnt;
	MRef<Library *> lib;
	list<string> * entryPoints;
	list<string>::iterator iEP;
	MRef<MPlugin *> p;
	int32_t nPlugins = 0;

	if( stat( path.c_str(), &dirStat ) == -1 ){
		return -1;
	}

	if( ! S_ISDIR( dirStat.st_mode ) ){
		return -1;
	}

	dirHandle = opendir( path.c_str() );

	if( !dirHandle ){
		return -1;
	}

	dirEnt = readdir( dirHandle );

	while( dirEnt ){
			
		dirEnt = readdir( dirHandle );
		lib = Library::open( dirEnt->d_name );
		if( lib ){
			entryPoints = MPlugin::getListFromLibrary( lib );

			for( iEP = entryPoints->begin(); 
					iEP!= entryPoints->end();
					iEP ++ ){
				p = MPlugin::loadFromLibrary( lib, *iEP );
				if( p ){
					MPlugin::registerPlugin( p );
					nPlugins ++;
				}
			}
		}
		
	}

	if( closedir( dirHandle ) ){
		// Do something
	}

	return nPlugins;

}

MRef<MPlugin *> MPlugin::loadFromLibrary( const string &file, 
					  const string &entryPoint ){
	MRef<Library *> lib = NULL;
	list< MRef<Library *> >::iterator iLib;

	for( iLib = libraries.begin(); iLib != libraries.end(); iLib ++ ){
		if( (*iLib)->getPath() == file ){
			lib = *iLib;
		}
	}

	/* This library hasn't been opened yet */
	if( !lib ){
		lib = Library::open( file );
		if( lib ){
			libraries.push_back( lib );
		}
	}

	return MPlugin::loadFromLibrary( lib, entryPoint );
}


MRef<MPlugin *> MPlugin::loadFromLibrary( MRef<Library *> lib, 
					  const string &entryPoint ){
	MPlugin::creator creatorFunction;

	if( !lib ){
		/* This library doen't exist or couldn't be opened */
		return NULL;
	}

	creatorFunction = (MPlugin::creator)(lib->Library::getFunctionPtr(
				entryPoint ));

	if( creatorFunction ){
		return creatorFunction();
	}

	return NULL;
}

void MPlugin::registerPlugin( MRef<MPlugin *> p ){
	std::list< MPluginRegistry * >::iterator iReg;

	for( iReg = registries.begin(); iReg != registries.end(); iReg ++ ){
		if( (*iReg)->getPluginType() == p->pluginType ){
			(*iReg)->registerPlugin( p );
		}
	}
}

void MPluginRegistry::registerPlugin( MRef<MPlugin *> p ){
	plugins.push_back( p );
}

MPluginRegistry::MPluginRegistry(){
	MPlugin::registries.push_back( this );
}

MPluginRegistry::~MPluginRegistry(){
	MPlugin::registries.remove( this );
}
