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

#ifndef CALLDIALOG_H 
#define CALLDIALOG_H 

#include<config.h>

#ifdef EMBEDDED_CALLDIALOGS
#include<qwidget.h>
#else
#include<qdialog.h>
#endif

#include<qlayout.h>
#include"../Gui.h"
#include"../../Bell.h"
#include<libmutil/TimeoutProvider.h>
#if (QT_VERSION > 0x030000)
#include<qtextedit.h>
#endif
#include<qlineedit.h>
#include<qcheckbox.h>
#include<qpushbutton.h>
#include<qlayout.h>
#include<qlabel.h>
#include<libmutil/CommandString.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

class MinisipMainWindowWidget;

#ifdef EMBEDDED_CALLDIALOGS
#define CALLDIALOG_SUPERCLASS QWidget
#else
#define CALLDIALOG_SUPERCLASS QDialog
#endif

class CallDialog : public CALLDIALOG_SUPERCLASS, public TimeoutSubscriberInterface<string>{
	Q_OBJECT;
	public:
		CallDialog(string callId, MinisipMainWindowWidget *mainWidget, TimeoutProvider<string> *tp);
		CallDialog(string callId, MinisipMainWindowWidget *mainWidget, TimeoutProvider<string> *tp, string incomingFrom, string secured);

		virtual bool handleCommand(CommandString command);
		string getCallId();
		virtual void timeout(string c);

#ifdef EMBEDDED_CALLDIALOGS
		void done(int){
#ifdef DEBUG_OUTPUT
			cerr << "done():: done called - UNIMPLEMENTED"<< endl;
#endif
		}
#endif

	public slots:
		virtual void reject(){rejectClicked();};
		virtual void accept(){acceptClicked();};
		virtual void acceptClicked();
		virtual void rejectClicked();
		void keyPressEvent(QKeyEvent * e);
		
	protected:
		void hideAcceptButton();
		string callId;
		MinisipMainWindowWidget *mainWidget;
		QVBoxLayout vlayout;
		QHBoxLayout buttonlayout;
		QLabel status;
		QLabel security_status;
		QCheckBox checkRecord;
		QPushButton acceptButton;
		QPushButton rejectButton;
		TimeoutProvider<string> *timeoutProvider;
		Bell *bell;
		
};

class RegCallDialog : public CallDialog{
	Q_OBJECT;
	public:
		RegCallDialog(string callid, MinisipMainWindowWidget *mainWidget, TimeoutProvider<string> *tp, string proxy/*, string authenticated*/);

		virtual bool handleCommand(CommandString command);
//		virtual void timeout(string c){handleCommand(CommandString(callId,c));};

	public slots:
		virtual void rejectClicked();
	
	private:
		QGridLayout userPassLayout;
		QLabel usernameLabel;
		QLineEdit usernameEdit;
		QLabel passwordLabel;
		QLineEdit passwordEdit;
		QCheckBox saveCheck;
};

#endif
