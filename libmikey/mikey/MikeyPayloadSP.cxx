/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien, Joachim Orrblad
  
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
 *	    Joachim Orrblad <joachim@orrblad.com>
*/

#include<config.h>
#include<libmikey/MikeyPayloadSP.h>
#include<libmikey/MikeyException.h>
#include<stdlib.h>

using namespace std;

//
// MikeyPolicyParam
//
MikeyPolicyParam::MikeyPolicyParam( uint8_t type, uint8_t length, byte_t * value ):
	type(type), length(length){
	this->type=type;
	this->length=length;
	this->value = (byte_t*) calloc (length,sizeof(byte_t));
	for(int i=0; i< length; i++)
			this->value[i] = value[i];
}
//Destructor
MikeyPolicyParam::~MikeyPolicyParam(){
	free(value);
}
//
// MikeyPayloadSP
//
//Constructor when receiving Mikey message i.e. contruct MikeyPayloadSP from bytestream
MikeyPayloadSP::MikeyPayloadSP(byte_t *start, int lengthLimit):MikeyPayload(start){
	this->payloadTypeValue = MIKEYPAYLOAD_SP_PAYLOAD_TYPE;
	this->policy_param_length = 0;
	this->nextPayloadTypeValue = start[0]; 
	this->policy_no = start[1];
	this->prot_type = start[2];
	int i = 5;
	uint16_t j = ((uint16_t)start[3] << 8 | (uint16_t)start[4]) + 5;
	//byte_t *value;
	endPtr = startPtr + j;
	//while(i < lengthLimit) {
	while(i < j ) {
		this->addMikeyPolicyParam(start[i], start[i+1], &start[i+2]);
		i = i + 2 + start[i+1];
	}
	assert (endPtr - startPtr == length() );
}
//Constructor when constructing new Mikey message, policy type entries added later with MikeyPayloadSP::addMikeyPolicyParam
MikeyPayloadSP::MikeyPayloadSP(uint8_t policy_no, uint8_t prot_type){
	this->payloadTypeValue = MIKEYPAYLOAD_SP_PAYLOAD_TYPE;
	this->policy_param_length = 0;
	this->policy_no = policy_no;
	this->prot_type = prot_type;
}

//Destructor
MikeyPayloadSP::~MikeyPayloadSP(){
	list<MikeyPolicyParam *>::iterator i;
	for( i = param.begin(); i != param.end() ; i++ )
		delete *i;
	param.clear();
}
//Add a policytype i.e. add one MikeyPolicyParam in list<MikeyPolicyParam *> param
void MikeyPayloadSP::addMikeyPolicyParam( uint8_t type, uint8_t length, byte_t * value){
	if(this->getParameterType(type) != NULL)
		this->deleteMikeyPolicyParam(type);
	param.push_back (new MikeyPolicyParam(type, length, value));
	this->policy_param_length = this->policy_param_length + length + 2;
}
//Get the MikeyPolicyParam in list<MikeyPolicyParam *> param with type type
MikeyPolicyParam * MikeyPayloadSP::getParameterType(uint8_t type){
	list<MikeyPolicyParam *>::iterator i;
		for( i = param.begin(); i != param.end()  ; i++ ){
			if( (*i)->type == type )
				return *i;
		}
	return NULL;
}
//Generate bytestream
void MikeyPayloadSP::writeData(byte_t *start, int expectedLength){
	assert( expectedLength == this->length() );
	start[0] = this->nextPayloadTypeValue;
	start[1] = this->policy_no;
	start[2] = this->prot_type;
	start[3] = (byte_t) ((this->policy_param_length & 0xFF00) >> 8);
	start[4] = (byte_t) (this->policy_param_length & 0xFF);
	//Add policy params
	list<MikeyPolicyParam *>::iterator i = param.begin();
	int j=5,k;
	while (i != param.end() && j < expectedLength){
		start[j++] = (*i)->type;
		start[j++] = (*i)->length;
		for(k=0; k < ((*i)->length); k++)
			start[j++] = ((*i)->value)[k];
		i++;
	}
}
//Return total length of the MikeyPayloadSP data in bytes
int MikeyPayloadSP::length(){
	return 5 + this->policy_param_length;
}
//Return number of policy param entries
int MikeyPayloadSP::noOfPolicyParam(){
	return (int)param.size();
}
//Delete the MikeyPolicyParam in list<MikeyPolicyParam *> param with type type
void MikeyPayloadSP::deleteMikeyPolicyParam(uint8_t type){
	list<MikeyPolicyParam *>::iterator i;
	for( i = param.begin(); i != param.end()  ; i++ )
		if( (*i)->type == type ){
			this->policy_param_length = this->policy_param_length - (*i)->length - 2;
			delete *i;
			i=param.erase(i);
		}
}

std::string MikeyPayloadSP::debugDump(){
	string ret = "MikeyPayloadSP: next_payload<" + itoa( nextPayloadTypeValue ) + "> ";

	ret += string("policyNo: <") + itoa( policy_no ) + "> ";
	ret += string("protType: <") + itoa( prot_type ) + ">\n";

	list<MikeyPolicyParam *>::iterator i = param.begin();
	for( ; i != param.end(); i++ ){
		ret += string("type: <") + itoa( (*i)->type ) + "> ";
		ret += string("value: <")
		       + binToHex( (*i)->value, (*i)->length ) + ">\n";
	}
	
	return ret;
}
