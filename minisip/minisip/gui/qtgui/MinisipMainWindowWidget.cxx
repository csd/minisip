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

#include"MinisipMainWindowWidget.h"
#include"qtguistarter.h"
#include<qapplication.h>
#include<qvbox.h>
#include<qpopupmenu.h>
#if (QT_VERSION > 0x030000)
#include<qkeysequence.h>
#endif
#include<qmainwindow.h>
#include<qstatusbar.h>
#include<qpushbutton.h>
#include<qhbox.h>
#include<qlayout.h>
#include<qlineedit.h>
#include<qheader.h>
#include<qlistview.h>
#include<qmenubar.h>
#include<qevent.h>
#include<qmessagebox.h>
//#include"../../../sip/state_machines/SMCommand.h"
#ifdef OPIE
#include <qcopchannel_qws.h>
#endif

#include"SettingsDialog.h"
#include<libmutil/itoa.h>
#include<libmutil/trim.h>
#include<fstream>

#include<libmutil/XMLParser.h>
#include<libmnetutil/IP4ServerSocket.h>
#include<libmnetutil/TCPSocket.h>
#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipCommandString.h>
#include"../../../sip/DefaultDialogHandler.h"





MinisipMainWindowWidget::MinisipMainWindowWidget(TimeoutProvider<string> *tp,
		QWidget *parent, 
		const char *name ): QWidget( parent, name ),
			callboxlayout(), 
			settingsDialog(this), 
#if (QT_VERSION < 0x030000)
			tabCount(1),
#endif
			tabs(this),
			phonebooktabw(&tabs), 
			phonebookvlayout(&phonebooktabw), 
			text(&phonebooktabw), 
			vlayout( this ), 
			statusBar(NULL),
			timeoutProvider(tp)
{	
	setMinimumSize( 180, 200 );
	setCaption("Minisip");
	callboxlayout.addWidget(&text);
	
	QPushButton *call = new QPushButton( "Call", &phonebooktabw, "call" );
	callboxlayout.addWidget(call);
	
	listView = new QListView(&phonebooktabw);
	listView->setAllColumnsShowFocus(true);
	listView->setRootIsDecorated(true);
	listView->header()->hide();
	listView->addColumn("Phone books");
#if (QT_VERSION > 0x030000)
	listView->setResizeMode(QListView::AllColumns);
#endif


//	this->addWidget(&tabs);
//	phonebookvlayout.addWidget(&tabs);

	
	//vlayout.addLayout(&bottombox);
        phonebookvlayout.addWidget(listView);
        phonebookvlayout.addLayout(&callboxlayout);
	
        
	//tabs.insertTab(listView, "Phone books"/*title.c_str()*/,0);
        
	vlayout.addWidget(&tabs);
        
        tabs.insertTab(&phonebooktabw, "Phone books",0);
	
        
        tabs.showPage(&phonebooktabw);
	

	connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));
	connect(call, SIGNAL(clicked()), this, SLOT(callPressed()));
	connect(&text, SIGNAL(returnPressed()), this, SLOT(callPressed()));
	connect(listView, SIGNAL(selectionChanged()), this, SLOT(phoneBookSelect()));
	connect(listView, SIGNAL(doubleClicked(QListViewItem *, const QPoint &, int)), this, SLOT(userDoubleClicked(QListViewItem *, const QPoint &, int)));

/*
	list<string> phonebooks = sipphone->getPhoneConfig()->phonebooks;
	for (list<string>::iterator i=phonebooks.begin(); i!=phonebooks.end(); i++){
			addPhoneBook(*i);
	}
*/
	
#ifdef MINISIP_AUTOCALL	
	if (sipphoneconfig->autoCall.size()>0)
		tp->request_timeout(5000,this, "autocall");
#endif

#ifdef OPIE
	setCaption("minisip");
#ifndef QT_NO_COP
	QPEApplication::grabKeyboard();
#endif
#endif

	
#ifdef OPIE
	IP4ServerSocket *update_serversock = new IP4ServerSocket(5034,1);
	update_socket.connectToHost("127.0.0.1",5034);
	do_update_socket = update_serversock->do_accept();

	connect(&update_socket, SIGNAL(readyRead()), this, SLOT(readChar));
#endif
}

void MinisipMainWindowWidget::displayErrorMessage(string m){
	QMessageBox msg("Error message", m.c_str(), QMessageBox::Warning,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton,this);
	msg.exec();


}

