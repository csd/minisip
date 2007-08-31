/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Erik Ehrlund <eehrlund@kth.se>
*/

#include<config.h>

#include"OnlineMXmlConfBackend.h"
#include"libminisip/config/OnlineConfBackend.h"
#include<libminisip/config/UserConfig.h>

#include<libmutil/XMLParser.h>
#include<libmutil/stringutils.h>

#include<stdlib.h>
#include<libmcrypto/TlsException.h>
#include<libmcrypto/TlsSrpSocket.h>
#include<libmcrypto/cert.h>
#include<libmnetutil/NetworkException.h>
#include <vector>
#include <iostream>
#include <fstream>
#include<string.h>
using namespace std;

static std::list<std::string> pluginList;
static bool initialized;


extern "C" LIBMINISIP_API
std::list<std::string> *onlineconf_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MPlugin * onlineconf_LTX_getPlugin( MRef<Library*> lib ){
	return new OnlineMXmlConfigPlugin( lib );
}

OnlineMXmlConfBackend::OnlineMXmlConfBackend(){

	int port = 5556;
	int success ;
	int error=0;
	string user("test");
	string pass("qwerty");
	string addr("127.0.0.1");
	try {
		conf = new OnlineConfBack(addr, port, user , pass); 
	}
	catch (ResolvError &){
	/* ERROR RESOLVING THE ADDRESS, PROMT USER FOR NEW ADDRESS*/
		cout<<"Error resolving address"<<endl;
		exit(1);
	}
	catch (ConnectFailed &){
	/*CONNECTION FAILED, THE SERVER IS NOT RESPONDIG OR A BAD ADDRESS WAS ENTERED
	 * PROMT USER AND ASK FOR NEW ADDRESS*/
		cout<<"Could not establish a connection to: "<< addr<<endl;
		exit(1);
	}
	catch (TLSInitFailed &){
	/* GNUTLS INIT FAILED, DEBUG*/
		cout<<"GNUTLS INIT FAILED"<<endl;
		exit(1);
	}
	catch (char *str){
	/*TLS HANDSHAKE FAILED PROMT USER FOR NEW USERNAME, PASSWORD (AND ADDRESS)*/
		exit(1);
	}
	conf->setOnlineCert(NULL);
	vector<struct contdata*> res;
	success = conf->downloadReq(user,"credentials", res);
	if(success>0 && res.size()>1)
	{

		Certificate *cert = Certificate::load((unsigned char *)res.at(0)->data,(size_t) res.at(0)->size,
				"httpsrp:///"+user + "/credentials/cert.pem" );
		if (cert != NULL)
		{
			cert->setEncpk(res.at(1)->data, res.at(1)->size,pass,"httpsrp:///"+user + "/credentials/pkkey.pem");
			//cout<<"COMMON NAME ********: "<<cert->getName()<<endl;
			conf->setOnlineCert(cert);
		}
		else{
			cout<<"failed to load certificate"<<endl;
			error =1;
		}

	}
	else
	{
		error=1;
		/* Error */
	}

	res.clear();
	success =conf->downloadReq(user, "userprofile/default.conf", res);
	if(success>0 && error ==0 )
		{
			if(res.size()>2){
				Certificate *cert = conf->getOnlineCert();
				int outsize =0;
				unsigned char outbuf[res.at(2)->size];

				int ret =cert->denvelopeData((unsigned char*)(res.at(2)->data), res.at(2)->size , 
				outbuf, &outsize,(unsigned char*)(res.at(1)->data), res.at(1)->size, 
				(unsigned char*)(res.at(0)->data));
				
				if(ret >=0){
					string profile((char*)outbuf,outsize);				
					pars = new XMLstringParser(profile);
				}
				else{
					cout<<"Failed to decrypt the stored information someone"<<endl;
					cout<<"might have moddifed it or an error occured in transit"<<endl;
						error=1;
				}
			}
			else{ 
				error=1;
			}
		}
	else{
		error=1;
	}
	if(error !=0)
	{
		cout<<"failed to download userprofile, creating default one"<<endl;
		try{	
			pars = new XMLstringParser( "" );
			cout<<pars->xmlstring()<<endl;
		}
		catch(XMLException &exc ){
			mdbg << "OnlineMXmlConfBackend caught XMLException: " << exc.what() << endl;
			cerr << "Caught XMLException" << endl;
			throw ConfBackendException();
		}
	}
}

