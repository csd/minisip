/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2006 by Werner Dittmann
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
 *          Werner Dittmann <Werner.Dittmann@t-online.de>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


#include<config.h>
#include<libmcrypto/OakleyDH.h>
#include<libmutil/Exception.h>
#include<gcrypt.h>
#include"oakley_groups.h"
#include<iostream>

using namespace std;

#define CHECK_GCRY(err) if(err)	{ checkErr(err); }

static void checkErr(gcry_error_t a)
{
	if(a)
	{	
		cerr << "An error has occured: " << gcry_strerror(a) << endl;
	}
}

struct gcryptCtx {
	public:
		gcry_mpi_t privKey;
		gcry_mpi_t pubKey;
		uint32_t pLength;

		~gcryptCtx() {
			if( privKey ){
				gcry_mpi_release( privKey );
				privKey = NULL;
			}
			if( pubKey ){
				gcry_mpi_release( pubKey );
				pubKey = NULL;
			}
		}
};

struct dhParameter {
	public:
		gcry_mpi_t m_p;
		gcry_mpi_t m_g;
		uint32_t m_length;

		dhParameter(): m_p(NULL), m_g(NULL), m_length(0){}
		void init(const char *p, const char *g, uint32_t length);
};

void dhParameter::init(const char *p, const char *g, uint32_t length){
	CHECK_GCRY( gcry_mpi_scan( &m_p, GCRYMPI_FMT_HEX, p, 0, NULL ));
	CHECK_GCRY( gcry_mpi_scan( &m_g, GCRYMPI_FMT_HEX, g, 0, NULL ));
	m_length = length * 8;
}

#define NUM_GROUPS 3

static int g_init = 0;
static dhParameter g_dhParameters[NUM_GROUPS];


void OakleyDH_init(){
	if( g_init++ > 0 )
		return;

//	gcry_error_t res;

	g_dhParameters[DH_GROUP_OAKLEY1].init( OAKLEY1_P, OAKLEY1_G, OAKLEY1_L );
	g_dhParameters[DH_GROUP_OAKLEY2].init( OAKLEY2_P, OAKLEY2_G, OAKLEY2_L );
	g_dhParameters[DH_GROUP_OAKLEY5].init( OAKLEY5_P, OAKLEY5_G, OAKLEY5_L );
}

OakleyDH::OakleyDH(){
	OakleyDH_init();

	// Store opensslDhPtr in priv
	priv = new gcryptCtx();
	if( !priv )
	{
		throw Exception( "Could not create gcrypt "
				          "DH parameters." );
	}
}

OakleyDH::OakleyDH( int groupValue ){
	OakleyDH_init();

	// Store opensslDhPtr in priv
	priv = new gcryptCtx();
	if( !priv )
	{
		throw Exception( "Could not create gcrypt "
				          "DH parameters." );
	}

	if( !setGroup( groupValue ) ){
		throw Exception( "Could not set the  "
				          "DH group." );
	}
}

OakleyDH::~OakleyDH(){
	gcryptCtx *gnutlsDhPtr = ((gcryptCtx*)priv);

	if( gnutlsDhPtr ){
		delete gnutlsDhPtr;
		priv = NULL;
	}
}

bool OakleyDH::setGroup( int groupValue ){
	gcryptCtx *gnutlsDhPtr = ((gcryptCtx*)priv);

	if( groupValue >= NUM_GROUPS )
		return false;

	this->groupValue = groupValue;

	uint32_t len = g_dhParameters[ groupValue ].m_length;

	if( len != gnutlsDhPtr->pLength ){
		if( gnutlsDhPtr->privKey )
			gcry_mpi_release( gnutlsDhPtr->privKey );

		gnutlsDhPtr->privKey = gcry_mpi_new( len );
		gcry_mpi_randomize(gnutlsDhPtr->privKey, len, GCRY_STRONG_RANDOM);
		gnutlsDhPtr->pLength = len;
	}

	if( !generateKey() ){
		return false;
	}

	return true;
}

bool OakleyDH::generateKey()
{
	gcryptCtx *gnutlsDhPtr = ((gcryptCtx*)priv);

	gcry_mpi_t bnP = g_dhParameters[ groupValue ].m_p;
	gcry_mpi_t bnG = g_dhParameters[ groupValue ].m_g;

	if( gnutlsDhPtr->pubKey ){
		gcry_mpi_release( gnutlsDhPtr->pubKey );
	}

	gnutlsDhPtr->pubKey = gcry_mpi_new(0);

	gcry_mpi_powm(gnutlsDhPtr->pubKey, bnG, gnutlsDhPtr->privKey, bnP);
	return true;
}

uint32_t OakleyDH::publicKeyLength() const{
	gcryptCtx *gnutlsDhPtr = ((gcryptCtx*)priv);

	uint32_t len = (gcry_mpi_get_nbits( gnutlsDhPtr->pubKey ) + 7) / 8;
	return len;
}

uint32_t OakleyDH::getPublicKey(uint8_t *buf, uint32_t buflen) const{
	gcryptCtx *gnutlsDhPtr = ((gcryptCtx*)priv);

	if( buflen < publicKeyLength() )
		return 0;

	size_t nwritten = 0;
	gcry_mpi_print(GCRYMPI_FMT_USG, buf, buflen, &nwritten,
		       gnutlsDhPtr->pubKey);
	return nwritten;
}

int OakleyDH::computeSecret(const uint8_t *peerKeyPtr, uint32_t peerKeyLength,
			    uint8_t *secret, uint32_t secretSize) const{
	gcryptCtx *gnutlsDhPtr = ((gcryptCtx*)priv);

	if( !peerKeyPtr || !secret || secretSize < secretLength() ){
		return -1;
	}

	gcry_mpi_t pubKeyOther;
	gcry_mpi_t sec = gcry_mpi_new(0);
	gcry_mpi_t bnP = g_dhParameters[ groupValue ].m_p;

	gcry_mpi_scan(&pubKeyOther, GCRYMPI_FMT_USG, peerKeyPtr, peerKeyLength, NULL);
	gcry_mpi_powm(sec, pubKeyOther, gnutlsDhPtr->privKey, bnP);
	gcry_mpi_release(pubKeyOther);

	size_t result;
	gcry_mpi_print(GCRYMPI_FMT_USG, secret, secretSize, &result, sec);
	gcry_mpi_release(sec);
	return result;
}

int OakleyDH::group() const{
	return groupValue;

}

uint32_t OakleyDH::secretLength() const{
	gcryptCtx *gnutlsDhPtr = ((gcryptCtx*)priv);
	uint32_t len = (gnutlsDhPtr->pLength + 7) / 8;
	return len;
}