void MinisipMainWindowWidget::setSipSoftPhoneConfiguration(MRef<SipSoftPhoneConfiguration*> sipphoneconfig){
	this->sipphoneconfig = sipphoneconfig;
        
	settingsDialog.setConfig(sipphoneconfig);

	list<string> phonebooks = sipphoneconfig->phonebooks;
	for (list<string>::iterator i=phonebooks.begin(); i!=phonebooks.end(); i++){
			addPhoneBook(*i);
	}
}

MRef<SipSoftPhoneConfiguration *>MinisipMainWindowWidget::getSipSoftPhoneConfiguration(){
    return sipphoneconfig;
}


void MinisipMainWindowWidget::readChar(){
#ifdef OPIE
	update_socket.getch();
#endif
}

#ifdef OPIE
void MinisipMainWindowWidget::updateGui(){
	char *str="X";
	void *buf = (void *)str;
	do_update_socket->do_write(buf,1);
}
#endif

void MinisipMainWindowWidget::phoneBookSelect(){
//#if (QT_VERSION > 0x030000)
//	QListViewItem *sel = listView->selectedItem();
//	if (sel->rtti()==PersonListItem::RTTI_VAL){
//		text.setText(((PersonListItem *)sel)->getUri().c_str());
//		text.setText("");
//	}else{
//		text.setText("");
//	}
//	
//#else
	QListViewItem *sel = listView->selectedItem();
	if (sel->childCount()==0){
		text.setText(((PersonListItem *)sel)->getUri().c_str());
	}else{
		text.setText("");
	}


//#endif
	
	/*if (dynamic_cast<PersonListItem *>(sel)!=NULL){
		text.setText(((PersonListItem *)sel)->getUri().c_str());
	}else{
		text.setText("");
	}*/
}

void MinisipMainWindowWidget::userDoubleClicked(QListViewItem *item,const QPoint &, int){
	/*if (dynamic_cast<PersonListItem *>(item)!=NULL){
		text.setText(((PersonListItem *)item)->getUri().c_str());
		doCall();

	}else{
		text.setText("");
	}*/
}

#ifdef MINISIP_AUTOCALL
void MinisipMainWindowWidget::timeout(string command){
	
//	if (command=="tst_error_message"){
//		handleCommand(CommandString("","error_message","This is an example error message!"));
//		return;
//	}

	assert(command=="autocall");
	handleCommand(CommandString("","autocall"));
}
#endif

void MinisipMainWindowWidget::createRegisterDialog(string title, string callId, string proxy){
	RegCallDialog *regdialog = new RegCallDialog(callId, this, timeoutProvider, proxy);
//	tabs.addTab(regdialog, title.c_str());
//	
#if (QT_VERSION < 0x030000)
	tabCount++;
#endif
	tabs.insertTab(regdialog, title.c_str()/*,0*/);
	//tabs.setCurrentPage(0);
	tabs.show();
	addCallDialog(regdialog);
	
#if (QT_VERSION < 0x030000)
	tabs.setCurrentPage(tabCount - 1);
#else
	tabs.setCurrentPage(tabs.count() - 1);
#endif
	
}

void MinisipMainWindowWidget::createCallDialog(string title, string callId, string from="", string secured=""){
	CallDialog *calldialog;
#if (QT_VERSION < 0x030000)
		tabCount++;
#endif
	if (from.length()>0 || secured.length()>0){
		calldialog = new CallDialog(callId, this, timeoutProvider, from, secured);
		tabs.insertTab(calldialog, title.c_str());
		
		tabs.showPage( calldialog );
	}else{
		calldialog = new CallDialog(callId, this, timeoutProvider);
		tabs.insertTab(calldialog, title.c_str());
		tabs.showPage( calldialog );
	}
	addCallDialog(calldialog);
}

void MinisipMainWindowWidget::doCall(){
	string uri=text.text().ascii();
	if (uri.length()>0){
		
		
		string id=callback->guicb_doInvite( uri );  //NOTE: callback is a protected member from the
                                                             //      superclass "GUI".
		if (id=="malformed"){
			QMessageBox::warning(this,"Minisip","The SIP address you specified is not valid",  QMessageBox::Ok, QMessageBox::NoButton);
			return;
		}

		createCallDialog("Call",id);

//		log(LOG_INFO, "MinisipMainWindowWidget::callPressed(): id of new call is "+id);
	}
}


void MinisipMainWindowWidget::callPressed(){
//	log(LOG_INFO, "Pressed call, text="+text.text());
	doCall();
}



