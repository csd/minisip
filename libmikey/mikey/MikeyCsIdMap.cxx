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
 *          Joachim Orrblad <joachim@orrblad.com>
*/

#include<config.h>
#include<libmikey/MikeyException.h>
#include<libmikey/MikeyCsIdMap.h>
#include<libmutil/itoa.h>


MikeySrtpCs::MikeySrtpCs( uint8_t policyNo, uint32_t ssrc, uint32_t roc ):
        policyNo( policyNo ), ssrc( ssrc ), roc( roc ){};
//added 041201 JOOR
MikeyIPSEC4Cs::MikeyIPSEC4Cs( uint8_t policyNo, uint32_t spi, uint32_t spiSrcaddr, uint32_t spiDstaddr ):
        policyNo( policyNo ), spi( spi ), spiSrcaddr( spiSrcaddr ), spiDstaddr( spiDstaddr ){};

MikeyCsIdMapSrtp::MikeyCsIdMapSrtp(){
        cs = vector<MikeySrtpCs *>::vector();
}

//added 041201 JOOR
MikeyCsIdMapIPSEC4::MikeyCsIdMapIPSEC4(){
        cs = list<MikeyIPSEC4Cs *>::list();
}

MikeyCsIdMapSrtp::MikeyCsIdMapSrtp( byte_t * data, int length ){
        if( length % 9 ){
                throw new MikeyException(
                                "Invalid length of SRTP_ID map info" );
        }

        uint8_t nCs = length / 9;
        uint8_t i;
        uint32_t ssrc, roc;
        byte_t policyNo;

        for( i = 0; i < nCs; i++ ){
                policyNo = data[ i*9 ];
                ssrc = (uint32_t)data[ i*9 + 1 ] << 24 |
                       (uint32_t)data[ i*9 + 2 ] << 16 |
                       (uint32_t)data[ i*9 + 3 ] <<  8 |
                       (uint32_t)data[ i*9 + 4 ];
                roc  = (uint32_t)data[ i*9 + 5 ] << 24 |
                       (uint32_t)data[ i*9 + 6 ] << 16 |
                       (uint32_t)data[ i*9 + 7 ] <<  8 |
                       (uint32_t)data[ i*9 + 8 ];
                addStream( ssrc, roc, policyNo );
        }
}


MikeyCsIdMapIPSEC4::MikeyCsIdMapIPSEC4( byte_t * data, int length ){
        if( length % 13 ){
                throw new MikeyException(
                                "Invalid length of IPSEC4_ID map info" );
        }

        uint8_t nCs = length / 13;
        uint8_t i;
        uint32_t spi, spiSrcaddr, spiDstaddr;
        byte_t policyNo;

        for( i = 0; i < nCs; i++ ){
                policyNo = data[ i*13 ];
                spi = (uint32_t)data[ i*13 + 1 ] << 24 |
                      (uint32_t)data[ i*13 + 2 ] << 16 |
                      (uint32_t)data[ i*13 + 3 ] <<  8 |
                      (uint32_t)data[ i*13 + 4 ];
                spiSrcaddr  = (uint32_t)data[ i*13 + 5 ] << 24 |
                      (uint32_t)data[ i*13 + 6 ] << 16 |
                      (uint32_t)data[ i*13 + 7 ] <<  8 |
                      (uint32_t)data[ i*13 + 8 ];
                spiDstaddr  = (uint32_t)data[ i*13 + 9 ] << 24 |
                      (uint32_t)data[ i*13 + 10 ] << 16 |
                      (uint32_t)data[ i*13 + 11 ] <<  8 |
                      (uint32_t)data[ i*13 + 12 ];
                addSA( spi, spiSrcaddr, spiDstaddr, policyNo );
        }
}

MikeyCsIdMapSrtp::~MikeyCsIdMapSrtp(){
        vector<MikeySrtpCs *>::iterator i;

        for( i = cs.begin(); i!= cs.end() ; i++ )
                delete *i;
}
//added 041201 JOOR
MikeyCsIdMapIPSEC4::~MikeyCsIdMapIPSEC4(){
        list<MikeyIPSEC4Cs *>::iterator i;

        for( i = cs.begin(); i!= cs.end() ; i++ )
                delete *i;
}

