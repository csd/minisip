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

#include"CallDialog.h"
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipMessageTransport.h>
#include"../../../sip/SipSoftPhoneConfiguration.h"
#include"../../../sip/DefaultDialogHandler.h"
#include"MinisipMainWindowWidget.h"
#include<qmessagebox.h>

#define config mainWidget->getSipSoftPhoneConfiguration()

#ifdef EMBEDDED_CALLDIALOGS
CallDialog::CallDialog(string callId, MinisipMainWindowWidget *main, TimeoutProvider<string> *tp): QWidget(0,"Call"), callId(callId), mainWidget(main),
#else
CallDialog::CallDialog(string callId, MinisipMainWindowWidget *main, TimeoutProvider<string> *tp): QDialog(0,"Call",FALSE, WDestructiveClose), callId(callId), mainWidget(main),
#endif
		vlayout(this),
		buttonlayout(),
		status("Connecting...", this), 
		security_status("", this),
		checkRecord("Record call", this),
		acceptButton("OK", this),
		rejectButton("Hang up", this),
		timeoutProvider(tp),
		bell(NULL)
{
	vlayout.addWidget(&status);
	status.setAlignment(AlignCenter);
	
	vlayout.addWidget(&security_status);
	security_status.setAlignment(AlignCenter);
	
	vlayout.addWidget(&checkRecord);
	buttonlayout.addWidget(&acceptButton);
	buttonlayout.addWidget(&rejectButton);
	vlayout.addLayout(&buttonlayout);
	connect(&acceptButton,SIGNAL(clicked()), this, SLOT(acceptClicked()) );
	connect(&rejectButton,SIGNAL(clicked()), this, SLOT(rejectClicked()) );
	
	hideAcceptButton();

	//status.setMinimumWidth(250);
	bell=NULL;
}
#ifdef EMBEDDED_CALLDIALOGS
CallDialog::CallDialog(string callId, MinisipMainWindowWidget *main, TimeoutProvider<string> *tp,string incomingFrom, string secured): QWidget(0,"Call"), callId(callId), mainWidget(main),
#else
CallDialog::CallDialog(string callId, MinisipMainWindowWidget *main, TimeoutProvider<string> *tp,string incomingFrom, string secured): QDialog(0,"Call",FALSE,WDestructiveClose), callId(callId), mainWidget(main),
#endif
		vlayout(this),
		buttonlayout(),
		status("Connecting...", this), 
		security_status("The call is ", this),
		checkRecord("Record call", this),
		acceptButton("OK", this),
		rejectButton("Hang up", this),
		timeoutProvider(tp)
{
	status.setText(("Incoming call\nfrom "+ incomingFrom).c_str());
	bell = new Bell(timeoutProvider);
        if( bell != NULL ){ 
	        bell->start();
        }
	
	vlayout.addWidget(&status);
	status.setAlignment(AlignCenter);

	security_status.setText(("The call is <b>"+secured+"</b>").c_str());
	vlayout.addWidget(&security_status);
	security_status.setAlignment(AlignCenter);

	vlayout.addWidget(&checkRecord);

	buttonlayout.addWidget(&acceptButton);
	buttonlayout.addWidget(&rejectButton);
	vlayout.addLayout(&buttonlayout);
	connect(&acceptButton,SIGNAL(clicked()), this, SLOT(acceptClicked()) );
	connect(&rejectButton,SIGNAL(clicked()), this, SLOT(rejectClicked()) );

}


void CallDialog::hideAcceptButton(){
	acceptButton.hide();
}

string CallDialog::getCallId(){
	return callId;
}

void CallDialog::timeout(string c){
	mainWidget->handleCommand(CommandString(callId,c));
}

void CallDialog::acceptClicked(){
	CommandString accept(callId, SipCommandString::accept_invite);
//        SipSMCommand cmd(accept,SipSMCommand::remote, SipSMCommand::TU);
	mainWidget->getCallback()->guicb_handleCommand(/*cmd*/ accept);

        if( bell != NULL ){
	        bell->stop();
	        delete bell;
	        bell = NULL;
        }

}

