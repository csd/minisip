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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

/* Name
 * 	SipSoftPhoneConfiguration.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#include"SipSoftPhoneConfiguration.h"
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipMessageTransport.h>
#include"../soundcard/SoundIO.h"
#include"../mediahandler/MediaHandler.h"
#include"../minisip/contactdb/PhoneBook.h"
#include"../minisip/contactdb/MXmlPhoneBookIo.h"
#include<fstream>
#include<libmnetutil/NetworkFunctions.h>
#include<libmnetutil/NetworkException.h>

static void installConfigFile( string filename );


SipSoftPhoneConfiguration::SipSoftPhoneConfiguration(): 
	inherited(),
	securityConfig(),
//	dialogContainer(NULL),
	sip(NULL),
//	pstnProxy(NULL),
//	pstnProxyPort(0),
//	pstnNumber(string("")),
	useSTUN(false),
	stunServerPort(0),
	findStunServerFromSipUri(false),
	findStunServerFromDomain(false),
	stunDomain(""),
	useUserDefinedStunServer(false),
	proxyConnection(NULL),
//	use_gw_ip(false),
//	doRegister(false),
//	doRegisterPSTN(false),
	soundDevice(""),
	videoDevice(""),
	autodetectProxy(false),
	dynamicSipPort(false),
	usePSTNProxy(false),
	tcp_server(false),
#ifndef NO_SECURITY
	tls_server(false),
#endif
	p2tGroupListServerPort(0)
{
#ifdef MINISIP_MEMDEBUG 
	//dialogContainer.setUser("SipSoftPhoneConfiguration/sipphone");
	sip.setUser("SipSoftPhoneConfiguration/sipphone");
#endif
}


void SipSoftPhoneConfiguration::save(){
	XMLFileParser * parser = new XMLFileParser( ""/*configFileName*/ );
	
	inherited.save( parser );
	securityConfig.save( parser );
	
	list< MRef<SipIdentity *> >::iterator iIdent;
	uint32_t ii = 0;
		
	string accountPath;

	for( iIdent = identities.begin(); iIdent != identities.end(); ii++, iIdent ++){
		accountPath = string("account[")+itoa(ii)+"]/";

		parser->changeValue( accountPath + "account_name", (*iIdent)->identityIdentifier );
		
		parser->changeValue( accountPath + "sip_uri", (*iIdent)->sipUsername + "@" + (*iIdent)->sipDomain );
		
		parser->changeValue( accountPath + "proxy_addr", (*iIdent)->sipProxy.sipProxyAddressString );

		if( (*iIdent)->sipProxy.sipProxyUsername != "" ){
			parser->changeValue( accountPath + "proxy_username", (*iIdent)->sipProxy.sipProxyUsername );
		
			parser->changeValue( accountPath + "proxy_password", (*iIdent)->sipProxy.sipProxyPassword );
		}

		if( (*iIdent) == pstnIdentity ){
			parser->changeValue( accountPath + "pstn_account", "yes" );
		}

		if( (*iIdent) == inherited.sipIdentity ){
			parser->changeValue( accountPath + "default_account", "yes" );
		}
		
		parser->changeValue( accountPath + "register", 
				(*iIdent)->registerToProxy?"yes":"no" );
	}
	
	parser->changeValue( "sound_device", soundDevice );

	/************************************************************
	 * PhoneBooks
	 ************************************************************/	
	ii = 0;
	list< MRef<PhoneBook *> >::iterator iPb;
	for( iPb = phonebooks.begin(); iPb != phonebooks.end(); ii++, iPb ++ ){
		parser->changeValue( "phonebook[" + itoa(ii) + "]", 
				     (*iPb)->getPhoneBookId() );
	}

	/************************************************************
	 * STUN settings
	 ************************************************************/
	parser->changeValue("use_stun", (useSTUN ? "yes" : "no") );
	parser->changeValue("stun_server_autodetect", findStunServerFromSipUri?"yes":"no");
	if (findStunServerFromDomain){
		parser->changeValue("stun_server_domain", stunDomain );
	}
	else{
		parser->changeValue("stun_server_domain", "");
	}

	parser->changeValue("stun_manual_server", userDefinedStunServer);
	
	/************************************************************
	 * Advanced settings
	 ************************************************************/
	parser->changeValue("tcp_server", tcp_server? "yes":"no");
