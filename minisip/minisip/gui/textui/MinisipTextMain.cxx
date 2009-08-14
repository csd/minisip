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

#include"MinisipTextUI.h"
#include<libminisip/Minisip.h>
#include<libminisip/gui/ConsoleDebugger.h>

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
	
	cerr << "Creating TextUI"<< endl;
	MRef<MinisipTextUI *> gui = new MinisipTextUI();
	cerr << "Minisip: 1" << endl;
// 	LogEntry::handler = (GtkMainUI *)*gui;
	cerr << "Minisip: 2" << endl;

	merr.setExternalHandler( dynamic_cast<DbgHandler *>( *gui ) );
	mout.setExternalHandler( dynamic_cast<DbgHandler *>( *gui ) );
	mdbg.setExternalHandler( dynamic_cast<DbgHandler *>( *gui ) );

	LogEntry::handler = NULL;

	Minisip minisip( *gui, argc, argv );
	if( minisip.startSip() > 0 ) {
		gui->displayMessage("");
		gui->displayMessage("To auto-complete, press <tab>. For a list of commands, press <tab>.", MinisipTextUI::bold);
		gui->displayMessage("");

		minisip.runGui();
	} else {
		cerr << endl << "ERROR while starting SIP!" << endl << endl;
	}
	minisip.exit();
	return 0;
}

