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

#include <libmnetutil/Downloader.h>

using namespace std;

CertificatePair::CertificatePair() {
	issuedToThisCA = MRef<Certificate*>();
	issuedByThisCA = MRef<Certificate*>();
}
CertificatePair::CertificatePair(MRef<Certificate*> issuedToThisCA) {
	this->issuedToThisCA = issuedToThisCA;
	this->issuedByThisCA = MRef<Certificate*>();
}
CertificatePair::CertificatePair(MRef<Certificate*> issuedToThisCA, MRef<Certificate*> issuedByThisCA) {
	this->issuedToThisCA = issuedToThisCA;
	this->issuedByThisCA = issuedByThisCA;
}

PrivateKey::PrivateKey(){
}

PrivateKey::~PrivateKey(){
}

Certificate::Certificate(){
}

Certificate::~Certificate(){
}

bool Certificate::hasAltNameSipUri(std::string uri) {
	return hasAltName(uri, Certificate::SAN_URI);
}
bool Certificate::hasAltName(std::string uri) {
	SubjectAltName types[] = {Certificate::SAN_DNSNAME, Certificate::SAN_RFC822NAME, Certificate::SAN_URI, Certificate::SAN_IPADDRESS};
	for (int i=0; i<4; i++)
		if (hasAltName(uri, types[i]))
			return true;
	return false;
}
bool Certificate::hasAltName(std::string uri, SubjectAltName type) {
	std::vector<std::string> altNames = getAltName(type);
	std::vector<std::string>::iterator nameIter;
	for (nameIter = altNames.begin(); nameIter != altNames.end(); nameIter++) {
		if ((*nameIter) == uri) {
			return true;
		}
	}
	return false;
}

string Certificate::getFile(){
	return file;
}

string Certificate::getPkFile(){
	massert(m_pk);
	return m_pk->getFile();
}

int Certificate::denvelopeData( unsigned char * data,
				 int size,
				 unsigned char *retdata,
				 int *retsize,
				 unsigned char *enckey,
				 int enckeylgth,
				 unsigned char *iv){
	massert(m_pk);
	massert(data);
	massert(retdata);
	massert(enckey);
	massert(iv);
	return m_pk->denvelopeData( data, size, retdata, retsize,
				     enckey, enckeylgth, iv );
}

int Certificate::signData( unsigned char * data, int data_length,
			    unsigned char * sign,
			    int * sign_length ){
	massert(m_pk);
	return m_pk->signData( data, data_length, sign, sign_length );
}

int Certificate::privateDecrypt(const unsigned char *data, int size,
				 unsigned char *retdata, int *retsize){
	massert(m_pk);
	return m_pk->privateDecrypt( data, size, retdata, retsize );
}


bool Certificate::verifySignedBy( MRef<Certificate*> cert ){
	massert(cert);
	MRef<CertificateSet*> set = CertificateSet::create();;
	set->addCertificate(cert);
	return cert->control(*set);
}


bool Certificate::hasPk(){
	return !m_pk.isNull();
}

MRef<PrivateKey*> Certificate::getPk(){
	return m_pk;
}

void Certificate::setPk( MRef<PrivateKey *> PrivateKey )
{
	if( !PrivateKey->checkCert( this ) ){
		cerr << "Private key does not match the Certificate" << endl;
		throw CertificateExceptionPkey(
			"The private key does not match the Certificate" );
	}

	m_pk = PrivateKey;
}

void Certificate::setPk( const std::string &file_ ){
	setPk( PrivateKey::load( file_ ) );
}

void Certificate::setEncpk(char *derEncPk, int length,
			    const std::string &password,
			    const std::string &path){
	setPk( PrivateKey::load( derEncPk, length, password, path ) );
}

CertificateSetItem::CertificateSetItem() : certificateUri(""), certificate(NULL) {
}

CertificateSetItem::CertificateSetItem(std::string certUri) : certificateUri(certUri), certificate(NULL) {
	loadCertAndIndex();
}

CertificateSetItem::CertificateSetItem(MRef<Certificate*> cert) : certificateUri(""), certificate(cert) {
	loadCertAndIndex();
}

CertificateSetItem::~CertificateSetItem(){
}

void CertificateSetItem::loadCertAndIndex() {
	if (!certificate.isNull()) {
		reindexCert();
	} else if (certificateUri.length() > 0) {
		MRef<Downloader*> loader = Downloader::create(certificateUri);
		int certLen = 0;
		char* certData;
		certData = loader->getChars(&certLen);
		if (certData != NULL && certLen > 0) {
			certificate = Certificate::load(reinterpret_cast<unsigned char*>(certData), certLen);
			reindexCert();
		}
	}
}

std::string CertificateSetItem::getSubject() {
	return subject;
}
std::vector<std::string> CertificateSetItem::getSubjectAltNames() {
	return subjectAltNames;
}
std::string CertificateSetItem::getSubjectKeyIdentifier() {
	return subjectKeyIdentifier;
}

std::string CertificateSetItem::getIssuer() {
	return issuer;
}
std::vector<std::string> CertificateSetItem::getIssuerAltNames() {
	return issuerAltNames;
}
std::string CertificateSetItem::getIssuerKeyIdentifier() {
	return issuerKeyIdentifier;
}

bool CertificateSetItem::isSelfSigned() {
	return selfSigned;
}

std::string CertificateSetItem::getCertificateUri() {
	return certificateUri;
}
MRef<Certificate*> CertificateSetItem::getCertificate() {
	if (certificate.isNull())
		loadCertAndIndex();
	return certificate;
}