int MikeyCsIdMapSrtp::length(){
        return 9 * (int)cs.size();
}
//added 041201 JOOR
int MikeyCsIdMapIPSEC4::length(){
        return 13 * (int)cs.size();
}


void MikeyCsIdMapSrtp::writeData( byte_t * start, int expectedLength ){
        if( expectedLength < length() ){
                throw new MikeyExceptionMessageLengthException(
                                "CsSrtpId is too long" );
        }

        int j = 0,k;
        vector<MikeySrtpCs *>::iterator i;

        for( i = cs.begin(); i != cs.end(); i++ ){
                start[ 9*j ] = (*i)->policyNo & 0xFF;
                for( k = 0; k < 4; k++ ){
                        start[9*j+1+k] = ((*i)->ssrc >> 8*(3-k)) & 0xFF;
                }
                for( k = 0; k < 4; k++ ){
                        start[9*j+5+k] = ((*i)->roc >> 8*(3-k)) & 0xFF;
                }
                j++;
        }
}
//added 041202 JOOR
void MikeyCsIdMapIPSEC4::writeData( byte_t * start, int expectedLength ){
        if( expectedLength < length() ){
                throw new MikeyExceptionMessageLengthException(
                                "CsIPSEC4Id is too long" );
        }

        int j = 0,k;
        list<MikeyIPSEC4Cs *>::iterator i;

        for( i = cs.begin(); i != cs.end(); i++ ){
                start[ 13*j ] = (*i)->policyNo & 0xFF;
                for( k = 0; k < 4; k++ ){
                        start[13*j+1+k] = ((*i)->spi >> 8*(3-k)) & 0xFF;
                }
                for( k = 0; k < 4; k++ ){
                        start[13*j+5+k] = ((*i)->spiSrcaddr >> 8*(3-k)) & 0xFF;
                }
                for( k = 0; k < 4; k++ ){
                        start[13*j+9+k] = ((*i)->spiDstaddr >> 8*(3-k)) & 0xFF;
                }
                j++;
        }
}


byte_t MikeyCsIdMapSrtp::findCsId( uint32_t ssrc ){
        vector<MikeySrtpCs *>::iterator i;
        uint8_t j = 1;

        for( i = cs.begin(); i != cs.end()  ; i++,j++ ){
                if( (*i)->ssrc == ssrc ){
                        return j;
                }
        }
        return 0;
}
//added 041201 JOOR
byte_t MikeyCsIdMapIPSEC4::findCsId( uint32_t spi, uint32_t spiSrcaddr, uint32_t spiDstaddr ){
        list<MikeyIPSEC4Cs *>::iterator i;
        uint8_t j = 1;

        for( i = cs.begin(); i != cs.end()  ; i++,j++ ){
                if( (*i)->spi == spi && (*i)->spiSrcaddr == spiSrcaddr && (*i)->spiDstaddr == spiDstaddr){
                        return j;
                }
        }
        return 0;
}
//added 041214 JOOR
byte_t MikeyCsIdMapSrtp::findpolicyNo( uint32_t ssrc ){
        vector<MikeySrtpCs *>::iterator i;
        for( i = cs.begin(); i != cs.end()  ; i++ ){
                if( (*i)->ssrc == ssrc ){
                        return (*i)->policyNo;
                }
        }
        return 0;
}
//added 050110 JOOR
MikeyIPSEC4Cs * MikeyCsIdMapIPSEC4::getCsIdnumber(int number){
        list<MikeyIPSEC4Cs *>::iterator i;
        int j = 1;
        for( i = cs.begin(); i != cs.end()  ; i++ ){
                if(j == number)
                        return (*i);
        j++;
        }
        return NULL;
}


