/*
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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include"qtguistarter.h"
#ifdef OPIE
#include<qpe/qpeapplication.h>
#else
#include<qapplication.h>
#endif
#include<qpushbutton.h>
#include<qvbox.h>
#include<qmenubar.h>
#include<qpopupmenu.h>
#if (QT_VERSION > 0x030000)
#include<qkeysequence.h>
#endif
#include<qmainwindow.h>
#include<qstatusbar.h>
#include"MinisipMainWindowWidget.h"
//#include"../../../util/ConfigFile.h"
#include<libmutil/TimeoutProvider.h>       
//#include"../../../icon.xpm"



MinisipMainWindowWidget *guiFactory(int32_t argc, char **argv, TimeoutProvider<string> *tp){
#ifdef OPIE
	QPEApplication *a = new QPEApplication(argc, argv);
#else
	QApplication *a = new QApplication(argc, argv);
#endif
	MinisipMainWindowWidget *m = new MinisipMainWindowWidget(tp);
	a->setMainWidget(m);
	return m;
}

int32_t qtguirun(MinisipMainWindowWidget *m){
	QMainWindow *mw = new QMainWindow();
	QPoint p(0,0);
	m->reparent(mw,p);
	m->setApplication(qApp);
	
	QPopupMenu *fileMenu = new QPopupMenu();
	fileMenu->insertItem("&Settings...", m, SLOT(displaySettings()), Qt::CTRL + Qt::Key_S);

	fileMenu->insertSeparator();
	fileMenu->insertItem("E&xit", qApp, SLOT(quit()), Qt::CTRL + Qt::Key_X);
	
	QMenuBar *bar = new QMenuBar(mw);
	bar->insertItem("&File",fileMenu);
#if 0
#ifdef DEBUG_OUTPUT
	QPopupMenu *viewMenu = new QPopupMenu();
	viewMenu->insertItem("&View debug information...", m, SLOT(displayLog()), Qt::CTRL + Qt::Key_V);
	viewMenu->insertItem("&Packet loss visualization...", m, SLOT(displayPacketLoss()), Qt::CTRL + Qt::Key_P);
	int32_t id=viewMenu->insertItem("&Jitter buffer visualization...", m, SLOT(displayLog()), Qt::CTRL + Qt::Key_V);
	viewMenu->setItemEnabled(id,false);
	viewMenu->insertItem("&Internal State information...", m, SLOT(displayInternalState()), Qt::CTRL + Qt::Key_I);
	
	bar->insertItem("&View",viewMenu);
#endif
#endif
	mw->setCentralWidget(m);
	m->setStatusBar(mw->statusBar());
	//mw->setIcon(QPixmap((const char **)minisip_xpm));
	mw->setCaption( "minisip" );
#ifdef OPIE
	((QPEApplication *)qApp)->showMainWidget(mw);
#else
	mw->show();
#endif
	fprintf( stderr, "desktopSettings: %d\n", qApp->desktopSettingsAware()); //FIXME: Jobi, what? --Erik
//	mw->statusBar()->message("Ready");
	return qApp->exec();
}

