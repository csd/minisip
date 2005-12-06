#include<config.h>

#include<libmutil/MPlugin.h>
#include<libmutil/Library.h>

#include<sys/types.h>
#include<sys/stat.h>

#ifndef _MSC_VER
#include <unistd.h>
#include<dirent.h>
#endif


using namespace std;

list< MRef<Library *> > MPlugin::libraries;

std::list< MPluginRegistry * > MPlugin::registries;

MPlugin::~MPlugin(){
}
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
#ifdef _MSC_VER
	cerr << "\nUNIMPLEMENTED: MPlugin::loadFromDirectory (not implemented for W32 yet)"<<endl;
	assert(false);
	return -1;
#else
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
			
		lib = Library::open( ( path + "/" + dirEnt->d_name ).c_str() );
		if( lib ){
			entryPoints = MPlugin::getListFromLibrary( lib );

			if( entryPoints ){
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
		
		dirEnt = readdir( dirHandle );

		
	}

	if( closedir( dirHandle ) ){
		// Do something
	}
	
	if( nPlugins > 0 ){
		libraries.push_back( lib );
	}
	p = NULL;
	lib = NULL;


	return nPlugins;
#endif
	
}

MRef<MPlugin *> MPlugin::loadFromLibrary( const string &file, 
					  const string &entryPoint ){
	MRef<Library *> lib = NULL;
	list< MRef<Library *> >::iterator iLib;
	MRef<MPlugin *> p;
	bool newLib = false;

	for( iLib = libraries.begin(); iLib != libraries.end(); iLib ++ ){
		if( (*iLib)->getPath() == file ){
			lib = *iLib;
		}
	}

	/* This library hasn't been opened yet */
	if( !lib ){
		lib = Library::open( file );
		if( lib ){
			newLib = true;
		}
	}

	p =  MPlugin::loadFromLibrary( lib, entryPoint );

	if( p && newLib ){
		libraries.push_back( lib );
	}

	return p;
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
		MRef<MPlugin *> * pp = creatorFunction();
		fprintf( stderr, "pp: %x\n", pp );
		fprintf( stderr, "getName: %s\n", (**pp)->getName().c_str() );
		fprintf( stderr, "getDescription: %s\n", (**pp)->getDescription().c_str() );
		return *pp;
	}

	return NULL;
}

void MPlugin::registerPlugin( MRef<MPlugin *> p ){
	std::list< MPluginRegistry * >::iterator iReg;


	for( iReg = registries.begin(); iReg != registries.end(); iReg ++ ){
		if( (*iReg)->getPluginType() == p->getPluginType() ){
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