static string getstringViaHTTP(string uri){
	string host = uri.substr(0, uri.find("/"));
	string path= uri.substr(uri.find("/"));

	TCPSocket sock(host, 80);	//FiXME: parse port from addr
	sock.write("GET "+path+"\r\n\r\n");
	
	char buf[4096];
	string ret;
	int n;
	while ((n=sock.read(buf,4095))>0){
		buf[n]=0;
		ret = ret + string(buf); 
	}
	return ret;
}

void MinisipMainWindowWidget::addPhoneBook(string filename){
//	cerr << "Opening phonebook "<< filename << endl;
	if (filename.size()<=7)
		return;
	XMLParser *parser;

	if (filename.substr(0,7)=="file://"){
//		cerr << "Creating XML file parser"<< endl;
		parser = new XMLFileParser(filename.substr(7));	
	}else{

		if (filename.substr(0,7)=="http://"){
			parser = new XMLstringParser( getstringViaHTTP(filename.substr(7)) );
		}else{
			cerr << "Warning: unknown phone book access method (should start with file:// or http://): "<< filename << endl;
			return;
		}
	}

	string phonebook;
	int phonebooks=0;
	do{
		string q = "phonebook["+itoa(phonebooks)+"]/name";
//		cerr << "Parsing phonebook "<< q << endl;
		phonebook=parser->getValue(q,"");
		if (phonebook!=""){
			QListViewItem *phonebookparent= new QListViewItem(listView, phonebook.c_str());
			string contact;

			int contacts=0;
			do{
				string q = "phonebook["+itoa(phonebooks)+"]/contact["+itoa(contacts)+"]/name";
				contact = parser->getValue(q,"");
//				cerr << "Parsing contact:"<< q << endl;
				if (contact!=""){
					QListViewItem *contactparent= new QListViewItem(phonebookparent, contact.c_str());
					int pops=0;
					string qbase;
					string desc;
					do{
						string qbase = "phonebook["+itoa(phonebooks)+"]/contact["+itoa(contacts)+"]/pop["+itoa(pops)+"]/";
						desc = parser->getValue(qbase+"desc","");
//						cerr << "Parsed: "<< desc<< endl;
						if (desc!=""){
							string uri = parser->getValue(qbase+"uri","");
							PersonListItem *item = new PersonListItem(contactparent,desc,uri,false, "");//TODO: parse different parts of line and send as arguments
						}
						pops++;
					}while(desc!="");
				}
				contacts++;
			}while(contact!="");
		}
		phonebooks++;
	}while(phonebook!="");
	delete parser;
}


void MinisipMainWindowWidget::displaySettings(){
#ifdef OPIE
	((QPEApplication *)qApp)->showMainWidget(&settingsDialog);
#else
	settingsDialog.show();
#endif
}

void MinisipMainWindowWidget::customEvent(QCustomEvent *e){
	//	cerr << "Got event in MinisipMainWindowWidget"<< endl;

	CommandEvent *ce = /*dynamic_cast<CommandEvent *>*/(CommandEvent *)(e);
	if (ce==NULL){
		cerr << "WARNING could not convert to customEvent"<<endl;
	}else{
		//		cerr << "got command "<< ce->getCommand().getOp()<<endl;
		CommandString command(ce->getCommand());
#ifdef MINISIP_AUTOCALL
		if (command.getOp()=="autocall"){
			text.setText(sipphone->autoCall.c_str());
			doCall();
			return;
		}
#endif
//		if (command.getOp()!=CommandString::log)
//			log(LOG_INFO, "MinisipMainWindowWidget::customEvent: Received command "+command.getstring());

		for (list<CallDialog *>::iterator i=calls.begin(); i != calls.end(); i++)
			if ((*i)->handleCommand(command))
				return;

//		if (command.getOp()==CommandString::register_ok){
//			if (statusBar!=NULL)
//				statusBar->message(("Registred to proxy "+command.getParam()).c_str(),10000);
//			return;
//		}

		if (command.getOp()==SipCommandString::ask_password){

			createRegisterDialog("Registration", command.getDestinationId(), command.getParam());
			handleCommand(command);
		}
		
		if (command.getOp()==SipCommandString::error_message){
			QMessageBox msg("Error message", command.getParam().c_str(), QMessageBox::Warning,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton,this);
			msg.exec();

			return;
		}

/*
		if (command.getOp()==CommandString::log){
#ifdef DEBUG_OUTPUT
//			cerr << "LOG: "<<command.getParam2()<< endl;
#endif
#if (QT_VERSION > 0x030000) && defined DEBUG_OUTPUT
			logDialog.log(atoi(command.getParam().c_str()), command.getParam2());
#endif
			return;
		}
*/
                
		if (command.getOp()==SipCommandString::proxy_register){
//			return getSipPhone()->enqueueCommand(SipSMCommand(command, SipSMCommand::remote, SipSMCommand::TU));
                        //SipSMCommand cmd(command, SipSMCommand::remote, SipSMCommand::TU);
			callback->guicb_handleCommand(/*cmd*/ command);
			return;
		}

//		if (command.getOp()==CommandString::register_failed_authentication){
//			if (statusBar!=NULL)
//				statusBar->message(("Failed registration to proxy."));
//			//			logDialog.log(atoi(command.getParam().c_str()), command.getParam2());
//			return;
//		}

		if (command.getOp()==SipCommandString::register_sent){

			//createRegisterDialog("Call", command.getCallId(), command.getParam());
		}


		if (command.getOp()==SipCommandString::incoming_available){
			if (statusBar!=NULL)
				statusBar->message("Started a call",10000);


//			CallDialog *calldialog = new CallDialog(command.getCallId(),this, timeoutProvider, command.getParam(), command.getParam2());
//			addCallDialog(calldialog);
//			calldialog->show();
			createCallDialog("Call", command.getDestinationId(), command.getParam(), command.getParam2() );


			return;
		}
	}
}


