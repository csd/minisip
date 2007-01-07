/*
  Copyright (C) 2005, 2004 Erik Eliasson, Pan Xuan
  
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
 *          Pan Xuan <xuan@kth.se>
*/

#include <config.h>

#include <libmcrypto/SmartCard.h>
#include <libmcrypto/SmartCardException.h>
#include <libmutil/stringutils.h>

#include <winscard.h>

using namespace std;

SmartCard::SmartCard(){
	this -> userPinCode = NULL;
	this -> adminPinCode = NULL;
	unsigned long dwProtocol;
	int readerNumbers;
	char * tempReaderPtr = NULL;
	LONG rv;
	
	/*Creates a communication context to the PC/SC resource manager*/
	rv = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hContext);
	if(rv != SCARD_S_SUCCESS){
		throw SmartCardException("couldn't communicate with the PS/SC resource manager");
	}

	/* Retrieve the available readers list.
	 * 1. Call with a null buffer to get the number of bytes to allocate
	 * 2. malloc the necessary memory
	 * 3. call with the real allocated buffer
	 */
	rv = SCardListReaders(hContext, NULL, NULL, &readerLength);
	if(rv != SCARD_S_SUCCESS)
	{
		throw SmartCardException("rv");
	}
	readerNamesPtr = (char *) malloc (readerLength * sizeof(char));
	rv = SCardListReaders(hContext, NULL, readerNamesPtr, &readerLength);
	if(rv != SCARD_S_SUCCESS){
		throw SmartCardException("rv");
	}

	/* Extract readers from the '\0' separated string and get the total number of readers */
	readerNumbers = 0;
	tempReaderPtr = readerNamesPtr;
	while (*tempReaderPtr != '\0')
	{
		tempReaderPtr += strlen(tempReaderPtr) + 1;
		readerNumbers++;
	}
	
	/*Fill in the map with the reader's number and name*/
	
	if (readerNumbers == 0){
		throw SmartCardException("no supported card reader found");
	}
	
	readerNumbers = 1;
	tempReaderPtr = readerNamesPtr;
	while (*tempReaderPtr != '\0')
	{
		readerMap.insert(make_pair(readerNumbers,tempReaderPtr));
		tempReaderPtr += strlen(tempReaderPtr) + 1;
		readerNumbers++;
	}
	
	/*Connect to the smart card*/
	map<int, char*>::iterator i = readerMap.begin();
	rv = SCardConnect(hContext, (*i).second, SCARD_SHARE_EXCLUSIVE,
		SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &dwProtocol);
	if(rv == SCARD_S_SUCCESS){
		if(dwProtocol == SCARD_PROTOCOL_T0)
			protPci = SCARD_PCI_T0;
		else
			protPci = SCARD_PCI_T1;
		establishedConnection = 1;
	}
	else 
		establishedConnection = 0;
}

SmartCard::~SmartCard()
{
	if (establishedConnection){
		long rvDisconnect, rvRelease;
		rvDisconnect = SCardDisconnect(hCard, SCARD_UNPOWER_CARD);
		rvRelease = SCardReleaseContext(hContext);
	}	

	if(readerNamesPtr)
		delete [] readerNamesPtr;
	
/*	map <int, char *>::iterator i;
	for(i = readerMap.begin(); i != readerMap.end(); i++){
		delete (*i).second;
	}

*/
	delete [] userPinCode;
	delete [] adminPinCode;
}



void SmartCard::setPin(const char * pinCode){
	this->userPinCode = new unsigned char[4];
	memset(this->userPinCode, 0, 4);
	memcpy(this->userPinCode, pinCode,4);
}

void SmartCard::setAdminPin(const char * adminPinCode){
	this->adminPinCode = new unsigned char[8];
	memset(this->adminPinCode, 0, 8);
	memcpy(this->adminPinCode, adminPinCode, 8);
}


/* one round trip of APDU exchange */
bool SmartCard::transmitApdu(unsigned long sendLength, unsigned char * sendBufferPtr, 
							 unsigned long & recvLength, unsigned char * recvBufferPtr)
{	
#ifdef DEBUG_OUTPUT
	cerr << "TRANSMITAPDU: "<< binToHex(sendBufferPtr,sendLength)<<endl;
#endif
	SCARD_IO_REQUEST recvPci;
	long rvTransmit;
	rvTransmit = SCardTransmit(hCard, protPci, sendBufferPtr, sendLength, &recvPci, recvBufferPtr, &recvLength);
#ifdef DEBUG_OUTPUT
	cerr << "Received "<< recvLength << " bytes: "<< binToHex(recvBufferPtr,recvLength)<<endl;
#endif
	if (rvTransmit == SCARD_S_SUCCESS){
		return true;
	}
	else
		cerr << "the return code is: " << rvTransmit  << endl;
		throw SmartCardException("APDU transmission failure");
}

void SmartCard::startTransaction(){
		SCardBeginTransaction(hCard);
}

void SmartCard::endTransaction(){
		SCardEndTransaction(hCard, SCARD_LEAVE_CARD);
}