void CertificateSetItem::reindexCert() {
	if (!certificate.isNull()) {
		subject = certificate->getName();
		std::vector<std::string> subjectAltNames;

		Certificate::SubjectAltName altTypes[] = {Certificate::SAN_DNSNAME, Certificate::SAN_RFC822NAME, Certificate::SAN_URI, Certificate::SAN_IPADDRESS};
		for (int i=0; i < 4; i++) {
			std::vector<std::string> tempNames = certificate->getAltName(altTypes[i]);
			subjectAltNames.insert(subjectAltNames.end(), tempNames.begin(), tempNames.end());
		}

		//subjectKeyIdentifier;

		issuer = certificate->getIssuer();
		//issuerAltNames;
		//issuerKeyIdentifier;

		selfSigned = (subject == issuer);
	}
}

CertificateSet::CertificateSet(){
	items_index = items.begin();
}

CertificateSet::~CertificateSet(){
	std::list<MRef<CertificateSetItem*> >::iterator i;
	std::list<MRef<CertificateSetItem*> >::iterator last = items.end();

	items.clear();
}

CertificateSet* CertificateSet::clone(){
	CertificateSet * db = create();

	lock();
	std::list<MRef<CertificateSetItem*> >::iterator i;
	std::list<MRef<CertificateSetItem*> >::iterator last = items.end();

	for( i = items.begin(); i != last; i++ ){
		db->addItem( *i );
	}

	unlock();
	return db;
}

void CertificateSet::lock(){
        mLock.lock();
}

void CertificateSet::unlock(){
        mLock.unlock();
}

void CertificateSet::addItem( MRef<CertificateSetItem*> item ){
	items.push_back( item );
	items_index = items.begin();
}

MRef<CertificateSetItem*> CertificateSet::createDirItem( std::string dir ){
	MRef<CertificateSetItem*> item = new CertificateSetItem();

	item->item = dir;
	item->type = CERT_DB_ITEM_TYPE_DIR;
	return item;
}

MRef<CertificateSetItem*> CertificateSet::createFileItem( std::string file ){
	MRef<CertificateSetItem*> item = new CertificateSetItem;

	item->item = file;
	item->type = CERT_DB_ITEM_TYPE_FILE;
	return item;
}

MRef<CertificateSetItem*> CertificateSet::createCertItem( MRef<Certificate*> cert ){
	MRef<CertificateSetItem*> item = new CertificateSetItem();

	item->item = "";
	item->type = CERT_DB_ITEM_TYPE_OTHER;
	return item;
}

void CertificateSet::addDirectory( string dir ){
	MRef<CertificateSetItem*> item = createDirItem( dir );
	addItem( item );
}

void CertificateSet::addFile( string file ){
	MRef<CertificateSetItem*> item = createFileItem( file );
	addItem( item );
}

void CertificateSet::addCertificate( MRef<Certificate *> cert ){
	MRef<CertificateSetItem*> item = createCertItem( cert );
	addItem( item );
}

void CertificateSet::remove( MRef<CertificateSetItem*> removedItem ){
	initIndex();

	while( items_index != items.end() ){
		if( **(*items_index) == **removedItem ){
			items.erase( items_index );
			initIndex();
			return;
		}
		items_index ++;
	}
	initIndex();
}

list<MRef<CertificateSetItem*> > &CertificateSet::getItems(){
	return items;
}

void CertificateSet::initIndex(){
	items_index = items.begin();
}

MRef<CertificateSetItem*> CertificateSet::getNext(){
	MRef<CertificateSetItem*> tmp;

	if( items_index == items.end() ){
		items_index = items.begin();
		return NULL;
	}

	tmp = *items_index;
	items_index ++;
	return tmp;
}

CertificateChain::CertificateChain(){
	item = cert_list.begin();

}

CertificateChain::CertificateChain( MRef<Certificate *> cert ){

	cert_list.push_back( cert );
	item = cert_list.begin();
}

CertificateChain::~CertificateChain(){
}

CertificateChain* CertificateChain::clone(){
	CertificateChain * chain = create();

	lock();
	std::list<MRef<Certificate*> >::iterator i;
	std::list<MRef<Certificate*> >::iterator last = cert_list.end();

	for( i = cert_list.begin(); i != last; i++ ){
		chain->addCertificate( *i );
	}

	unlock();
	return chain;
}

void CertificateChain::lock(){
        mLock.lock();
}

void CertificateChain::unlock(){
        mLock.unlock();
}

bool CertificateChain::isEmpty(){
	return cert_list.empty();
}


void CertificateChain::addCertificate( MRef<Certificate *> cert ){

	if( !cert_list.empty() ){
		MRef<Certificate *> lastCert = *(--cert_list.end());

		if( lastCert->getIssuer() != cert->getName() ){
			throw CertificateExceptionChain(
			 	"The previous Certificate in the chain is not"
				" issued by the given one" );
		}
	}

	cert_list.push_back( cert );
	item = cert_list.begin();
}

void CertificateChain::removeLast(){
	cert_list.erase( -- cert_list.end() );

	item = cert_list.begin();
}


void CertificateChain::initIndex(){
	item = cert_list.begin();
}

MRef<Certificate *> CertificateChain::getNext(){
	MRef<Certificate *> ret;

	if( item == cert_list.end() ){
		item = cert_list.begin();
		return NULL;
	}

	ret = *item;
	item ++;
	return ret;
}

MRef<Certificate *> CertificateChain::getFirst(){
	if( cert_list.size() == 0 ){
		return NULL;
	}

	return *(cert_list.begin());
}

void CertificateChain::clear(){
	cert_list.clear();

}

int CertificateChain::length(){
	return (int)cert_list.size();
}