void MinisipMainWindowWidget::handleCommand(CommandString command){
//		if (command.getOp()!=CommandString::log)
//			log(LOG_INFO, "MinisipMainWindowWidget::handleCommand: Received command "+command.getstring());
		CommandEvent *ce = new CommandEvent(command); // Qt will delete the event when done
		qApp->postEvent(this,ce);
#ifdef OPIE
		updateGui();
#endif		
	}

void MinisipMainWindowWidget::setStatusBar(QStatusBar *bar){
	statusBar = bar;
}

//#ifdef OPIE
//void MinisipMainWindowWidget::setApplication(QPEApplication *app){
//#else
void MinisipMainWindowWidget::setApplication(QApplication *app){
	//#endif
	this->qapplication = app;
}

/*
void MinisipMainWindowWidget::log(int type, string msg){
	//	logDialog.log(type,msg);	//not thread safe
	CommandString log("",CommandString::log,itoa(type), msg);
	handleCommand(log);
}
*/

void MinisipMainWindowWidget::addCallDialog(CallDialog *d){
	calls.push_back(d);
}

void MinisipMainWindowWidget::removeCallDialog(string callId){
	for (list<CallDialog *>::iterator i=calls.begin(); i!=calls.end(); i++)
		if ((*i)->getCallId()==callId){

			tabs.removePage(*i);
			calls.erase(i);
			
#if (QT_VERSION < 0x030000)
			tabCount--;
#endif
			return;
		}
}

void MinisipMainWindowWidget::run(){
	qtguirun( this );
}

bool MinisipMainWindowWidget::configDialog( MRef<SipSoftPhoneConfiguration *> conf ){
	bool ok;
	SettingsDialog * sd = new SettingsDialog();
	sd->setConfig( conf );
	ok = sd->exec();
	delete sd;
	return ok;
}

PersonListItem::PersonListItem( QListViewItem *parent, string name, string uri, bool online, string location)
        : QListViewItem( parent )
{
	this->name = name;
	this->uri = uri;
	this->online = online;
	this->location = location;
	string loc;
	if (location.length()>0)
		loc = "["+location+"] ";
	setText(0, (loc+name+", "+uri).c_str()) ;
}

#if (QT_VERSION > 0x030000)
int const PersonListItem::RTTI_VAL=3333;

int PersonListItem::rtti(){
	return RTTI_VAL;
}
#endif

void PersonListItem::paintCell( QPainter *p, const QColorGroup &cg,
		int32_t column, int32_t width, int32_t alignment )
{
	QColorGroup _cg( cg );
	QColor c = _cg.text();

	if ( /*status == "online"*/ online )
		_cg.setColor( QColorGroup::Text, Qt::darkGreen );
	//	if ( status == "offline" )
	//		_cg.setColor( QColorGroup::Text, Qt::red );

	QListViewItem::paintCell( p, _cg, column, width, alignment );

	_cg.setColor( QColorGroup::Text, c );
}


CommandEvent::CommandEvent(CommandString command): QCustomEvent(QEvent::User), command(command){

}

CommandString CommandEvent::getCommand(){
	return command;
}