void CallDialog::rejectClicked(){
//	cerr << "CallDialog::rejectClicked: clicked"<< endl;
	if (rejectButton.text()=="Close"){
		timeoutProvider->cancel_request(this, SipCommandString::close_window);
		mainWidget->removeCallDialog(callId);
		done(0);
		return;
	}
	if (rejectButton.text()=="Hang up"){
		mainWidget->removeCallDialog(callId);
//		if (status.text()=="Ringing..."){
//			SMCommand cncl(callId, SMCommand::cancel);
//			mainWidget->log(LOG_INFO, "Sending cancel command for call id " +callId);
//			mainWidget->getSipPhone()->doCommand(cncl);
//
//		}else{
		CommandString hup(callId, SipCommandString::hang_up);
//		mainWidget->log(LOG_INFO, "Sending hang up command for call id " +callId);
                //SipSMCommand cmd(hup, SipSMCommand::remote, SipSMCommand::TU);
		mainWidget->getCallback()->guicb_handleCommand(/*cmd*/hup);

		if( bell != NULL ){
			bell->stop();
			delete bell;
			bell = NULL;
		}
//		}
		done(0);
		return;
	}

	/* Moved to Accept button
	
	if (rejectButton.text()=="Answer"){
		CommandString accept(callId, SipCommandString::accept_invite);
		mainWidget->getSipPhone()->enqueueCommand(SipSMCommand(accept,SipSMCommand::remote, SipSMCommand::TU));
		cerr <<"stopping bell..."<< endl;
		bell->stop();
		cerr << "deleteing bell"<< endl;
		delete bell;
	}*/
}