#ifndef NO_SECURITY
	parser->changeValue("tls_server", tls_server? "yes":"no");
#endif

	parser->saveToFile( configFileName );
	delete( parser );

}

string SipSoftPhoneConfiguration::load( string filename ){
	configFileName = filename;

	installConfigFile( filename );

	proxyConnection = NULL;
	usePSTNProxy = false;

	string ret = "";

	XMLFileParser * parser = new XMLFileParser( filename );
	string account;
	int ii = 0;

	try{
		do{
			string accountPath = string("account[")+itoa(ii)+"]/";
			account = parser->getValue(accountPath+"account_name");
			MRef<SipIdentity*> ident= new SipIdentity();
			ident->setIdentityName(account);

			if( ii == 0 ){
				inherited.sipIdentity = ident;
			}

			ii++;

			try{

				string uri = parser->getValue(accountPath + "sip_uri");
				ident->setSipUri(uri);

				string proxy = parser->getValue(accountPath + "proxy_addr","");
				
				uint16_t proxyPort = parser->getIntValue(accountPath +"proxy_port", 5060);
				
				ident->setDoRegister(parser->getValue(accountPath + "register","")=="yes");

				try{
					if (proxy!=""){
						ident->sipProxy = SipProxy(proxy);
					}
					else{
						autodetectProxy = true;
						proxy = SipProxy::findProxy(uri,proxyPort);
						if (proxy == "unknown"){
							ret += "Minisip could not guess your SIP proxy. Please check your settings and your network access.";
						}
						else{
							ident->sipProxy = SipProxy(proxy);
						}
					}
				}
				catch( NetworkException * exc ){
					ret +="Minisip could not resolve "
					      "the SIP proxy of the account "
					      + ident->identityIdentifier + ".";
					ident->setDoRegister( false );
				}
				ident->sipProxy.sipProxyPort = proxyPort;
				string proxyUser = parser->getValue(accountPath +"proxy_username", "");

				ident->sipProxy.sipProxyUsername = proxyUser;
				string proxyPass = parser->getValue(accountPath +"proxy_password", "");
				ident->sipProxy.sipProxyPassword = proxyPass;

				if (parser->getValue(accountPath + "pstn_account","")=="yes"){
					pstnIdentity = ident;
					usePSTNProxy = true;
					ident->securitySupport = false;
				}



				if (parser->getValue(accountPath + "default_account","")=="yes"){
					inherited.sipIdentity = ident;
				}

				identities.push_back(ident);

			}
			catch(XMLElementNotFound enf){
				cerr << "WARNING: got <"<< enf.what()<<"> when parsing configuration for account "<<account <<endl;;
			}

		}while( true );

	}catch(XMLElementNotFound enf){
		;
	}

	tcp_server = parser->getValue("tcp_server", "yes") == "yes";
	tls_server = parser->getValue("tls_server", "no") == "yes";

	soundDevice =  parser->getValue("sound_device","");

#ifdef VIDEO_SUPPORT
	videoDevice = parser->getValue( "video_device", "" );
	frameWidth = parser->getIntValue( "frame_width", 176 );
	frameHeight = parser->getIntValue( "frame_height", 144 );
#endif

	useSTUN = parser->getValue("use_stun","no")=="yes";
	findStunServerFromSipUri = parser->getValue("stun_server_autodetect","no")==string("yes");

	findStunServerFromDomain = parser->getValue("stun_server_domain","")!="";
	stunDomain = parser->getValue("stun_server_domain","");
	useUserDefinedStunServer = parser->getValue("stun_manual_server","")!="";
	userDefinedStunServer = parser->getValue("stun_manual_server","");
	phonebooks.clear();


	int i=0;
	string s;
	do{
		s = parser->getValue("phonebook["+itoa(i)+"]","");

		if (s!=""){
			MRef<PhoneBook *> pb;
			if (s.substr(0,7)=="file://"){
				pb = PhoneBook::create(new MXmlPhoneBookIo(s.substr(7)));
			}
			// FIXME http and other cases should go here
			if( !pb.isNull() ){
				phonebooks.push_back(pb);
			} else{
				merr << "Could not open the phonebook " << end;
			}
		}
		i++;
	}while(s!="");

	inherited.load( parser );
	securityConfig.load( parser );

	// FIXME: per identity security
	if( inherited.sipIdentity){
		inherited.sipIdentity->securitySupport = securityConfig.secured;
	}

	delete parser;
	return ret;

}


