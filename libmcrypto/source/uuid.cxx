/*
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
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include <config.h>

#include <libmcrypto/uuid.h>
#include "uuid/uuid.h"

using namespace std;

#define UUID_STRLEN 36

#define uuid ((uuid_t*)m_priv)

// Uuid::Uuid(): m_priv(NULL){
// }

Uuid::Uuid(const void *priv){
	m_priv = new uuid_t;

	*uuid = *((uuid_t*)priv);
}

Uuid::~Uuid(){
	delete uuid;
}

Uuid* Uuid::create(){
	uuid_t u;

	uuid_create(&u);
	return new Uuid(&u);
}

int Uuid::operator=(const Uuid &u){
	return uuid_compare(uuid, ((uuid_t*)u.m_priv));
}

string Uuid::toString(){
	char buf[UUID_STRLEN+1]="";

	return uuid_to_str(uuid, buf, sizeof(buf));
}

