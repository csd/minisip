#include<config.h>
#include"SipDialogSecurityConfig.h"
#include<libmutil/cert.h>
#include<libmutil/XMLParser.h>
#include<libmutil/itoa.h>
#include<libmsip/SipDialogConfig.h>


SipDialogSecurityConfig::SipDialogSecurityConfig():
	secured(false),
	ka_type(0),
	use_srtp(false),		
	use_ipsec(false),
	cert(NULL),
	cert_db(NULL),
	psk_enabled(false),
	psk(NULL),
	psk_length(0),
	dh_enabled(false),
	check_cert(false)
{
}

void SipDialogSecurityConfig::save( XMLFileParser * parser ){

        parser->changeValue("secured", secured?string("yes"): string("no"));

	parser->changeValue("use_srtp", use_srtp?string("yes"): string("no"));
	parser->changeValue("use_ipsec", use_ipsec?string("yes"): string("no"));

        parser->changeValue("psk_enabled", psk_enabled?string("yes"): string("no"));
        parser->changeValue("dh_enabled", dh_enabled?string("yes"): string("no"));

        char * pskString = new char[psk_length+1];
        memcpy( pskString, psk, psk_length );
        pskString[psk_length] = '\0';
        parser->changeValue("psk", pskString);
        delete [] pskString;

        string kaTypeString;
        switch( ka_type ){
                case KEY_MGMT_METHOD_MIKEY_DH:
                        kaTypeString = "dh";
                        break;
                case KEY_MGMT_METHOD_MIKEY_PSK:
                        kaTypeString = "psk";
                        break;
                case KEY_MGMT_METHOD_MIKEY_PK:
                        kaTypeString = "pk";
        }

        parser->changeValue("ka_type", kaTypeString);
 
	/***********************************************************
         * Certificate settings
         ***********************************************************/

        /* Update the certificate part of the configuration file */
        cert->lock();
        cert->init_index();
        MRef<certificate *> certItem = cert->get_next();

        /* The first element is the personal certificate, the next ones
         * are saved as certificate_chain */
        if( !certItem.isNull() ){
                parser->changeValue("certificate",certItem->get_file());
                parser->changeValue("private_key",certItem->get_pk_file());
                certItem = cert->get_next();
        }

        uint32_t i = 0;

 	while( !certItem.isNull() ){
                parser->changeValue("certificate_chain["+itoa(i)+"]",
                                certItem->get_file() );
                i++;
                certItem = cert->get_next();
        }

        cert->unlock();

 	/* CA database saved in the config file */
        uint32_t iFile = 0;
        uint32_t iDir  = 0;
        cert_db->lock();
        cert_db->init_index();
        ca_db_item * caDbItem = cert_db->get_next();

        while( caDbItem != NULL ){
                switch( caDbItem->type ){
                        case CERT_DB_ITEM_TYPE_FILE:
                                parser->changeValue("ca_file["+itoa(iFile)+"]",
                                        caDbItem->item);
                                iFile ++;
                                break;
                        case CERT_DB_ITEM_TYPE_DIR:
                                parser->changeValue("ca_dir["+itoa(iDir)+"]",
                                        caDbItem->item);
                                iDir ++;
                                break;
                }

                caDbItem = cert_db->get_next();
        }
        
	cert_db->unlock();
}

void SipDialogSecurityConfig::load( XMLFileParser * parser ){

	secured = parser->getValue("secured","no")=="yes";
	use_srtp = parser->getValue("use_srtp","no")=="yes";		
	use_ipsec = parser->getValue("use_ipsec","no")=="yes";
	
	dh_enabled   = parser->getValue("dh_enabled","no")=="yes";
	psk_enabled  = parser->getValue("psk_enabled","no")=="yes";
	check_cert   = parser->getValue("check_cert","no")=="yes";

	if( parser->getValue("ka_type", "psk") == "psk" )
		ka_type = KEY_MGMT_METHOD_MIKEY_PSK;
	
	else if( parser->getValue("ka_type", "psk") == "dh" )
		ka_type = KEY_MGMT_METHOD_MIKEY_DH;


	else if( parser->getValue("ka_type", "psk") == "pk" )
		ka_type = KEY_MGMT_METHOD_MIKEY_PK;

	else{
		ka_type = KEY_MGMT_METHOD_MIKEY_PSK;
#ifdef DEBUG_OUTPUT
		merr << "Invalid KA type in config file, default to PSK"<<end;
#endif
	}

	string pskString = parser->getValue("psk","Unspecified PSK");
	psk_length = (int)pskString.size();
	psk = new unsigned char[psk_length];

	memcpy( psk, pskString.c_str(), psk_length );

	/****************************************************************
	 * Certificate settings 
	 ****************************************************************/

	string certFile = parser->getValue("certificate","");
	string privateKeyFile = parser->getValue("private_key","");
	
	cert = new certificate_chain();

	if( certFile != "" ){
		certificate * cert=NULL;

		try{
			cert = new certificate( certFile );
			this->cert->add_certificate( cert );
		}
		catch( certificate_exception * ){
			merr << "Could not open the given certificate " << certFile <<end;
		}

		if( privateKeyFile != "" ){

			try{
				cert->set_pk( privateKeyFile );
			}
			catch( certificate_exception_pkey * ){
				merr << "The given private key " << privateKeyFile << " does not match the certificate"<<end;
			}

			catch( certificate_exception *){
				merr << "Could not open the given private key "<< privateKeyFile << end;
			}
		}
	}

	uint32_t iCertFile = 0;
	certFile = parser->getValue("certificate_chain[0]","");

	while( certFile != "" ){
		try{
			certificate * cert = new certificate( certFile );
			this->cert->add_certificate( cert );
		}
		catch( certificate_exception *){
			merr << "Could not open the given certificate" << end;
		}
		iCertFile ++;
		certFile = parser->getValue("certificate_chain["+itoa(iCertFile)+"]","");

	}

	cert_db = new ca_db();
	iCertFile = 0;
	certFile = parser->getValue("ca_file[0]","");

	while( certFile != ""){
		try{
			cert_db->add_file( certFile );
		}
		catch( certificate_exception *){
			merr << "Could not open the CA certificate" << end;
		}
		iCertFile ++;
		certFile = parser->getValue("ca_file["+itoa(iCertFile)+"]","");

	}
	iCertFile = 0;

	certFile = parser->getValue("ca_dir[0]","");

	while( certFile != ""){
		try{
			cert_db->add_directory( certFile );
		}
		catch( certificate_exception *){
			merr << "Could not open the CA certificate directory " << certFile << end;
		}
		iCertFile ++;
		certFile = parser->getValue("ca_dir["+itoa(iCertFile)+"]","");
	}
}
			

void SipDialogSecurityConfig::useIdentity( MRef<SipIdentity *> identity ){
	identity->lock();
	secured = identity->securitySupport;
	identity->unlock();
}
