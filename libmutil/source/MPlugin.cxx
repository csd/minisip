#include<libmutil/MPlugin.h>
#include<libmutil/Library.h>

using namespace std;

MPlugin * MPlugin::loadFromLibrary( string file, string entryPoint ){
	MRef<Library *> lib = NULL;
	list< MRef<Library *> >::iterator iLib;
	MPlugin::creator creatorFunction;

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
