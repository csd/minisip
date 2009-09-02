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

	cerr << endl << "Starting MiniSIP TextUI ... welcome!" << endl << endl;
	setupDefaultSignalHandling(); //Signal handlers are created for all 
				      //threads created with libmutil/Thread.h
				      //For the main thread we have to
				      //install them
	
	cerr << "Creating OpenGlGui"<< endl;
	Minisip::doLoadPlugins(argv);
	MRef<OpenGlGui*> gui = new OpenGlGui();
	gui->start();

	Thread::msleep(3000);

	Minisip minisip( *gui, argc, argv );
	if( minisip.startSip() > 0 ) {
//		gui->displayMessage("");
//		gui->displayMessage("To auto-complete, press <tab>. For a list of commands, press <tab>.", MinisipTextUI::bold);
//		gui->displayMessage("");

		minisip.join();
	} else {
		cerr << endl << "ERROR while starting SIP!" << endl << endl;
	}
	minisip.exit();
	return 0;
}

