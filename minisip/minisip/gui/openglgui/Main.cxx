/*
 *  Copyright (C) 2009 Erik Eliasson
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
 * Authors:     Erik Eliasson <ere@kth.se>
 * 		Mikael Magnusson <mikma@users.sourceforge.net>
 *           
 */

#include<config.h>

#include<fstream>

#include"OpenGlGui.h"
#include<libminisip/Minisip.h>

using namespace std;

int main( int argc, char *argv[] )
{
#ifdef DEBUG_OUTPUT
	setThreadName("main");
#endif
	merr.setPrintStreamName(true);
	mout.setPrintStreamName(true);
	mdbg.setPrintStreamName(true);
	bool fullscreen=false;

	if (argv && argv[1] && argv[1][0]=='-' && argv[1][1]=='f'){
		fullscreen=true;
		int i=1;
		do{
			argv[i]=argv[i+1];
			i++;
		}while(argv[i]);
	}

	cerr << endl << "Starting MiniSIP TextUI ... welcome!" << endl << endl;
	setupDefaultSignalHandling(); //Signal handlers are created for all 
				      //threads created with libmutil/Thread.h
				      //For the main thread we have to
				      //install them
	
	cerr << "Creating OpenGlGui"<< endl;
	Minisip::doLoadPlugins(argv);
	MRef<OpenGlGui*> gui = new OpenGlGui(fullscreen);

	Minisip minisip( *gui, argc, argv );
	gui->start();
	cerr <<"EEEE: :::::::::::::::: running minisip.startSip"<<endl;
	if( minisip.startSip() > 0 ) {
		cerr <<"EEEE: :::::::::::::::: running minisip.startDebugger"<<endl;
		
		minisip.startDebugger();
		cerr <<"EEEE: :::::::::::::::: running minisip.join"<<endl;
		//minisip.join();
		
		gui->join();
//		Thread::msleep(5000);
	} else {
		cerr << endl << "ERROR while starting SIP!" << endl << endl;
	}

	cerr <<"EEEE: ,,,,,,,,,,,,,,,,,,,,,,,, running minisip.exit"<<endl;
	minisip.exit();
	Thread::msleep(1000);
	return 0;
}

