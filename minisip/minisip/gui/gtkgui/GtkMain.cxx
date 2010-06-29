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

#include<fstream>

#include"GtkMainUI.h"
#include<libminisip/Minisip.h>
#include<libminisip/config/UserConfig.h>

#include<libmnetutil/TCPSocket.h>

using namespace std;

static bool redirectOutput(const char *logName){
	FILE *ret;
	ret=freopen(logName, "a", stdout);
	massert(ret!=NULL);
	ret=freopen(logName, "a", stderr);
	massert(ret!=NULL);
 	cerr << "Created log file" << endl;

	return true;
}

int main( int argc, char *argv[] )
{
	merr.setPrintStreamName(true);
	mout.setPrintStreamName(true);
	mdbg.setPrintStreamName(true);

#if defined(DEBUG_OUTPUT) || !defined(WIN32)
	cerr << endl << "Starting MiniSIP GTK ... welcome!" << endl << endl;
	setThreadName("main");
#endif

	setupDefaultSignalHandling(); //Signal handlers are created for all 
				      //threads created with libmutil/Thread.h
				      //For the main thread we have to
				      //install them
#ifndef DEBUG_OUTPUT
	redirectOutput( UserConfig::getFileName( "minisip.log" ).c_str() );
#endif
	
	cerr << "Creating GTK GUI"<< endl;
	MRef<Gui *> gui = GtkMainUI::create( argc, argv );

// 	cerr << "Minisip: gtk 1" << endl;
// 	LogEntry::handler = (GtkMainUI *)*gui;
// 	cerr << "Minisip: gtk 2" << endl;

	LogEntry::handler = (GtkMainUI *)*gui;

	Minisip minisip( gui, argc, argv );

	if( minisip.startSip() > 0 ) {
#ifdef DEBUG_OUTPUT
		minisip.startDebugger();
#else
		//in non-debug mode, send merr to the gui
		merr.setExternalHandler( dynamic_cast<DbgHandler *>( *gui ) );
		mout.setExternalHandler( dynamic_cast<DbgHandler *>( *gui ) );
		mdbg.setExternalHandler( dynamic_cast<DbgHandler *>( *gui ) );
#endif	// DEBUG_OUTPUT

		minisip.runGui();

#ifndef DEBUG_OUTPUT
		merr.setExternalHandler( NULL );
		mout.setExternalHandler( NULL );
		mdbg.setExternalHandler( NULL );
#endif
	} else {
		cerr << endl << "ERROR while starting SIP!" << endl << endl;
	}
	minisip.exit();

}
