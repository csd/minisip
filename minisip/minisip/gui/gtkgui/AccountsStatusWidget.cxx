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

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>
#include<AccountsStatusWidget.h>

#include<libmsip/SipDialogConfig.h> // SipIdentity
#include<libmsip/SipCommandString.h>


#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind
#else
#define SLOT(a,b) sigc::mem_fun(a,b)
#define BIND sigc::bind
#endif

using namespace std;

AccountsStatusWidget::AccountsStatusWidget( Glib::RefPtr<AccountsList> list ):
	registerMenu( "Register" ),
	unregisterMenu( "Unregister" )
{
	set_model( list );
	
	rAccount = new Gtk::CellRendererText();
	insert_column_with_data_func( 0, "Account", *rAccount,
			SLOT( *this, &AccountsStatusWidget::drawAccount ) );
	
	rStatus = new Gtk::CellRendererText();
	insert_column_with_data_func( 1, "Status", *rStatus,
			SLOT( *this, &AccountsStatusWidget::drawStatus ) );

	columns = list->columns;

	set_headers_visible( false );
	set_rules_hint( true );

	signal_button_press_event().connect_notify(
		SLOT( *this, &AccountsStatusWidget::onClicked ), false );

	// Set up the menu
	popupMenu.add( registerMenu );
	popupMenu.add( unregisterMenu );
	popupMenu.show_all();

	registerMenu.signal_activate().connect( 
		SLOT( *this, &AccountsStatusWidget::registerClicked ) );
	
	unregisterMenu.signal_activate().connect( 
		SLOT( *this, &AccountsStatusWidget::unregisterClicked ) );

	callback = NULL;
}

AccountsStatusWidget::~AccountsStatusWidget(){
	delete rStatus;
	delete rAccount;
}

void AccountsStatusWidget::drawAccount( Gtk::CellRenderer * renderer,
		const Gtk::TreeModel::iterator & iter ){

	Gtk::CellRendererText * textR = (Gtk::CellRendererText *)renderer;
	Glib::ustring account = "<b>" + (*iter)[columns->name] + "</b>\n  ";
	account += "<small><span foreground=\"#0000FF\">" + 
		(*iter)[columns->uri] +
		"</span></small>";

	textR->property_markup().set_value( account );

}

void AccountsStatusWidget::drawStatus( Gtk::CellRenderer * renderer,
		const Gtk::TreeModel::iterator & iter ){

	Gtk::CellRendererText * textR = (Gtk::CellRendererText *)renderer;
	Glib::ustring status;
	MRef<SipIdentity *> identity; 

	identity = (*iter)[columns->identity];

	if( identity && identity->isRegistered() ){
		status = "<span foreground=\"#00FF00\">Registered</span>";
	}
	else{
		status = "<span foreground=\"#FF0000\">Unregistered</span>";
	}

	textR->property_markup().set_value( status );

}

void AccountsStatusWidget::onClicked( GdkEventButton * event ){
	if( event->button == 3 ){
		popupMenu.popup( event->button, gtk_get_current_event_time() );
	}
}

void AccountsStatusWidget::registerClicked(){
	Glib::RefPtr<Gtk::TreeSelection> selection = get_selection();

	Gtk::TreeModel::iterator iter = selection->get_selected();
	MRef<SipIdentity *> id;

	if( iter ){
		id = (*iter)[columns->identity];
		if( id ){
			CommandString reg( "", SipCommandString::proxy_register );
			reg["identityId"] = id->getId();
			id->lock();
			reg["proxy_domain"] = id->getSipUri().getIp();
			reg.setParam3( id->getSipRegistrar()->getDefaultExpires() );
			id->unlock();
			callback->handleCommand("sip", reg );
		}
	}
}

void AccountsStatusWidget::unregisterClicked(){
	Glib::RefPtr<Gtk::TreeSelection> selection = get_selection();

	Gtk::TreeModel::iterator iter = selection->get_selected();
	MRef<SipIdentity *> id;

	if( iter ){
		id = (*iter)[columns->identity];
		if( id ){
			CommandString reg( "", SipCommandString::proxy_register );
			reg["identityId"] = id->getId();
			id->lock();
			reg["proxy_domain"] = id->getSipUri().getIp();
			id->unlock();
			reg.setParam3( "0" );
			callback->handleCommand("sip", reg );
		}
	}
}

void AccountsStatusWidget::setCallback( MRef<CommandReceiver*> callback ){
	this->callback = callback;
}