bool CallDialog::handleCommand(CommandString command){
	if (callId == command.getDestinationId()){
		if (command.getOp()==SipCommandString::remote_user_not_found){
			hideAcceptButton();
			status.setText("<b>User not found</b>");
			checkRecord.setChecked(false);
			checkRecord.setEnabled(false);
			rejectButton.setText("Close");
//			repaint();
//			mainWidget->removeCallDialog(callId); 	// Dialog will still exist until close is 
								// clicked - just not receive any SIP commands
		}
		if (command.getOp()==SipCommandString::invite_ok){
			string who;
			if (command.getParam().length()>0)
				who = " with " + command.getParam();
			
			status.setText(("In call"+who).c_str());
			
			security_status.setText(("The call is "+command.getParam2()).c_str());
			hideAcceptButton();
			rejectButton.setText("Hang up");
//			repaint();
			
		}
		if (command.getOp()==SipCommandString::authentication_failed){
			status.setText("<b>Authentication failed</b>");
			checkRecord.setChecked(false);
			checkRecord.setEnabled(false);
			rejectButton.setText("Close");
			//repaint();
			timeoutProvider->request_timeout(5000, this, SipCommandString::close_window);
//			mainWidget->removeCallDialog(callId); 	// Dialog will still exist until close is 
								// clicked - just not receive any SIP commands
		}
		
		if (command.getOp()==SipCommandString::invite_no_reply){
			status.setText("No reply");
			checkRecord.setChecked(false);
			checkRecord.setEnabled(false);
			hideAcceptButton();
			rejectButton.setText("Close");
//			repaint();
			timeoutProvider->request_timeout(5000, this, SipCommandString::close_window);
//			mainWidget->removeCallDialog(callId); 	// Dialog will still exist until close is 
								// clicked - just not receive any SIP commands
		}
		
		if (command.getOp()==SipCommandString::transport_error){
			status.setText("The call failed due to a network error");
			checkRecord.setChecked(false);
			checkRecord.setEnabled(false);
			rejectButton.setText("Close");
//			repaint();
			timeoutProvider->request_timeout(5000, this, SipCommandString::close_window);
//			mainWidget->removeCallDialog(callId); 	// Dialog will still exist until close is 
								// clicked - just not receive any SIP commands
		}
		if (command.getOp()==SipCommandString::remote_hang_up){
			status.setText("Call ended");
//			status.repaint();
//			checkRecord.setChecked(false);
//			checkRecord.setEnabled(false);
			security_status.setText("");
			rejectButton.setText("Close");
//			repaint();
			timeoutProvider->request_timeout(5000, this, SipCommandString::close_window);
//			mainWidget->removeCallDialog(callId); 	// Dialog will still exist until close is 
								// clicked - just not receive any SIP commands
		}
		
		if (command.getOp()==SipCommandString::remote_unacceptable){
			status.setText("Remote side could not handle the call");
//			checkRecord.setChecked(false);
//			checkRecord.setEnabled(false);
			security_status.setText("Check your authentication settings.");
			rejectButton.setText("Close");
//			repaint();
			timeoutProvider->request_timeout(5000, this, SipCommandString::close_window);
//			mainWidget->removeCallDialog(callId); 	// Dialog will still exist until close is 
								// clicked - just not receive any SIP commands
		}
		if (command.getOp()==SipCommandString::remote_reject){
			status.setText("Remote side rejected the call");
//			checkRecord.setChecked(false);
//			checkRecord.setEnabled(false);
			security_status.setText("");
			rejectButton.setText("Close");
//			repaint();
			timeoutProvider->request_timeout(5000, this, SipCommandString::close_window);
//			mainWidget->removeCallDialog(callId); 	// Dialog will still exist until close is 
								// clicked - just not receive any SIP commands
		}
		if (command.getOp()==SipCommandString::remote_cancelled_invite){
			status.setText("Remote side cancelled the call");
			checkRecord.setChecked(false);
			checkRecord.setEnabled(false);
			rejectButton.setText("Close");
			security_status.setText("");
			hideAcceptButton();
			/* Stop the bell */
                        if( bell != NULL ){
			        bell->stop();
			        delete bell;
			        bell = NULL;
                        }
//			repaint();
			timeoutProvider->request_timeout(5000, this, SipCommandString::close_window);
//			mainWidget->removeCallDialog(callId); 	// Dialog will still exist until close is 
		}

		if (command.getOp()==SipCommandString::remote_ringing){
			status.setText("Ringing...");
//			checkRecord.setChecked(false);
//			checkRecord.setEnabled(false);
			rejectButton.setText("Hang up");
//			repaint();
		}
		
		if (command.getOp()==SipCommandString::temp_unavail){
			status.setText("The user is reported temporary unavailable");
			timeoutProvider->request_timeout(5000, this, SipCommandString::close_window);
			rejectButton.setText("Close");
//			repaint();
		}
		
		if (command.getOp()==SipCommandString::transport_error){
			status.setText("The user could not be contacted (transmisison error)");
			rejectButton.setText("Close");
//			repaint();
		}


		if (command.getOp()==SipCommandString::cancel_ok){
			status.setText("Call canceled");
			timeoutProvider->request_timeout(5000, this, SipCommandString::close_window);
			rejectButton.setText("Close");
//			repaint();
		}

		if (command.getOp()==SipCommandString::security_failed){
			/*QMessageBox * dialog = new QMessageBox("Switch to insecure call?", "A secured call could not be established.\nThe person you are trying to reach does probably not support\nsecurity. Do you want to go on with an insecured call,\nor cancel the call?",  QMessageBox::Warning,  QMessageBox::Ok,  QMessageBox::Cancel,  QMessageBox::NoButton, this);
			CommandString * resp;
			if (dialog->exec() == QMessageBox::Ok)
				resp = new SipCommandString(callId, SipCommandString::accept_insecure);
			else
				resp = new CommandString(callId, SipCommandString::reject_insecure);
			mainWidget->getSipPhone()->enqueueCommand(SipSMCommand(*resp, SipSMCommand::remote, SipSMCommand::TU));
			*/
			status.setText("Security is not handled by the receiver");
//			status.repaint();
			security_status.setText("");
//			security_status.repaint();
			rejectButton.setText( "Close" );
			//cancel.repaint();
			
			timeoutProvider->request_timeout(5000, this, SipCommandString::close_window);
		}

		if(command.getOp() == SipCommandString::close_window){
			rejectClicked();
		}
		return true;
	}

	return false;
}

void CallDialog::keyPressEvent(QKeyEvent * e){
	switch(e->key()){
		case Key_F12:
			rejectClicked();
			break;
		default:
			fprintf(stderr, "%x\n", e->key() );
			//e->ignore();
	}
}

