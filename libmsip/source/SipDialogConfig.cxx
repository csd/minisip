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
#include<config.h>

#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipDialog.h>
//#include<libmsip/SipSoftPhoneConfiguration.h>
#include<libmsip/SipMessageTransport.h>
#include<libmutil/itoa.h>
#include<libmutil/dbg.h>
#include<libmutil/MemObject.h>
#include<libmsip/SipDialogContainer.h>
//#include<libmsip/dialogs/DefaultDialogHandler.h>
#include<libmsip/SipInvite.h>



SipIdentity::SipIdentity(string addr)/*: sipProxyPort(0), sipProxyIpAddr(NULL)*/ : securitySupport(false),registerToProxy(false){
	setSipUri(addr);
	securitySupport = false;
}

void SipIdentity::setSipUri(string addr){
        if (addr.substr(0,4)=="sip:")
                addr = addr.substr(4);
        if (addr.find("@")==string::npos){
                cerr << "WARNING: malformed sip address: "<< addr<<endl;
        }

        sipUsername = addr.substr(0, addr.find("@"));
        sipDomain = addr.substr(addr.find("@")+1);
        //cerr << "sipUsername=<"<< sipUsername << "> sipDomain=<" << sipDomain << ">"<< endl;
}


SipCommonConfig::SipCommonConfig(): /*MObject("SipCommonConfig"),*/
	localUdpPort(0),
	localTcpPort(0),
#ifndef NO_SECURITY
	localTlsPort(0),
#endif
	//proxyAddr(NULL),
//	proxyPort(0),
	transport(("")),
	autoAnswer(false){
//            merr << "running SipCommonConfig constructor"<< end;

#ifdef MINISIP_MEMDEBUG
	sipTransport.setUser("SipCommonConfig/messageTransport");
#endif
}

void SipCommonConfig::save( XMLFileParser * parser ){
	/***********************************************************
	 * General settings
	 ***********************************************************/
#if 0
	parser->changeValue("sip_address", sipIdentity->sipUsername +"@" + sipIdentity->sipDomain);
	parser->changeValue("proxy_username",sipIdentity->sipProxy.sipProxyUsername);
	parser->changeValue("proxy_password",sipIdentity->sipProxy.sipProxyPassword);
#endif


	/***********************************************************
	 * Advanced settings
	 ***********************************************************/
	parser->changeValue("local_udp_port", itoa(localUdpPort));
	parser->changeValue("local_tcp_port", itoa(localTcpPort));
#ifndef NO_SECURITY
	parser->changeValue("local_tls_port", itoa(localTlsPort));
#endif
	parser->changeValue("auto_answer", autoAnswer?"yes":"no");

	parser->changeValue("transport", transport);

}

void SipCommonConfig::load( XMLFileParser * parser ){
	transport = parser->getValue("transport", "UDP");
#ifdef OLD_MEDIA
	if (parser->getValue("codec_prio_1","none")=="none"){
		codecs.push_back(new G711CODEC());
		codecs.push_back(new ILBCCODEC());
	} else {
		if (parser->getValue("codec_prio_1","none")=="PCMu")
			codecs.push_back(new G711CODEC());

		if (parser->getValue("codec_prio_1","none")=="iLBC")
			codecs.push_back(new ILBCCODEC());

		if (parser->getValue("codec_prio_2","none")=="PCMu")
			codecs.push_back(new G711CODEC());

		if (parser->getValue("codec_prio_2","none")=="iLBC")
			codecs.push_back(new ILBCCODEC());
	}
#endif

	localUdpPort = parser->getIntValue("local_udp_port",5060);
	externalContactUdpPort = localUdpPort;

	localTcpPort = parser->getIntValue("local_tcp_port",5060);
	localTlsPort = parser->getIntValue("local_tls_port",5061);

	autoAnswer = parser->getValue("auto_answer", "no") == "yes";

}





//SipDialogConfig::SipDialogConfig(MRef<SipCommonConfig*> commonconf): MObject("SipDialogConfig") {
SipDialogConfig::SipDialogConfig(SipCommonConfig &commonconf)/*: MObject("SipDialogConfig")*/ : proxyConnection(NULL) {
//    merr << "SipDialogConfig::SipDialogCOnfig: copying inherited..."<< end;
//    inherited = phoneconf->inherited;
//	SipCommonConfig tmp(commonconf);
//	inherited = tmp;

//	inherited = **commonconf;
	inherited = commonconf;
	
//    merr << "SipDialogConfig::SipDialogCOnfig: copying done"<< end;

    last_invite=NULL;
    
#ifdef MINISIP_MEMDEBUG 
    last_invite.setUser("SipDialogConfig/last_invite");
#endif

/////    seqNo=100;

    //	callId = itoa(rand())+"@"+inherited.localIpString;
//    callId = itoa(rand())+"@"+inherited.externalContactIP;
    local_ssrc = rand();
    
}

void SipDialogConfig::useIdentity(
			//IPAddress *proxy,
			MRef<SipIdentity*> identity,
//			string domain,
//			int proxyPort, 
//			string user, 
//			string proxyUsername, 
//			string proxyPassword,
			bool useSecurity,
			string transport)
{
//	inherited.proxyAddr = phoneconf->pstnProxy;
//	inherited.proxyAddr = proxy;
//	inherited.sipIdentity.sipProxy.sipProxyIpAddr = proxy;
	inherited.sipIdentity = identity;
	
//	inherited.sipIdentity.sipDomain = domain;
	//inherited.sipDomain = domain;
	
//	inherited.proxyPort = phoneconf->pstnProxyPort;

//	inherited.sipIdentity.sipProxyPort = proxyPort;
//	inherited.proxyPort = proxyPort;
	
//	proxyIPAddress = phoneconf->pstn_proxy;

//	inherited.proxyUsername = phoneconf->pstnProxyUsername;

//	inherited.sipIdentity.sipProxyUsername = proxyUsername;
	//inherited.proxyUsername = proxyUsername;
	
//	inherited.proxyPassword = phoneconf->pstnProxyPassword;

//	inherited.sipIdentity.sipProxyPassword = proxyPassword;
//	inherited.proxyPassword = proxyPassword;
	
//	inherited.userUri = phoneconf->pstnNumber + "@" + inherited.proxyAddr->get_string(); 
//	inherited.userUri = user + "@" + proxy->getString(); 
	inherited.transport = transport;
}