//added 041201 JOOR
byte_t MikeyCsIdMapIPSEC4::findpolicyNo( uint32_t spi, uint32_t spiSrcaddr, uint32_t spiDstaddr ){
        list<MikeyIPSEC4Cs *>::iterator i;
        for( i = cs.begin(); i != cs.end()  ; i++ ){
                if( (*i)->spi == spi && (*i)->spiSrcaddr == spiSrcaddr && (*i)->spiDstaddr == spiDstaddr ){
                        return (*i)->policyNo;
                }
        }
        return 0;
}


uint32_t MikeyCsIdMapSrtp::findRoc( uint32_t ssrc ){
        vector<MikeySrtpCs *>::iterator i;

        for( i = cs.begin(); i != cs.end()  ; i++ ){
                if( (*i)->ssrc == ssrc ){
                        return (*i)->roc;
                }
        }
        return 0;
}

void MikeyCsIdMapSrtp::setSsrc( uint32_t ssrc, uint8_t csId ){
        if( csId > cs.size() ){
                return;
        }

        (cs[ csId - 1 ])->ssrc = ssrc;
}

void MikeyCsIdMapSrtp::setRoc( uint32_t roc, uint8_t csId ){
        if( csId > cs.size() ){
                return;
        }

        (cs[ csId - 1 ])->roc = roc;
}



void MikeyCsIdMapSrtp::addStream( uint32_t ssrc, uint32_t roc, byte_t policyNo, byte_t csId ){
        if( csId == 0 ){
                cs.push_back( new MikeySrtpCs( policyNo, ssrc, roc ) );
                return;
        }


        if( csId > cs.size()  ){
                return;
        }

        (cs[csId - 1])->ssrc = ssrc;
        (cs[csId - 1])->policyNo = policyNo;
        (cs[csId - 1])->roc = roc;
        return;
}

//added 041201 JOOR
void MikeyCsIdMapIPSEC4::addSA( uint32_t spi, uint32_t spiSrcaddr, uint32_t spiDstaddr, byte_t policyNo, byte_t csId){
        if( csId == 0 ){
                cs.push_back( new MikeyIPSEC4Cs( policyNo, spi, spiSrcaddr, spiDstaddr ) );
                return;
        }
        list<MikeyIPSEC4Cs *>::iterator i;
        uint8_t j = 1;
        for( i = cs.begin(); i != cs.end() ; i++,j++ ){
                if( j == csId ){
                        (*i)->spi = spi;
                        (*i)->policyNo = policyNo;
                        (*i)->spiSrcaddr = spiSrcaddr;
                        (*i)->spiDstaddr = spiDstaddr;
                }
        }

        return;
}

std::string MikeyCsIdMapSrtp::debugDump(){
	std::string output = "";
	std::vector<MikeySrtpCs *>::iterator iCs;
	uint8_t csId = 1;

	for( iCs = cs.begin(); iCs != cs.end(); iCs++, csId++ ){
		output += "csId: <" + itoa( csId ) + ">\n";
		output += "   policyNo: <" + itoa( (*iCs)->policyNo ) + ">\n";
		output += "   SSRC: <" + itoa( (*iCs)->ssrc ) + ">\n";
		output += "   ROC: <" + itoa( (*iCs)->roc ) + ">\n";
		output += "\n";
	}
	return output;
}

std::string MikeyCsIdMapIPSEC4::debugDump(){
	std::string output = "";
	std::list<MikeyIPSEC4Cs *>::iterator iCs;
	uint8_t csId = 1;

	for( iCs = cs.begin(); iCs != cs.end(); iCs++, csId++ ){
		output += "csId: <" + itoa( csId ) + ">\n";
		output += "   spi: <" + itoa( (*iCs)->spi ) + ">\n";
		output += "   policyNo: <" + itoa( (*iCs)->policyNo ) + ">\n";
		output += "   Source Addr.: <" + itoa( (*iCs)->spiSrcaddr ) + ">\n";
		output += "   Dest. Addr.: <" + itoa( (*iCs)->spiDstaddr ) + ">\n";
		output += "\n";
	}
	return output;
}
