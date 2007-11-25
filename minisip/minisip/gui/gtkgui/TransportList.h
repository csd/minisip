/*
  Copyright (C) 2006-2007 Mikael Magnusson
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
*/

#ifndef TRANSPORT_LIST_H
#define TRANSPORT_LIST_H

#include<gtkmm.h>
#include<libmutil/MemObject.h>

class SipTransportConfig;

class TransportListColumns : public Gtk::TreeModel::ColumnRecord{
	public:
		TransportListColumns();

		Gtk::TreeModelColumn< MRef<SipTransportConfig*> > config;
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> scheme;
		Gtk::TreeModelColumn<Glib::ustring> protocol;
		Gtk::TreeModelColumn<Glib::ustring> description;
		Gtk::TreeModelColumn<bool> enabled;
};

class TransportList : public MObject, public Gtk::ListStore{
	public:
		// MObject
		virtual std::string getMemObjectType() const {return "TransportList";};

		static Glib::RefPtr<TransportList> create( );

		void loadFromConfig( MRef<SipSoftPhoneConfiguration *> config );
		std::string saveToConfig( MRef<SipSoftPhoneConfiguration *> config );

		TransportListColumns *getColumns() const;

	private:
		TransportList( TransportListColumns *columns );

		TransportListColumns *columns;
};

#endif	// TRANSPORT_LIST_H