static void installConfigFile(string filename){

        string phonebookFileName;
        char *home = getenv("HOME");
        if (home==NULL){
                merr << "WARNING: Could not determine home directory"<<end;
#ifdef WIN32
                phonebookFileName = string("c:\\minisip.addr");
#else
                phonebookFileName = string("/.minisip.addr");
#endif
        }else{
                phonebookFileName = string(home)+ string("/.minisip.addr");
        }

        string defaultConfig =
                                "<account>\n"
                                "<account_name> My account </account_name>\n"
                                "<sip_uri> username@domain.org </sip_uri>\n"
                                "<proxy_addr> sip.domain.org </proxy_addr>\n"
                                "<register> yes </register>\n"
                                "<proxy_port> 5060 </proxy_port>\n"

                                "<proxy_username> user </proxy_username>\n"
                                "<proxy_password> password </proxy_password>\n"
                                "<pstn_account> no </pstn_account>\n"
                                "<default_account> yes </default_account>\n"
                                "</account>\n"
                                "<transport> UDP </transport>\n"
//                              "<stun_server_ip> stun.ssvl.kth.se</stun>\n"
                                "<tcp_server>yes</tcp_server>\n"
                                "<tls_server>no</tls_server>\n"
                                "<secured>no</secured>\n"
                                "<ka_type>dh</ka_type>\n"
                                "<psk>Unspecified PSK</psk>\n"
                                "<certificate></certificate>\n"
                                "<private_key></private_key>\n"
                                "<ca_certificate></ca_certificate>\n"
                                "<dh_enabled>no</dh_enabled>\n"
                                "<psk_enabled>no</psk_enabled>\n"
                                "<check_cert>no</check_cert>\n"
//                              "<register>no</register>\n"
//                              "<proxy_port>5060</proxy_port>\n"
                                "<local_udp_port> 5060 </local_udp_port>\n"
                                "<local_tcp_port> 5060 </local_tcp_port>\n"
                                "<local_tls_port> 5061 </local_tls_port>\n"
                                "<local_media_port> 10000 </local_media_port>\n"                                "<sound_device>/dev/dsp</sound_device>\n"
                                "<codec_prio_1>PCMu</codec_prio_1>\n"
                                "<codec_prio_2>iLBC</codec_prio_2>\n"
                                "<phonebook>file://" + phonebookFileName +
                                "</phonebook>\n";

	string defaultPhonebook =
                                "<phonebook name=Example>\n"
                                "<contact name=\"Contact\">\n"
                                "<pop desc=\"Phone\" uri=\"0000000000\"></pop>\n"
                                "<pop desc=\"Laptop\" uri=\"sip:contact@minisip.org\"></pop>\n"
                                "</contact>\n"
                                "</phonebook>\n";


	
	ifstream file(filename.c_str());
        if (!file){
                merr << "WARNING: Configuration file ("<< filename << ") was not found - creating it with default values."<< end;
                file.close();
                ofstream newfile(filename.c_str());
                newfile<< defaultConfig;
        }
	/*else{
		merr << "Could not open " << filename << " to write the default configuration file." <<end;

        }*/

        ifstream phonebookFile( phonebookFileName.c_str() );
        if (!phonebookFile){
                phonebookFile.close();
                ofstream newfile(phonebookFileName.c_str());
                newfile<< defaultPhonebook;
        }
	/*else{
		merr << "Could not open " << phonebookFileName << " to write the default phonebook file." <<end;
        }*/

}


