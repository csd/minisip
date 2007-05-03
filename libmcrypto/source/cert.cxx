/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2006 Mikael Magnusson
  
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
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include<config.h>

#include<libmcrypto/cert.h>

#include<stdio.h>
#include<assert.h>

#include<iostream>
#include <fstream>

using namespace std;

priv_key::priv_key(){
}

priv_key::~priv_key(){
}

certificate::certificate(){
}

certificate::~certificate(){
}


string certificate::get_file(){
	return file;
}

string certificate::get_pk_file(){
	return m_pk->get_file();
}

int certificate::denvelope_data( unsigned char * data,
				 int size,
				 unsigned char *retdata,
				 int *retsize,
				 unsigned char *enckey,
				 int enckeylgth,
				 unsigned char *iv){
	return m_pk->denvelope_data( data, size, retdata, retsize,
				     enckey, enckeylgth, iv );
}

int certificate::sign_data( unsigned char * data, int data_length, 
			    unsigned char * sign,
			    int * sign_length ){
	return m_pk->sign_data( data, data_length, sign, sign_length );
}

int certificate::private_decrypt(const unsigned char *data, int size,
				 unsigned char *retdata, int *retsize){
	return m_pk->private_decrypt( data, size, retdata, retsize );
}

bool certificate::has_pk(){
	return !m_pk.isNull();
}

MRef<priv_key*> certificate::get_pk(){
	return m_pk;
}

void certificate::set_pk( MRef<priv_key *> priv_key )
{
	if( !priv_key->check_cert( this ) ){
		cerr << "Private key does not match the certificate" << endl;
		throw certificate_exception_pkey(
			"The private key does not match the certificate" );
	}

	m_pk = priv_key;
}

void certificate::set_pk( const std::string &file ){
	set_pk( priv_key::load( file ) );
}

void certificate::set_encpk(char *derEncPk, int length,
			    const std::string &password,
			    const std::string &path){
	set_pk( priv_key::load( derEncPk, length, password, path ) );
}

ca_db_item::~ca_db_item(){
}


ca_db::ca_db(){
	items_index = items.begin();
}

ca_db::~ca_db(){
	std::list<MRef<ca_db_item*> >::iterator i;
	std::list<MRef<ca_db_item*> >::iterator last = items.end();

	items.clear();
}

ca_db* ca_db::clone(){
	ca_db * db = create();

	lock();
	std::list<MRef<ca_db_item*> >::iterator i;
	std::list<MRef<ca_db_item*> >::iterator last = items.end();

	for( i = items.begin(); i != last; i++ ){
		db->add_item( *i );
	}
	
	unlock();
	return db;
}

void ca_db::lock(){
        mLock.lock();
}

void ca_db::unlock(){
        mLock.unlock();
}

void ca_db::add_item( MRef<ca_db_item*> item ){
	items.push_back( item );
	items_index = items.begin();
}

MRef<ca_db_item*> ca_db::create_dir_item( std::string dir ){
	MRef<ca_db_item*> item = new ca_db_item();
	
	item->item = dir;
	item->type = CERT_DB_ITEM_TYPE_DIR;
	return item;
}

MRef<ca_db_item*> ca_db::create_file_item( std::string file ){
	MRef<ca_db_item*> item = new ca_db_item;
	
	item->item = file;
	item->type = CERT_DB_ITEM_TYPE_FILE;
	return item;
}

MRef<ca_db_item*> ca_db::create_cert_item( MRef<certificate*> cert ){
	MRef<ca_db_item*> item = new ca_db_item();
	
	item->item = "";
	item->type = CERT_DB_ITEM_TYPE_OTHER;
	return item;
}

void ca_db::add_directory( string dir ){
	MRef<ca_db_item*> item = create_dir_item( dir );
	add_item( item );
}

void ca_db::add_file( string file ){
	MRef<ca_db_item*> item = create_file_item( file );
	add_item( item );
}

void ca_db::add_certificate( MRef<certificate *> cert ){
	MRef<ca_db_item*> item = create_cert_item( cert );
	add_item( item );
}

void ca_db::remove( MRef<ca_db_item*> removedItem ){
	init_index();

	while( items_index != items.end() ){
		if( **(*items_index) == **removedItem ){
			items.erase( items_index );
			init_index();
			return;
		}
		items_index ++;
	}
	init_index();
}

list<MRef<ca_db_item*> > &ca_db::get_items(){
	return items;
}

void ca_db::init_index(){
	items_index = items.begin();
}

MRef<ca_db_item*> ca_db::get_next(){
	MRef<ca_db_item*> tmp;
	
	if( items_index == items.end() ){
		items_index = items.begin();
		return NULL;
	}

	tmp = *items_index;
	items_index ++;
	return tmp;
}

certificate_chain::certificate_chain(){
	item = cert_list.begin();

}

certificate_chain::certificate_chain( MRef<certificate *> cert ){
	
	cert_list.push_back( cert );
	item = cert_list.begin();
}

certificate_chain::~certificate_chain(){
}

certificate_chain* certificate_chain::clone(){
	certificate_chain * chain = create();

	lock();
	std::list<MRef<certificate*> >::iterator i;
	std::list<MRef<certificate*> >::iterator last = cert_list.end();

	for( i = cert_list.begin(); i != last; i++ ){
		chain->add_certificate( *i );
	}
	
	unlock();
	return chain;
}

void certificate_chain::lock(){
        mLock.lock();
}

void certificate_chain::unlock(){
        mLock.unlock();
}

bool certificate_chain::is_empty(){
	return cert_list.empty();
}


void certificate_chain::add_certificate( MRef<certificate *> cert ){
	
	if( !cert_list.empty() ){
		MRef<certificate *> lastCert = *(--cert_list.end());

		if( lastCert->get_issuer() != cert->get_name() ){
			throw certificate_exception_chain(
			 	"The previous certificate in the chain is not"
				" issued by the given one" );
		}
	}
	
	cert_list.push_back( cert );
	item = cert_list.begin();
}

void certificate_chain::remove_last(){
	cert_list.erase( -- cert_list.end() );

	item = cert_list.begin();
}


void certificate_chain::init_index(){
	item = cert_list.begin();
}

MRef<certificate *> certificate_chain::get_next(){
	MRef<certificate *> ret;
	
	if( item == cert_list.end() ){
		item = cert_list.begin();
		return NULL;
	}

	ret = *item;
	item ++;
	return ret;
}

MRef<certificate *> certificate_chain::get_first(){
	if( cert_list.size() == 0 ){
		return NULL;
	}
	
	return *(cert_list.begin());
}

void certificate_chain::clear(){
	cert_list.clear();

}

int certificate_chain::length(){
	return (int)cert_list.size();
}