OnlineMXmlConfBackend::~OnlineMXmlConfBackend(){
	delete pars;
	delete conf;
}

OnlineConfBack* OnlineMXmlConfBackend::getConf()
{
	return conf;
}


void OnlineMXmlConfBackend::commit(){
	string commit;
	commit = pars->xmlstring();
	Certificate *cert= conf->getOnlineCert();

	int retsize =0, enckeylgth=0;
	unsigned char buf[commit.size()+128];
	unsigned char key[256];
	unsigned char *iv;
	if(cert!=NULL){
		cert->envelopeData((unsigned char*)commit.c_str(), commit.size(), buf, &retsize,key, &enckeylgth, &iv);
		string enckod =conf->base64Encode((char*)buf, retsize);
		string ivstr = conf->base64Encode((char*)iv, 16);
		string keystr = conf->base64Encode((char*)key, enckeylgth);
		string attach;
		attach = conf->attachFile("", ivstr);
		attach = conf->attachFile(attach,keystr);
		attach = conf->attachFile(attach,enckod);
		conf->uploadReq(conf->getUser(),"userprofile",attach);
	}
}

void OnlineMXmlConfBackend::save( const std::string &key, const std::string &value ){
	try{
		pars->changeValue( key, value );
	}
	catch( XMLException &exc ){
		mdbg << "OnlineMXmlConfBackend caught XMLException: " << exc.what() << endl;
		throw ConfBackendException();
	}

}

void OnlineMXmlConfBackend::save( const std::string &key, const int32_t value ){
	try{
		pars->changeValue( key, itoa( value ) );
	}
	catch( XMLException &exc ){
		mdbg << "OnlineMXmlConfBackend caught XMLException: " << exc.what() << endl;
		throw ConfBackendException();
	}
}


std::string OnlineMXmlConfBackend::loadString( const std::string &key, const std::string &defaultValue ){
	std::string ret = "";

	try{
		ret = pars->getValue( key, defaultValue );
	}
	catch( XMLException &exc ){
		mdbg << "OnlineMXmlConfBackend caught XMLException: " << exc.what() << endl;
		throw ConfBackendException();
	}

	return ret;

}

int32_t OnlineMXmlConfBackend::loadInt( const std::string &key,
		const int32_t defaultValue ){
	int32_t ret = -1;

	try{
		ret = pars->getIntValue( key, defaultValue );
	}
	catch( XMLException &exc ){
		mdbg << "OnlineMXmlConfBackend caught XMLException: " << exc.what() << endl;
		throw ConfBackendException();
	}

	return ret;
}

string OnlineMXmlConfBackend::getDefaultConfigFilename(){
	return UserConfig::getFileName( "minisip.conf" );
}

OnlineMXmlConfigPlugin::OnlineMXmlConfigPlugin( MRef<Library *> lib ): ConfigPlugin( lib ){
}

MRef<ConfBackend *> OnlineMXmlConfigPlugin::createBackend(const std::string &configFilePath)const{
// MRef<ConfBackend *> OnlineMXmlConfigPlugin::createBackend(MRef<Gui*> gui, const string &)const{
	return new OnlineMXmlConfBackend();
}

std::string OnlineMXmlConfigPlugin::getName()const{
	return "onlineconf";
}

std::string OnlineMXmlConfigPlugin::getDescription()const{
	return "Online MXmlConf backend";
}

uint32_t OnlineMXmlConfigPlugin::getVersion()const{
	return 0x00000001;
}
