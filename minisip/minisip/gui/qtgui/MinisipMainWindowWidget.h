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

#ifndef MINISIPMAINWINDOWWIDGET_H
#define MINISIPMAINWINDOWWIDGET_H

#include<config.h>

#include<qapplication.h>
#ifdef OPIE
#include<qpe/qpeapplication.h>
#endif

#include<qstatusbar.h>
#include<qlistview.h>
#include<qtabwidget.h>
#include<qsocket.h>
#include"SettingsDialog.h"
#include"CallDialog.h"



#ifdef OPIE
#include<qcopchannel_qws.h>
#include<opie/odevicebutton.h>
#endif

#include"../Gui.h"
#include<list>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipSMCommand.h>
#include<libmutil/TimeoutProvider.h>


class MinisipMainWindowWidget : public QWidget, public Gui
#ifdef MINISIP_AUTOCALL
		, public TimeoutSubscriberInterface<string>
#endif

{
	Q_OBJECT;
	public:
		MinisipMainWindowWidget(TimeoutProvider<string> *tp, QWidget *parent=0, const char *name=0);

		virtual void setSipSoftPhoneConfiguration(MRef<SipSoftPhoneConfiguration*> sipPhone);
		virtual MRef<SipSoftPhoneConfiguration*> getSipSoftPhoneConfiguration();

		virtual void displayErrorMessage(string s);

		virtual void customEvent(QCustomEvent *e);

		void timeout(string);
		
		void addPhoneBook(string filename);

		void setStatusBar(QStatusBar *bar);

		virtual void handleCommand(CommandString command);
		virtual bool configDialog(MRef<SipSoftPhoneConfiguration *> conf);

//#ifdef OPIE
//		void setApplication(QPEApplication *app);
//#else
		void setApplication(QApplication *app);
//#endif

//		virtual void log(int type, string msg);

		virtual void run();

		void addCallDialog(CallDialog *d);
		void removeCallDialog(string callId);

#ifdef OPIE
		void updateGui();
#endif

	public slots:
			void displaySettings();
			void callPressed();
//			void displayLog();
//			void displayPacketLoss();
//			void displayInternalState();
			void phoneBookSelect();
			void userDoubleClicked(QListViewItem *, const QPoint &, int);
			void readChar();
	private:
		void doCall();
		void createCallDialog(string title, string callId, string from, string secured);
		void createRegisterDialog(string title, string callId, string proxy);
		QHBoxLayout callboxlayout;
		QApplication *qapplication;

		SettingsDialog settingsDialog;

#if (QT_VERSION < 0x030000)
		int tabCount;
#endif

		QTabWidget tabs;
		QWidget	phonebooktabw;
		QVBoxLayout phonebookvlayout;
		QVBoxLayout vlayout;
		list<CallDialog *> calls;
		MRef<SipSoftPhoneConfiguration *>sipphoneconfig;
		QStatusBar *statusBar;
		QListView *listView;
		QLineEdit text;
		TimeoutProvider<string> *timeoutProvider;
#ifdef OPIE
		QSocket update_socket;
		StreamSocket *do_update_socket;
#endif
		
};

class PersonListItem : public QListViewItem {
	public:
		static const int RTTI_VAL;
	//	PersonListItem(QListView *parent, string name, string uri, string status);
		PersonListItem(QListViewItem *parent, string name, string uri, bool online, string location);
		virtual void paintCell(QPainter *p, 
				const QColorGroup &cg,
				int32_t column, 
				int32_t width, 
				int32_t alignment 
				);
		string getUri(){return uri;};

#if (QT_VERSION > 0x030000)
		virtual int rtti();
#endif
	private:
		string name;
		string uri;
		bool online;
		string location;
};

class CommandEvent : public QCustomEvent{
	public:
		CommandEvent(CommandString command);
		CommandString getCommand();
	private:
		CommandString command;
};

#endif
