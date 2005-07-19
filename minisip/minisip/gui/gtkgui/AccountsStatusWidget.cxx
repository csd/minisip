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


#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind
#else
#define SLOT(a,b) sigc::mem_fun(a,b)
#define BIND sigc::bind
#endif

using namespace std;

AccountsStatusWidget::AccountsStatusWidget( Glib::RefPtr<AccountsList> list ){
	cerr << "Before set model" << endl;
	set_model( list );
	
	Gtk::CellRendererText * rAccount = new Gtk::CellRendererText();
	insert_column_with_data_func( 0, "Account", *rAccount,
			SLOT( *this, &AccountsStatusWidget::drawAccount ) );
	
	Gtk::CellRendererText * rStatus = new Gtk::CellRendererText();
	insert_column_with_data_func( 1, "Status", *rStatus,
			SLOT( *this, &AccountsStatusWidget::drawStatus ) );

	columns = list->columns;

	set_headers_visible( false );
	set_rules_hint( true );
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
	MRef<SipIdentity *> identity = (*iter)[columns->identity];

	if( identity->isRegistered() ){
		status = "<span foreground=\"#00FF00\">Registered</span>";
	}
	else{
		status = "<span foreground=\"#FF0000\">Unregistered</span>";
	}

	textR->property_markup().set_value( status );

}