RegCallDialog::RegCallDialog(string callid, MinisipMainWindowWidget *mainWidget, TimeoutProvider<string> *tp, string proxy/*, string authenticated*/):
			CallDialog(callid, mainWidget, tp),
			userPassLayout(this,3,2),
			usernameLabel("Username: ",this),
			usernameEdit(this),
			passwordLabel("Password: ",this),
			passwordEdit(this),
			saveCheck("Save this information", this){
	

	//vlayout.addWidget(&status);
	status.setText(("Registering to "+proxy+"...").c_str());
	
	//vlayout.addWidget(&security_status);
	security_status.setText("");
//	if(authenticated != "")
//		status.setText(string("(Authenticated Registration)").c_str());
	
	
	vlayout.insertLayout(2,&userPassLayout);
	vlayout.insertSpacing(3,25);
	userPassLayout.addWidget(&usernameLabel, 0, 0);
	userPassLayout.addWidget(&usernameEdit,  0, 1);
	userPassLayout.addWidget(&passwordLabel, 1, 0);
	userPassLayout.addWidget(&passwordEdit,  1, 1);
	userPassLayout.addWidget(&saveCheck,     2, 0);

	usernameLabel.hide();
	usernameEdit.hide();
	passwordLabel.hide();
	passwordEdit.hide();
	saveCheck.hide();

	//vlayout.addWidget(&checkRecord);
	checkRecord.hide();

	//vlayout.addWidget(&cancel);
	//connect(&cancel,SIGNAL(clicked()), this, SLOT(cancelClicked()) );
	//status.setMinimumWidth(250);

}

void RegCallDialog::rejectClicked(){
	if(rejectButton.text() == "Register"){
		CommandString reg(getCallId(), SipCommandString::setpassword, usernameEdit.text().ascii(), passwordEdit.text().ascii());
                //SipSMCommand cmd(reg, SipSMCommand::remote, SipSMCommand::TU);
                mainWidget->getCallback()->guicb_handleCommand(/*cmd*/ reg);
	}
	CallDialog::rejectClicked();
	/*if(cancel.text() == "Close"){
		timeoutProvider->cancel_request(this, SipCommandString::close_window);
		mainWidget->removeCallDialog(callId);
		done(0);
	}*/
}

bool RegCallDialog::handleCommand(CommandString command){
	if(command.getDestinationId() != getCallId()){
		return false;
	}
	
	if(command.getOp() == SipCommandString::register_sent){
		return true;
	}
	if(command.getOp() == SipCommandString::register_ok){
		status.setText(("Registration to "+command.getParam()+" successful").c_str());
		if(saveCheck.isChecked()){
			fprintf(stderr, "Saving registration information\n");
			XMLFileParser * parser = new XMLFileParser(config->configFileName);
			parser->changeValue("proxy_username",usernameEdit.text().ascii());
			parser->changeValue("proxy_password",passwordEdit.text().ascii());
			parser->saveToFile();
			delete parser;
		}
		usernameLabel.hide();
		usernameEdit.hide();
		passwordLabel.hide();
		passwordEdit.hide();
		saveCheck.hide();

		rejectButton.setText(string("Close").c_str());
		hideAcceptButton();
//		repaint();
		timeoutProvider->request_timeout(5000, this, SipCommandString::close_window);
		return true;
	}
	if(command.getOp() == SipCommandString::close_window){
		rejectClicked();
		return true;
	}
	if(command.getOp() == SipCommandString::register_failed_authentication
			|| command.getOp() == SipCommandString::register_failed){
		status.setText(string("Registration to "+command.getParam()+" failed\n"+command.getParam2()).c_str());
		//FIXME: ugly ...
		if (rejectButton.text() == "Cancel" || rejectButton.text() == "Hang up")
			rejectButton.setText(string("Close").c_str());
//		repaint();
		timeoutProvider->request_timeout(5000, this, SipCommandString::close_window);
		return true;
	}
	
	if(command.getOp() == SipCommandString::transport_error){
		status.setText(string("Registration to <"+command.getParam()+"> failed\n"+command.getParam2()).c_str());
		//FIXME: ugly ...
		if (rejectButton.text() == "Cancel" || rejectButton.text() == "Hang up")
			rejectButton.setText(string("Close").c_str());
//		repaint();
		timeoutProvider->request_timeout(5000, this, SipCommandString::close_window);
		return true;
	}
	
	if(command.getOp() == SipCommandString::ask_password){
		status.setText(("Please enter registration information\n"
				"for " + command.getParam()).c_str());
		rejectButton.setText("Register");
		usernameLabel.show();
		usernameEdit.show();
		passwordLabel.show();
		passwordEdit.show();
		saveCheck.show();
		return true;
	}
	
	return false;	
}
